#CAPTION#
addTLMBus
---------

Adds a TLM bus.
#END#

#LUA#
.. code-block:: lua

  status = oms_addTLMBus(cref, domain, dimensions, interpolation)
  
  The second argument "domain", should be any of the following, 
  
  oms_tlm_domain_input
  oms_tlm_domain_output
  oms_tlm_domain_mechanical
  oms_tlm_domain_rotational
  oms_tlm_domain_hydraulic
  oms_tlm_domain_electric
  
  The fourth argument "interpolation", should be any of the following,
  
  oms_tlm_no_interpolation
  oms_tlm_coarse_grained
  oms_tlm_fine_grained

#END#

#PYTHON#
.. code-block:: python

  status = oms.addTLMBus(cref, domain, dimensions, interpolation)
  
  The second argument "domain", should be any of the following, 
  
  oms.tlm_domain_input
  oms.tlm_domain_output
  oms.tlm_domain_mechanical
  oms.tlm_domain_rotational
  oms.tlm_domain_hydraulic
  oms.tlm_domain_electric
  
  The fourth argument "interpolation", should be any of the following,
  
  oms.default
  oms.coarsegrained
  oms.finegrained

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addTLMBus(const char* cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_addTLMBus(cref, domain, dimensions, interpolation);
  
  The second argument "domain", should be any of the following, 
  
  "OpenModelica.Scripting.oms_tlm_domain.oms_tlm_domain_input"
  "OpenModelica.Scripting.oms_tlm_domain.oms_tlm_domain_output"
  "OpenModelica.Scripting.oms_tlm_domain.oms_tlm_domain_mechanical"
  "OpenModelica.Scripting.oms_tlm_domain.oms_tlm_domain_rotational"
  "OpenModelica.Scripting.oms_tlm_domain.oms_tlm_domain_hydraulic"
  "OpenModelica.Scripting.oms_tlm_domain.oms_tlm_domain_electric"
  
  The fourth argument "interpolation", should be any of the following,
  
  "OpenModelica.Scripting.oms_tlm_interpolation.oms_tlm_no_interpolation"
  "OpenModelica.Scripting.oms_tlm_interpolation.oms_tlm_coarse_grained"
  "OpenModelica.Scripting.oms_tlm_interpolation.oms_tlm_fine_grained"

#END#

#DESCRIPTION#
#END#
