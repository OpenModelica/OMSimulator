#CAPTION#
addTLMInterface
---------------

Adds an external model to a TLM composite model.
#END#

#LUA#
.. code-block:: lua

  status = oms2_addTLMInterface(cref, subref, name, dimensions, causality, interpolation, domain, sigrefs, nsigrefs)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_addTLMInterface(const char *cref, const char* subref, const char *name, int dimensions, oms_causality_enu_t causality, oms_tlm_interpolation_t interpolation, const char* domain, const char** sigrefs, int nsigrefs);

#END#

#DESCRIPTION#
#END#
