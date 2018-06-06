#CAPTION#
compareSimulationResults
------------------------

This function compares a given signal of two result files within absolute and relative tolerances.
#END#

#LUA#
.. code-block:: lua

  compareSimulationResults(filenameA, filenameB, var, relTol, absTol)

The following table describes the input values:

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "filenameA", "String", "Name of first result file to compare."
  "filenameB", "String", "Name of second result file to compare."
  "var", "String", "Name of signal to compare."
  "relTol", "Number", "Relative tolerance."
  "absTol", "Number", "Absolute tolerance."

The following table describes the return values:

.. csv-table::
  :header: "Type", "Description"
  :widths: 10, 65

  "Integer", "1 if the signal is considered as equal, 0 otherwise"

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  int oms2_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);

#END#

#DESCRIPTION#
#END#
