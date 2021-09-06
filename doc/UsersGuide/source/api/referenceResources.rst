#CAPTION#
referenceResources
------------------

Switches the references of ".ssv" and ".ssm" in a SSP file. Referencing of ".ssv" and ".ssm" files are currently supported. The API can be used in two ways.

1) Referencing only the ".ssv" file.
2) Referencing both the ".ssv" along with the ".ssm" file.

This API should be used in combination with "oms_deleteResources".To switch with a new reference, the old reference must be deleted first using "oms_deleteResources" and then reference with new resources.

When deleting only the references of a ".ssv" file, if a parameter mapping file ".ssm" is binded to a ".ssv" file, then the reference of ".ssm" file will also be deleted.
It is not possible to delete the references of ".ssm" seperately as the ssm file is binded to a ssv file. Hence it is not possible to switch the reference of ".ssm" file alone.
So inorder to switch the reference of ".ssm" file, the users need to bind the reference of ".ssm" file along with the ".ssv".

The filename of the reference or resource file is provided by the users using colon suffix at the end of cref (e.g) ":root.ssv",
and the ".ssm" file is optional and is provided by the user as the second argument to the API.


#END#

#LUA#
.. code-block:: lua

  status = oms_referenceResources(cref, ssmFile)

  -- Example
  oms_importFile("referenceResources1.ssp")
  -- delete only the references in ".ssd" file
  oms_deleteResources("referenceResources1.root:root.ssv")
  -- usage-1 switch with new references, only ssv file
  oms_referenceResources("referenceResources1.root:Config1.ssv")
  -- usage-2 switch with new references, both ssv and ssm file
  oms_referenceResources("referenceResources1.root:Config1.ssv", "Config1.ssm")
  oms_export("referenceResources1.ssp")

#END#

#PYTHON#
.. code-block:: python

  status = oms.referenceResources(cref, ssmFile)

  ## Example
  from OMSimulator import OMSimulator
  oms = OMSimulator()
  oms.importFile("referenceResources1.ssp")
  ## delete only the references in ".ssd" file
  oms.deleteResources("referenceResources1.root:root.ssv")
  ## usage-1 switch with new references, only ssv file
  oms.referenceResources("referenceResources1.root:Config1.ssv")
  ## usage-2 switch with new references, both ssv and ssm file
  oms.referenceResources("referenceResources1.root:Config1.ssv", "Config1.ssm")


#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_referenceResources(const char* cref, const char* ssmFile);

#END#


#DESCRIPTION#
#END#
