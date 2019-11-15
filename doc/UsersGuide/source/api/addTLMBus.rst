#CAPTION#
addTLMBus
---------

Adds a TLM bus.
#END#

#LUA#
.. code-block:: lua

  status = oms_addTLMBus(cref, domain, dimensions, interpolation)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addTLMBus(cref, domain, dimensions, interpolation)

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
