#CAPTION#
listVariants
------------

This API shows the number of variants available [(e.g). SystemStructure.ssd, VarA.ssd, VarB.ssd ] from a ssp file.

#END#

#LUA#
.. code-block:: lua

  status = oms_listVariants(cref)

#END#

#PYTHON#
.. code-block:: python

  status = oms.listVariants(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_listVariants(const char* cref);

#END#


#DESCRIPTION#
  An example for finding the number of available variants in a ssp file

  oms_newModel("model")
  oms_addSystem("model.root", "system_wc")
  oms_addSubModel("model.root.A", "A.fmu")
  oms_duplicateVariant("model", "varA")
  oms_duplicateVariant("varA", "varB")

  oms_listVariants("varB")

  The API will list the available variants like below
  <oms:Variants>
    <oms:variant name="model" />
    <oms:variant name="varB" />
    <oms:variant name="varA" />
  </oms:Variants>

#END#
