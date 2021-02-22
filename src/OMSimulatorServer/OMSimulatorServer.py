#!/usr/bin/env python3
'''OMSimulator Server for interactive simulation of SSP files.'''

import argparse
import json
import math
import sys
import threading

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

def receive_and_reply(socket):
  try:
    msg = socket.recv_json()
  except zmq.error.Again as error:
    print('server:  recv: ' + str(error), flush=True)
    return

  # answer = msg
  answer = json.loads(msg)
  answer = json.dumps(answer)

  try:
    socket.send_json(answer)
  except zmq.error.ZMQError as error:
    print('server:  send: ' + str(error), flush=True)
  else:
    print('server:  {}'.format(msg), flush=True)

def mogrify(topic: str, msg: dict):
  '''Combines a topic identifier and a json representation of a dictionary'''
  return '%s %s' % (topic, json.dumps(msg))

class Server:
  def __init__(self, args):
    self._alive = False
    self._context = zmq.Context()
    self._mutex = threading.Lock()
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

    # connect the PUB socket
    if args.endpoint_pub:
      socket_sub = self._context.socket(zmq.PUB)  #pylint: disable=no-member
      socket_sub.connect(args.endpoint_pub)
      self.print('PUB socket connected to {}'.format(args.endpoint_pub))

  @property
  def alive(self):
    '''Check if server instance is alive'''
    with self._mutex:
      return self._alive

  def print(self, msg):
    print('server:  {}'.format(msg), flush=True)

  def pub_msg(self, topic, msg: dict):
    if self._socket_sub:
      msg_ = mogrify(topic, msg)
      self._socket_sub.send_string(msg_)

  def run(self):
    self.pub_msg('status', {'progress': 0})

    startTime = self._model.startTime
    stopTime = self._model.stopTime

    self._model.instantiate()
    self._model.initialize()

    while self._model.time < stopTime:
      progress = math.floor((self._model.time-startTime) / (stopTime-startTime) * 100)
      self.pub_msg('status', {'progress': progress})
      self._model.doStep()

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
