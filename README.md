# OMSimulator [![License: OSMC-PL](https://img.shields.io/badge/license-OSMC--PL-lightgrey.svg)](OSMC-License.txt)

The OpenModelica FMI & SSP-based co-simulation environment.

### Branch Overview

- **`master`**: Development branch for active, unstable updates.
- **`maintenance/v2.1`**: Stable branch for version 2.1 with patch updates (latest release: **v2.1.3**).
- **`maintenance/v2.0`**: Stable branch for version 2.0 with patch updates (latest release: **v2.0.1**).

### Overview

OMSimulator can be used as:

- **Stand-alone application**
- **C library** for integration into custom applications
- **Python package** (`pip install OMSimulator`)

OMSimulator is also included with the OpenModelica installer, which includes OMEdit, a graphical editor.

* [OpenModelica](https://openmodelica.org/)
* [Standalone package](https://build.openmodelica.org/omsimulator/)

### Documentation

Latest documentation:

- [User Guide PDF](https://openmodelica.org/doc/OMSimulator/master/OMSimulator/UsersGuide/pdf/OMSimulator.pdf)
- [User Guide HTML](https://openmodelica.org/doc/OMSimulator/master/OMSimulator/UsersGuide/html/)
- [OMSimulatorLib Doxygen](https://openmodelica.org/doc/OMSimulator/master/OMSimulator/doxygen/html/index.html)

### Testing and Coverage

OMSimulator's coverage tests provide insights across a wide range of publicly available libraries. For an overview of all tested libraries and their detailed reports, visit the [Coverage Test Overview](https://libraries.openmodelica.org/branches/master-fmi/).

- [Modelica Standard Library (MSL4) Detailed Report](https://libraries.openmodelica.org/branches/master-fmi/Modelica_4.0.0/Modelica_4.0.0.html)
- [Modelica Standard Library (MSL4) Coverage Graph](https://libraries.openmodelica.org/branches/history/master-fmi/Modelica_4.0.0-recent.svg)

> **Note**: Issues may be attributed to OMSimulator itself or the FMI export in OpenModelica.

### Build Instructions

To compile the project, please follow the steps outlined in the [BUILD.md](BUILD.md) file.

### License

This project is licensed under the OSMC Public License. See [OSMC-License.txt](OSMC-License.txt) for details.
