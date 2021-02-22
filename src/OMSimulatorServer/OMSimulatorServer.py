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
  def __init__(self, args):
    self._alive = True
    self._context = zmq.Context()
    self._mutex = threading.Lock()
    self._pause = args.interactive
    self._socket_rep = None
    self._socket_sub = None
    self._thread = None

    oms.setCommandLineOption(' '.join(list(map((lambda x: x[1:-1]), args.option))))
    oms.setLoggingLevel(args.logLevel)

    if args.temp:
      oms.setTempDirectory(args.temp)

    self._model = oms.importFile(args.model)

    if args.result_file:
      self._model.resultFile = args.result_file

    self.print('OMS Server {}'.format(__version__))
    self.print('ZMQ {}'.format(zmq.zmq_version()))

    if args.interactive and not args.endpoint_rep:
      self.print('flag --endpoint-rep is mandatory in interactive simulation mode')
      sys.exit(1)

    # connet the REP socket
    if args.endpoint_rep:
      self._socket_rep = self._context.socket(zmq.REP)  #pylint: disable=no-member
      self._socket_rep.connect(args.endpoint_rep)
      self._socket_rep.RCVTIMEO = 1000  #in milliseconds
      self.print('REP socket connected to {}'.format(args.endpoint_rep))
      self._thread = threading.Thread(target=self._main, daemon=True)
      self._thread.start()

    # connect the PUB socket
    if args.endpoint_pub:
      socket_sub = self._context.socket(zmq.PUB)  #pylint: disable=no-member
      socket_sub.connect(args.endpoint_pub)
      self.print('PUB socket connected to {}'.format(args.endpoint_pub))

  def print(self, msg):
    print('server:  {}'.format(msg), flush=True)

  def pub_msg(self, topic, msg: dict):
    if self._socket_sub:
      msg_ = mogrify(topic, msg)
      self._socket_sub.send_string(msg_)

  def _main(self):
    alive = True
    while alive:
      try:
        msg = self._socket_rep.recv_json()
      except zmq.error.Again as error:
        self.print('recv: ' + str(error))
        continue

      fcn = msg['fcn'] if 'fcn' in msg else ''
      ok = False

      if 'simulation' == fcn:
        arg = msg['arg']
        if 'pause' == arg:
          with self._mutex:
            self._pause = True
        elif 'continue' == arg:
          with self._mutex:
            self._pause = False
        elif 'end' == arg:
          alive = False
          with self._mutex:
            self._alive = False

      answer = json.dumps({'status': 'ack' if ok else 'nack', 'fcn': fcn})
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
      progress = math.floor((time_-startTime) / (stopTime-startTime) * 100)
      self.pub_msg('status', {'progress': progress})

      if self._pause:
        time.sleep(0.3)
      else:
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

  server = Server(args)
  server.run()

if __name__ == '__main__':
  _main()
