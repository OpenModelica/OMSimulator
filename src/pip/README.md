# OMSimulator Python package

The OMSimulator Python package provides the Python bindings for OMSimulator.

The package can be installed in two ways:

1. **Standard installation** — the package downloads the appropriate OMSimulator
   binaries from pip repository which is part of pip release

2. **Local installation** — the package is prepared from the OMSimulator build and
   install tree without downloading any binaries.

# Standard installation
```bash
pip install OMSimulator
```

The package also contains the standalone OMSimulatorGui. Its own requirements
(PySide6, pyqtgraph and scipy) are not installed by default; install them with
the `gui` extra, which is also enough when OMSimulator is already installed:

```bash
pip install OMSimulator[gui]
```

Then start the GUI with:

```bash
python -m OMSimulatorGui
```

If the requirements are missing, the GUI says so and prints this install command.
# Building the source distribution (sdist)

The sdist is built by CMake; the binaries are not part of it, they are downloaded
when the package is installed.

```bash
cmake -S . -B build -DOMS_ENABLE_PIP=ON
cmake --build build
```

The sdist is written to `src/pip/install/dist/`. Building it needs the Python
`build` module (`pip install build`). Test it in a fresh virtual environment:

```bash
python -m venv oms-pip-test
oms-pip-test/bin/python -m pip install "src/pip/install/dist/<file>.tar.gz[gui]"
```

# OMSimulator-local-pip

The local pip installation builds the OMSimulator Python package from the
locally installed OMSimulator binaries.

Unlike the standard pip installation, the local installation does not download
OMSimulator binaries from the internet. The Python package is prepared directly
from the current CMake build and install tree.

This is useful when developing OMSimulator or testing Python bindings against
a locally built version.

```text
src/pip/install/     prepared local pip package
pyproject.toml       Python package configuration
OMSimulator/         Python package and native OMSimulator library
OMSimulatorGui/      the standalone GUI
schema/              OMSimulator schema files
```

# Running the local pip installation
Configure the project with OMS_ENABLE_LOCAL_PIP=ON

```bash
cmake -S . -B build -DOMS_ENABLE_LOCAL_PIP=ON
cmake --build build
cmake --install build
```

# Install the local Python package

After the CMake installation has prepared the package, it can be installed with:

```bash
cd src/pip/install
python -m pip install .
```

Alternatively, CMake provides a dedicated pip_install target:
```bash
cmake --build build --target pip_install
```