#CAPTION#
loadSnapshot
------------

Loads a snapshot to restore a previous model state.
#END#

#LUA#
.. code-block:: lua

  status = oms_loadSnapshot(cref, snapshot)

#END#

#PYTHON#
.. code-block:: python

  # not available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_loadSnapshot(const char* cref, const char* snapshot);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_loadSnapshot(cref, snapshot);

#END#

#DESCRIPTION#
#END#
