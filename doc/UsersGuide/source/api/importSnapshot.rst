#CAPTION#
importSnapshot
--------------

Loads a snapshot to restore a previous model state. The model must be in virgin model state, which means it must not be instantiated.
#END#

#LUA#
.. code-block:: lua

  status = oms_importSnapshot(cref, snapshot)

#END#

#PYTHON#
.. code-block:: python

  status = oms.importSnapshot(cref, snapshot)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_importSnapshot(const char* cref, const char* snapshot);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_importSnapshot(cref, snapshot);

#END#

#DESCRIPTION#
#END#
