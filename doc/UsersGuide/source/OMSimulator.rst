.. index:: OMSimulator

OMSimulator
===========

OMSimulator is a command line wrapper for the OMSimulatorLib library.

.. index:: OMSimulator; Flags

OMSimulator Flags
-----------------

A brief description of all command line flags will be displayed using
:code:`OMSimulator --help`:

.. literalinclude :: omsimulator-help.inc
   :language: bash

To use flag :code:`logLevel` with option debug (:code:`--logLevel=1`)
or debug+trace (:code:`--logLevel=2`) one needs to build OMSimulator
with debug configuration enabled. Refer to the `OMSimulator README on GitHub <https://github.com/OpenModelica/OMSimulator/blob/master/README.md>`_
for further instructions.

.. index:: OMSimulator; Examples

Examples
--------

.. code-block:: bash

  OMSimulator --timeout 180 example.lua
