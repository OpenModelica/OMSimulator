#CAPTION#
addTLMConnection
----------------

Connects two TLM connectors.
#END#

#LUA#
.. code-block:: lua

  status = oms3_addTLMConnection(crefA, crefB, delay, alpha, linearimpedance, angularimpedance)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_addTLMConnection(crefA, crefB, delay, alpha, linearimpedance, angularimpedance)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_addTLMConnection(const char* crefA, const char* crefB, double delay, double alpha, double linearimpedance, double angularimpedance);

#END#

#DESCRIPTION#
#END#
