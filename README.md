# OMSimulator [![License: OSMC-PL](https://img.shields.io/badge/license-OSMC--PL-lightgrey.svg)](OSMC-License.txt)

The OpenModelica FMI & SSP-based co-simulation environment.

## Overview

OMSimulator can be used as:

- **Stand-alone application**
- **C library** for integration into custom applications
- **Python package** (`pip install OMSimulator`)

OMSimulator is also included with the OpenModelica installer, which includes OMEdit, a graphical editor.

* [OpenModelica](https://openmodelica.org/)
* [Standalone package](https://build.openmodelica.org/omsimulator/)

## Documentation

Latest documentation:

- [User Guide PDF](https://openmodelica.org/doc/OMSimulator/master/OMSimulator/UsersGuide/pdf/OMSimulator.pdf)
- [User Guide HTML](https://openmodelica.org/doc/OMSimulator/master/OMSimulator/UsersGuide/html/)
- [OMSimulatorLib Doxygen](https://openmodelica.org/doc/OMSimulator/master/OMSimulator/doxygen/html/index.html)

## Testing and Coverage

OMSimulator tests are automatically run on Jenkins. See the [latest test reports](https://test.openmodelica.org/jenkins/job/OMSimulator/job/master/lastSuccessfulBuild/testReport/).

![Coverage Report](https://libraries.openmodelica.org/branches/history/master-fmi/Modelica_4.0.0-recent.svg)

The plot above shows the recent coverage report for the **Modelica 4.0.0** library over the last 7 runs.

> **Note**: Issues may be related to OMSimulator itself or the FMI export process in OpenModelica.

## License

This project is licensed under the OSMC Public License. See [OSMC-License.txt](OSMC-License.txt) for details.
