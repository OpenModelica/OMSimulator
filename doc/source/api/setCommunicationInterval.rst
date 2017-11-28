#CAPTION#
setCommunicationInterval
------------------------

This functions defines the communication interval of the given composite model.
#END#

#LUA#
.. code-block:: lua

  setCommunicationInterval(model, communicationInterval)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "communicationInterval", "Number", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setCommunicationInterval(void* model, double communicationInterval);
#END#

#DESCRIPTION#
#END#
