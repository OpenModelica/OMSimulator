#CAPTION#
setStopTime
-----------

This function defines the stop time of a given composite model.
#END#

#LUA#
.. code-block:: lua

  setStopTime(model, stopTime)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "stopTime", "Number", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setStopTime(void* model, double stopTime);
#END#

#DESCRIPTION#
#END#
