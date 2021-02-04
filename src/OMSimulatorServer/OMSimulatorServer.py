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

def _main():
  # parse command-line arguments
  parser = argparse.ArgumentParser(description='OMS-SERVER', allow_abbrev=False)
  parser.add_argument('--connect', default=None)
  args = parser.parse_args()

  # do something else
  logging.info('STOP')

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
