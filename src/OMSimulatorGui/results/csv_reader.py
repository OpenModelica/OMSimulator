# This file is part of OpenModelica.
#
# Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
# c/o Linköpings universitet, Department of Computer and Information Science,
# SE-58183 Linköping, Sweden.
#
# All rights reserved.
#
# THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
# THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
# VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
#
# The OpenModelica software and the OSMC (Open Source Modelica Consortium)
# Public License (OSMC-PL) are obtained from OSMC, either from the above
# address, from the URLs:
# http://www.openmodelica.org or
# https://github.com/OpenModelica/ or
# http://www.ida.liu.se/projects/OpenModelica,
# and in the OpenModelica distribution.
#
# GNU AGPL version 3 is obtained from:
# https://www.gnu.org/licenses/licenses.html#GPL
#
# This program is distributed WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
# IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
#
# See the full OSMC Public License conditions for more details.

'''Reads OMSimulator's self-describing CSV result files: a header row of
signal names (including 'time'), one column per signal, one row per time
sample. Needs no third-party dependency -- the stdlib csv module is enough.'''

import csv

import numpy as np


def readCsv(path: str) -> dict[str, tuple[np.ndarray, np.ndarray]]:
  with open(path, newline='') as f:
    reader = csv.reader(f)
    header = [h.strip() for h in next(reader)]
    columns: list[list[float]] = [[] for _ in header]
    for row in reader:
      for i, value in enumerate(row):
        columns[i].append(float(value))

  data = [np.asarray(column, dtype=float) for column in columns]
  timeIndex = header.index('time') if 'time' in header else 0
  timeSeries = data[timeIndex]
  return {name: (timeSeries, values) for name, values in zip(header, data)}
