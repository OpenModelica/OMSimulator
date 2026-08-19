# OMSimulator Python package

The OMSimulator Python package provides the Python bindings for OMSimulator.

The package can be installed in two ways:

1. **Standard installation** — the package downloads the appropriate OMSimulator
   binaries from pip repository which is part of pip release

2. **Local installation** — the package is prepared from the OMSimulator build and
   install tree without downloading any binaries.

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