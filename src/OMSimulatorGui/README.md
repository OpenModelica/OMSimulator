# OMSimulatorGui

A lightweight, standalone PySide6 GUI for editing and simulating SSP models. It calls the
`OMSimulator` Python API (`SSP`/`SSD`/`System`/...) directly, in-process, for editing -- no omc
dependency, no server process. Simulation itself still runs in a separate subprocess, for crash
isolation from a misbehaving FMU.

```text
main_window.py            MainWindow -- owns the open models, wires tree/canvas/menus together
app.py, __main__.py       QApplication bootstrap (python -m OMSimulatorGui)
OMSimulatorGui.in/.bat    launcher scripts, installed alongside OMSimulator's own

models/                   QAbstractItemModel wrapping System.elements for the tree view
views/                    the system tree, the diagram canvas and its items, the results plot
dialogs/                  one file per modal dialog (Add System, Simulation Setup, Options, ...)
simulation/               sim_client.py (QProcess wrapper) + sim_worker.py (the subprocess itself)
results/                  .mat/.csv result-file readers, dispatched by extension
requirements.txt          PySide6/pyqtgraph/scipy -- kept out of src/pip's core dependency list
                          on purpose, so the CLI/API stay usable without a GUI toolkit installed
```

## Building and running

Built by default. To disable it (e.g. to skip its extra Python package
requirements below), configure with:

```bash
cmake -S . -B build -DOMS_ENABLE_OMSimulatorGui=OFF
cmake --build build --target install
```

It has no compiled component -- pure Python, installed as a sibling of the `OMSimulator` package
under `<install>/lib/`. Install its extra dependencies into whichever Python environment
OMSimulator itself runs under:

```bash
pip install -r requirements.txt
```

Then launch it via the installed launcher, or directly as a module:

```bash
OMSimulatorGui
python -m OMSimulatorGui
```

After editing a file here, a full `cmake --build . --target install` is only needed the first
time (or after adding/removing a file, since that changes `CMakeLists.txt`'s own
`install(FILES ...)` list). For every other edit, re-running just this target's own scoped install
is enough and much faster:

```bash
cmake -P src/OMSimulatorGui/cmake_install.cmake
```
