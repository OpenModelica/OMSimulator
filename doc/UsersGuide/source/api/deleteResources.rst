#CAPTION#
deleteResources
---------------

Deletes the reference and resource file in a SSP. Deletion of ".ssv" and ".ssm" files are currently supported. The API can be used in two ways.

1) deleting only the reference file in ".ssd".
2) deleting both reference and resource files in ".ssp".

To delete only the reference file in ssd, the user should provide the full qualified cref of the ".ssv" file associated with a system or subsystem or component (e.g) "model.root:root1.ssv".

To delete both the reference and resource file in ssp, it is enough to provide only the model cref of the ".ssv" file  (e.g) "model:root1.ssv".

When deleting only the references of a ".ssv" file, if a parameter mapping file ".ssm" is binded to a ".ssv" file then the ".ssm" file will also be deleted.
It is not possible to delete the references of ".ssm" seperately as the ssm file is binded to a ssv file.

The filename of the reference or resource file is provided by the users using colon suffix at the end of cref. (e.g) ":root.ssv"

#END#

#LUA#
.. code-block:: lua

  status = oms_deleteResources(cref)

  -- Example
  oms_importFile("deleteResources1.ssp")
  -- delete only the references in ".ssd" file
  oms_deleteResources("deleteResources.root:root.ssv")
  -- delete both references and resources
  oms_deleteResources("deleteResources:root.ssv")
  oms_export("deleteResources1.ssp")

#END#

#PYTHON#
.. code-block:: python

  status = oms.deleteResources(cref))

  ## Example
  from OMSimulator import OMSimulator
  oms = OMSimulator()
  oms.importFile("deleteResources1.ssp")
  ## delete only the references in ".ssd" file
  oms.deleteResources("deleteResources.root:root.ssv")
  ## delete both references and resources
  oms.deleteResources("deleteResources:root.ssv")
  oms.export("deleteResources1.ssp")


#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_deleteResources(const char* cref);

#END#


#DESCRIPTION#
#END#
