#CAPTION#
addConnector
------------

Adds a connector to a given component.
#END#

#LUA#
.. code-block:: lua

  status = oms3_addConnector(cref, causality, type)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_addConnector(cref, causality, type)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_addConnector(const char* cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);

#END#

#DESCRIPTION#
#END#
