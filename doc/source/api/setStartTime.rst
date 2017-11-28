#CAPTION#
setStartTime
------------

This function defines the start time of a given composite model.
#END#

#LUA#
.. code-block:: lua

  setStartTime(model, startTime)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "startTime", "Number", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setStartTime(void* model, double startTime);
#END#

#DESCRIPTION#
#END#
