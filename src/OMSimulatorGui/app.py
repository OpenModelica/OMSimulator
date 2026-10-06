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

import sys

# The GUI's own dependencies come with `pip install OMSimulator[gui]`; the
# files themselves are always installed, so a missing one is reported as such
# in main() instead of failing with a bare ModuleNotFoundError at import time.
_GUI_DEPENDENCIES = ('PySide6', 'pyqtgraph', 'scipy')
_missingDependency = None
try:
  from PySide6.QtWidgets import QApplication

  from OMSimulatorGui.main_window import MainWindow
except ImportError as e:
  if (e.name or '').split('.')[0] not in _GUI_DEPENDENCIES:
    raise
  _missingDependency = e.name.split('.')[0]


def main(argv: list[str] | None = None) -> int:
  '''Entry point: creates the QApplication and MainWindow, runs the event loop.

  argv may optionally contain a single .ssp path to open on startup.
  '''
  if _missingDependency is not None:
    print(f"The OMSimulator GUI needs the Python package '{_missingDependency}', which is not installed.\n"
          "Install the GUI dependencies with:\n\n"
          "  pip install OMSimulator[gui]", file=sys.stderr)
    return 1

  argv = list(sys.argv) if argv is None else [sys.argv[0], *argv]

  app = QApplication(argv)
  app.setApplicationName('OMSimulatorGui')
  app.setOrganizationName('Open Source Modelica Consortium')

  window = MainWindow()
  window.show()

  # optional startup file: `python -m OMSimulatorGui model.ssp`
  positional = [a for a in argv[1:] if not a.startswith('-')]
  if positional:
    window.openFile(positional[0])

  return app.exec()
