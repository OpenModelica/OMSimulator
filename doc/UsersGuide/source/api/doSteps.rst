#CAPTION#
doSteps
-------

This function executes a given number of communication steps.
#END#

#LUA#
.. code-block:: lua

  doSteps(model, numberOfSteps);

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "numberOfSteps", "Number", "Number of communication steps that should be executed."
#END#

#CAPI#
.. code-block:: c

  oms_status_t oms_doSteps(const void* model, const int numberOfSteps);
#END#

#DESCRIPTION#
#END#
