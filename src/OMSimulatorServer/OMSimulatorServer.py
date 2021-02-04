#!/usr/bin/env python3
'''OMSimulator Server for interactive simulation of SSP files.'''

import argparse
import datetime
import json
import logging
import os

import OMSimulator
import zmq

__author__ = 'Lennart Ochel <lennart.ochel@ri.se>'
__version__ = OMSimulator.__version__
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
    print('recv: ' + str(error))
    return

  # answer = msg
  answer = json.loads(msg)
  answer = json.dumps(answer)

  try:
    socket.send_json(answer)
  except zmq.error.ZMQError as error:
    print('send: ' + str(error))
  else:
    print(msg)

def _main():
  # parse command-line arguments
  parser = argparse.ArgumentParser(description='OMS-SERVER', allow_abbrev=False)
  parser.add_argument('--port', default='1234', help='define the port for the req/rep comunication')
  args = parser.parse_args()

  # bind the socket
  context = zmq.Context()

  socket = context.socket(zmq.REP)  #pylint: disable=no-member
  socket.bind('tcp://*:{}'.format(args.port))
  socket.RCVTIMEO = 1000  #in milliseconds
  logging.info('Bound to port {}'.format(args.port))

  while socket:
    try:
      receive_and_reply(socket)
    except KeyboardInterrupt:
      close_socket_gracefully(socket)
      socket = None

if __name__ == '__main__':
  path = "log/"
  if not os.path.isdir(path):
    try:
      os.makedirs(path)
    except OSError:
      raise Exception("Creation of the log directory failed")

  filename = datetime.datetime.now().strftime('%Y%m%d-%H%M%S') + '-' + 'OMSimulator-Server' + '.log'
  format = '%(asctime)s: %(levelname)s [%(name)s] %(message)s'
  logging.basicConfig(filename=os.path.join(path, filename), level=logging.INFO, format=format)

  formatter = logging.Formatter(format)
  handler = logging.StreamHandler()
  handler.setFormatter(formatter)
  logging.getLogger().addHandler(handler)

  logging.info('OMS Server {}'.format(__version__))

  _main()
