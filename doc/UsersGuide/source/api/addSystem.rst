#CAPTION#
addSystem
---------

Adds a (sub-)system to a model or system.
#END#

#LUA#
.. code-block:: lua

  status = oms_addSystem(cref, type)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addSystem(cref, type)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addSystem(const char* cref, oms_system_enu_t type);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_addSystem(cref, type);
  
  The second argument type, should be any of the following,

  "OpenModelica.Scripting.oms_system.oms_system_none"
  "OpenModelica.Scripting.oms_system.oms_system_tlm" 
  "OpenModelica.Scripting.oms_system.oms_system_sc"
  "OpenModelica.Scripting.oms_system.oms_system_wc"

#END#

#DESCRIPTION#
#END#
