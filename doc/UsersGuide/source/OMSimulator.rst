.. index:: OMSimulator

OMSimulator
===========

OMSimulator is a command line interface for the OMSimulatorLib library.
It can be used to simulate Functional Mock-up Units (FMUs) and System
Structure and Parameterization (SSP) files, as well as execute dedicated
Python simulation scripts using the OMSimulator Python API.

.. index:: OMSimulator; Flags

OMSimulator Flags
-----------------

A brief description of all command line flags can be displayed using

:code:`OMSimulator --help`:

.. literalinclude:: omsimulator-help.inc
   :language: bash

To use the :code:`logLevel` flag with option debug (:code:`--logLevel=1`)
or debug+trace (:code:`--logLevel=2`), OMSimulator needs to be built with
debug configuration enabled. Refer to the
`OMSimulator README on GitHub
<https://github.com/OpenModelica/OMSimulator/blob/master/README.md>`_
for further instructions.

.. index:: OMSimulator; Examples

Examples
--------

OMSimulator supports several ways of running simulations.

Running a Python script
~~~~~~~~~~~~~~~~~~~~~~~

A dedicated Python script can be passed directly to OMSimulator. The script
can use the OMSimulator Python API to create and configure a simulation.

.. code-block:: bash

   OMSimulator example.py

This is useful when the simulation setup is created programmatically or
when a simulation needs additional Python logic.

Running an SSP file
~~~~~~~~~~~~~~~~~~~

An SSP file can be passed directly to OMSimulator. OMSimulator loads the
system structure from the SSP file and executes the simulation.

.. code-block:: bash

   OMSimulator example.ssp

Running an FMU
~~~~~~~~~~~~~~

An FMU can also be passed directly to OMSimulator.

.. code-block:: bash

   OMSimulator example.fmu

This provides a simple way to simulate an FMU without creating an SSP
system explicitly.
