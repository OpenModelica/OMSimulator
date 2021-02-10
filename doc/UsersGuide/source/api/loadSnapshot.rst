#CAPTION#
loadSnapshot
------------

Loads a snapshot to restore a previous model state. The model must be in virgin model state, which means it must not be instantiated.
#END#

#LUA#
.. code-block:: lua

  newCref, status = oms_loadSnapshot(cref, snapshot)

#END#

#PYTHON#
.. code-block:: python

  newCref, status = oms.loadSnapshot(cref, snapshot)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_loadSnapshot(const char* cref, const char* snapshot, char** newCref);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_loadSnapshot(cref, snapshot);

#END#

#DESCRIPTION#
#END#
