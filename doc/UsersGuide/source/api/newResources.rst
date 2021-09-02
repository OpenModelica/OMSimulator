#CAPTION#
newResources
------------

Adds a new empty resources to the SSP. The resource file is a ".ssv" file where the parameter values set by the users using
"oms_setReal()", "oms_setInteger()" and "oms_setReal()" are writtern to the file. Currently only ".ssv" files can be created.

The filename of the resource file is provided by the users using colon suffix at the end of cref. (e.g) ":root.ssv"
#END#

#LUA#
.. code-block:: lua

  status = oms_newResources(cref)

  -- Example
  oms_newModel("newResources")

  oms_addSystem("newResources.root", oms_system_wc)
  oms_addConnector("newResources.root.Input1", oms_causality_input, oms_signal_type_real)
  oms_addConnector("newResources.root.Input2", oms_causality_input, oms_signal_type_real)

  -- add Top level new resources, the filename is provided using the colon suffix ":root.ssv"
  oms_newResources("newResources.root:root.ssv")
  oms_setReal("newResources.root.Input1", 10)
  -- export the ssp with new resources
  oms_export("newResources", "newResources.ssp")


#END#

#PYTHON#
.. code-block:: python

  status = oms.newResources(cref)

  ## Example
  from OMSimulator import OMSimulator
  oms = OMSimulator()
  oms.newModel("newResources")

  oms.addSystem("newResources.root", oms_system_wc)
  oms.addConnector("newResources.root.Input1", oms.input, oms_signal_type_real)
  oms.addConnector("newResources.root.Input2", oms.input, oms_signal_type_real)

  ## add Top level resources, the filename is provided using the colon suffix ":root.ssv"
  oms.newResources("newResources.root:root.ssv")
  oms.setReal("newResources.root.Input1", 10)
  ## export the ssp with new resources
  oms.export("newResources", "newResources.ssp")

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_newResources(const char* cref)

#END#


#DESCRIPTION#

#END#
