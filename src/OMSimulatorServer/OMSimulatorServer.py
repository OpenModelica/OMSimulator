#!/usr/bin/env python3
'''OMSimulator Server for interactive simulation of SSP files.'''

import argparse
import json
import math
import sys
import threading
import time

import OMSimulator as oms
import zmq

__author__ = 'Lennart Ochel <lennart.ochel@ri.se>'
__version__ = oms.__version__
__copyright__ = '''\
Copyright (c) 2018-CurrentYear, Open Source Modelica Consortium (OSMC),
c/o Linköpings universitet, Department of Computer and Information Science,
SE-58183 Linköping, Sweden.'''
__status__ = 'development'

def close_socket_gracefully(socket):
  '''graceful termination'''
  socket.setsockopt(zmq.LINGER, 0) # to avoid hanging infinitely
  socket.close()

def mogrify(topic: str, msg: dict):
  '''Combines a topic identifier and a json representation of a dictionary'''
  return '%s %s' % (topic, json.dumps(msg))

class Server:
  def __init__(self, model, result_file, interactive, endpoint_pub, endpoint_rep):
    self.print('OMS Server {}'.format(__version__))
    self.print('ZMQ {}'.format(zmq.zmq_version()))

    self._alive = True
    self._context = zmq.Context()
    self._model = oms.importFile(model)
    self._mutex = threading.Lock()
    self._pause = interactive
    self._socket_rep = None
    self._socket_pub = None
    self._thread = None
    self._activeSignals = set()

    if result_file:
      self._model.resultFile = result_file

    # extract all available signals
    self._signals = self._model.getAllVariables()

    if interactive and not endpoint_rep:
      self.print('flag --endpoint-rep is mandatory in interactive simulation mode')
      sys.exit(1)

    # connect the PUB socket
    if endpoint_pub:
      self._socket_pub = self._context.socket(zmq.PUB)  #pylint: disable=no-member
      self._socket_pub.connect(endpoint_pub)
      self.print('PUB socket connected to {}'.format(endpoint_pub))

    # connet the REP socket
    if endpoint_rep:
      self._socket_rep = self._context.socket(zmq.REP)  #pylint: disable=no-member
      self._socket_rep.connect(endpoint_rep)
      self._socket_rep.RCVTIMEO = 1000  #in milliseconds
      self.print('REP socket connected to {}'.format(endpoint_rep))
      self._thread = threading.Thread(target=self._main, daemon=True)
      self._thread.start()

  def print(self, msg):
    print('server:  {}'.format(msg), flush=True)

  def pub_msg(self, topic, msg: dict):
    if self._socket_pub:
      msg_ = mogrify(topic, msg)
      try:
        self._socket_pub.send_string(msg_)
      except zmq.error.ZMQError as error:
        self.print('Error publishing: ' + str(error))

  def _main(self):
    alive = True
    while alive:
      try:
        msg = self._socket_rep.recv_json()
      except zmq.error.Again as error:
        #self.print('recv: ' + str(error))
        continue

      fcn = msg['fcn'] if 'fcn' in msg else ''
      answer = {'status': 'nack', 'error': 'unknown'}

      if 'simulation' == fcn:
        arg = msg['arg']
        if 'pause' == arg:
          with self._mutex:
            self._pause = True
          answer = {'status': 'ack'}
        elif 'continue' == arg:
          with self._mutex:
            self._pause = False
          answer = {'status': 'ack'}
        elif 'end' == arg:
          alive = False
          with self._mutex:
            self._alive = False
          answer = {'status': 'ack'}
      elif 'signals' == fcn:
        arg = msg['arg']
        if 'available' == arg:
          answer = {'status': 'ack', 'result': self._signals}
        elif 'enable' == arg:
          self._activeSignals.add(msg['cref'])
          answer = {'status': 'ack'}
        elif 'disable' == arg:
          self._activeSignals.remove(msg['cref'])
          answer = {'status': 'ack'}

      try:
        self._socket_rep.send_json(answer)
      except zmq.error.ZMQError as error:
        self.print('send: ' + str(error))
      else:
        self.print(msg)

  def run(self):
    self.pub_msg('status', {'progress': 0})

    time_ = self._model.time
    startTime = self._model.startTime
    stopTime = self._model.stopTime
    self._model.instantiate()
    self._model.initialize()

    while True:
      if self._pause:
        time.sleep(0.3)
      else:
        try:
          progress = math.floor((time_-startTime) / (stopTime-startTime) * 100)
        except ZeroDivisionError:
          progress = 100

        self.pub_msg('status', {'progress': progress})
        if self._activeSignals:
          results = {'time': time_}
          for signal in self._activeSignals:
            type_ = self._signals[signal]['type']
            if type_ == 'Real':
              results[signal] = oms.getReal(signal)
            elif type_ == 'Integer':
              results[signal] = oms.getInteger(signal)
            elif type_ == 'Boolean':
              results[signal] = oms.getBoolean(signal)
          self.pub_msg('results', results)
        with self._mutex:
          self._model.doStep()
          time_ = self._model.time
          if time_ >= stopTime and self._alive:
            break

    self._model.terminate()
    self._model.delete()
    self.pub_msg('status', {'progress': 100})

def _main():
  # parse command-line arguments
  parser = argparse.ArgumentParser(description='OMS-SERVER', allow_abbrev=False)
  parser.add_argument('--model', default=None, required=True, help='define the model to simulate')
  parser.add_argument('--endpoint-pub', default=None, help='define the endpoint for the pub/sub communication')
  parser.add_argument('--endpoint-rep', default=None, help='define the endpoint used for the interactive simulation communication')
  parser.add_argument('--interactive', action='store_true', help='enables the interactive simulation')
  parser.add_argument('--logLevel', default=0, type=int, help='defines the logging level')
  parser.add_argument('--option', default='', nargs='+', help='defines optional command line options')
  parser.add_argument('--result-file', default=None, help='defines the result file')
  parser.add_argument('--temp', default=None, help='defines the temp directory')
  args = parser.parse_args()

  oms.setCommandLineOption(' '.join(list(map((lambda x: x[1:-1]), args.option))))
  oms.setLoggingLevel(args.logLevel)

  if args.temp:
    oms.setTempDirectory(args.temp)

  server = Server(args.model, args.result_file, args.interactive, args.endpoint_pub, args.endpoint_rep)
  # run simulation thread
  server.run()

if __name__ == '__main__':
  _main()
