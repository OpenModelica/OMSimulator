#CAPTION#
setTolerance
------------

This function defines the tolerance of a given composite model.
#END#

#LUA#
.. code-block:: lua

  setTolerance(model, tolerance)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "tolerance", "Number", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setTolerance(void* model, double tolerance);
#END#

#DESCRIPTION#
The tolerance is used for solving algebraic loops on a global level, as well as for the tolerance that is forwarded to all the FMUs.
#END#
