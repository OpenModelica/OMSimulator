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

'''Reads the Dymola/OpenModelica-convention MAT-v4 trajectory result files
OMSimulator's own .mat writer produces. OMSimulator's writer is a separate
codebase from omc's, so this was verified empirically against a real
generated file (see the M6 planning spike) rather than assumed -- it turns
out to use exactly the same on-disk convention as omc's own writer:

- `Aclass`: a 4-line header; the *last* line is 'binTrans' or 'binNormal'
  (line 3 is a blank/reserved line -- easy to miscount) and says whether
  `data_1`/`data_2` below are stored variable-major (one row per variable)
  or sample-major (one row per time sample).
- `name`/`description`: char matrices, always stored one name per *column*
  regardless of the binTrans/binNormal flag (that flag only governs the
  data matrices) -- confirmed against the spike file, whose `name` matrix
  only decodes into valid variable names when read column-wise.
- `dataInfo[:, j]` for variable j is `(matrixNumber, index, _, _)`:
    matrixNumber <= 0  -> this *is* the time vector (data_2's row/column 0)
    matrixNumber == 1  -> a constant, stored once in `data_1` at abs(index)-1
    matrixNumber == 2  -> a trajectory, stored in `data_2` at abs(index)-1
  A negative `index` means the stored value/series must be negated (alias
  variables can reference the same physical signal with flipped sign).

Verified against the spike file by cross-checking extracted values for a
handful of signals (including a non-trivial one that is 0 for most of the
run and only becomes nonzero near the end) against the same simulation's
self-describing CSV output -- see feedback_headless_pyside_testing.md-style
verification notes in the project memory.'''

import numpy as np
from scipy.io import loadmat


def _decodeCharMatrix(matrix: np.ndarray) -> list[str]:
  '''`matrix` is (maxLength, count), one string per column.'''
  return [''.join(matrix[:, j]).rstrip('\x00').rstrip() for j in range(matrix.shape[1])]


def readMat(path: str) -> dict[str, tuple[np.ndarray, np.ndarray]]:
  '''Returns {signal_name: (times, values)}, including 'time' itself
  (mapped to itself as both times and values) so callers can treat it like
  any other signal if they want to.'''
  raw = loadmat(path, chars_as_strings=False, squeeze_me=False)

  # Aclass is 4 lines: ['Atrajectory', version, '' (reserved), 'binTrans'/
  # 'binNormal'] -- the transpose flag is the *last* line, not the 3rd
  # (verified against the spike file: aclass[2] is the blank reserved line).
  aclass = [''.join(row).rstrip('\x00').rstrip() for row in raw['Aclass']]
  transposed = bool(aclass) and aclass[-1] == 'binTrans'

  names = _decodeCharMatrix(raw['name'])

  data1 = raw.get('data_1')
  data2 = raw.get('data_2')
  if not transposed:
    # Normalize to the same "rows = variables, columns = samples" shape the
    # extraction below assumes, regardless of how this particular file
    # stored it.
    data1 = data1.T if data1 is not None else None
    data2 = data2.T if data2 is not None else None

  dataInfo = raw['dataInfo']
  timeSeries = np.asarray(data2[0, :], dtype=float)

  signals: dict[str, tuple[np.ndarray, np.ndarray]] = {}
  for j, name in enumerate(names):
    matrixNumber = int(dataInfo[0, j])
    index = int(dataInfo[1, j])
    sign = -1.0 if index < 0 else 1.0
    index = abs(index) - 1
    if matrixNumber <= 0:
      values = timeSeries
    elif matrixNumber == 1:
      values = np.full_like(timeSeries, sign * float(data1[index, 0]))
    else:
      values = sign * np.asarray(data2[index, :], dtype=float)
    signals[name] = (timeSeries, values)
  return signals
