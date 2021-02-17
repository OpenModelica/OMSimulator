#!/usr/bin/env python3
'''OMSimulator Server for interactive simulation of SSP files.'''

import argparse
import datetime
import json
import logging
import math
import os

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

def mogrify(topic: str, msg: dict):
  '''Combines a topic identifier and a json representation of a dictionary'''
  return '%s %s' % (topic, json.dumps(msg))

def pub_msg(socket, topic, msg: dict):
  if socket:
    msg_ = mogrify(topic, msg)
    socket.send_string(msg_)
    #logging.info(msg_)

def _main():
  # parse command-line arguments
  parser = argparse.ArgumentParser(description='OMS-SERVER', allow_abbrev=False)
  parser.add_argument('--endpoint-pub', default=None, help='define the endpoint for the pub/sub communication')
  parser.add_argument('--endpoint-rep', default=None, help='define the endpoint for the req/rep communication')
  parser.add_argument('--model', default=None, required=True, help='define the model to simulate')
  parser.add_argument('--logLevel', default=0, type=int, help='defines the logging level')
  parser.add_argument('--option', default='', help='defines optional command line options')
  parser.add_argument('--result-file', default=None, help='defines whether and if so to which file the results will be written')
  parser.add_argument('--temp', default='./temp/', help='defines the temp directory')
  args = parser.parse_args()

  logging.info('OMS Server {}'.format(__version__))
  logging.info('ZMQ {}'.format(zmq.zmq_version()))

  context = zmq.Context()

  # connet the REP socket
  if args.endpoint_rep:
    socket_rep = context.socket(zmq.REP)  #pylint: disable=no-member
    socket_rep.connect(args.endpoint_rep)
    socket_rep.RCVTIMEO = 1000  #in milliseconds
    logging.info('REP socket connected to {}'.format(args.endpoint_rep))
  else:
    socket_rep = None

  # connect the PUB socket
  if args.endpoint_pub:
    socket_sub = context.socket(zmq.PUB)  #pylint: disable=no-member
    socket_sub.connect(args.endpoint_pub)
    logging.info('PUB socket connected to {}'.format(args.endpoint_pub))
  else:
    socket_sub = None

  oms.setCommandLineOption(args.option)
  oms.setLoggingLevel(args.logLevel)
  oms.setTempDirectory(args.temp)
  model = oms.importFile(args.model)

  #print(model.systemType)

  if args.result_file:
    model.resultFile = args.result_file

  pub_msg(socket_sub, 'status', {'progress': 0})

  startTime = model.startTime
  stopTime = model.stopTime

  model.instantiate()
  model.initialize()

  while model.time < stopTime:
    progress = math.floor((model.time-startTime) / (stopTime-startTime) * 100)
    pub_msg(socket_sub, 'status', {'progress': progress})
    model.doStep()

  model.terminate()
  model.delete()
  pub_msg(socket_sub, 'status', {'progress': 100})

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

  _main()
