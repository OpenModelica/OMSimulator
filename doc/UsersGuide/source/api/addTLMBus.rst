#CAPTION#
simulate_asynchronous
---------------------

Adds a TLM bus.
#END#

#LUA#
.. code-block:: lua

  status = oms3_addTLMBus(cref, domain, dimensions, interpolation)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_addTLMBus(cref, domain, dimensions, interpolation)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_addTLMBus(const char* cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation);

#END#

#DESCRIPTION#
#END#
