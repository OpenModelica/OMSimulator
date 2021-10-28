#CAPTION#
addResources
------------

Adds an external resources to an existing SSP. The external resources should be a ".ssv" or ".ssm" file
#END#

#LUA#
.. code-block:: lua

  status = oms_addResources(cref, path)

  -- Example
  oms_importFile("addExternalResources1.ssp")
  -- add list of external resources from filesystem to ssp
  oms_addResources("addExternalResources", "../../resources/externalRoot.ssv")
  oms_addResources("addExternalResources:externalSystem.ssv", "../../resources/externalSystem1.ssv")
  oms_addResources("addExternalResources", "../../resources/externalGain.ssv")
  -- export the ssp with new resources
  oms_export("addExternalResources", "addExternalResources1.ssp")


#END#

#PYTHON#
.. code-block:: python

  status = oms.addResources(cref, path)

  ## Example
  from OMSimulator import OMSimulator
  oms = OMSimulator()
  oms.importFile("addExternalResources1.ssp")
  ## add list of external resources from filesystem to ssp
  oms.addResources("addExternalResources", "../../resources/externalRoot.ssv")
  oms.addResources("addExternalResources:externalSystem.ssv", "../../resources/externalSystem1.ssv")
  oms.addResources("addExternalResources", "../../resources/externalGain.ssv")
  ## export the ssp with new resources
  oms_export("addExternalResources", "addExternalResources1.ssp")

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addResources(const char* cref_, const char* path)

#END#


#DESCRIPTION#
#END#
