#CAPTION#
addTLMConnection
----------------

Connects two TLM connectors.
#END#

#LUA#
.. code-block:: lua

  status = oms_addTLMConnection(crefA, crefB, delay, alpha, linearimpedance, angularimpedance)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addTLMConnection(crefA, crefB, delay, alpha, linearimpedance, angularimpedance)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addTLMConnection(const char* crefA, const char* crefB, double delay, double alpha, double linearimpedance, double angularimpedance);

#END#

#OMC#
.. code-block:: modelica

  status := oms_addTLMConnection(crefA, crefB, delay, alpha, linearimpedance, angularimpedance);

#END#

#DESCRIPTION#
#END#
