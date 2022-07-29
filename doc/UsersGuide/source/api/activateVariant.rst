#CAPTION#
activateVariant
---------------

This API provides support to activate a multi-variant modelling from an ssp file [(e.g). SystemStructure.ssd, VarA.ssd, VarB.ssd ] from a ssp file.
By default when importing a ssp file the default variant will be "SystemStructure.ssd". The users can be able to switch between other variants by
using this API and make changes to that particular variant and simulate them.

#END#

#LUA#
.. code-block:: lua

  status = oms_activateVariant(crefA, crefB)

#END#

#PYTHON#
.. code-block:: python

  status = oms.activateVariant(crefA, crefB)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_activateVariant(const char* crefA, const char* crefB);

#END#


#DESCRIPTION#
  An example of activating the number of available variants in a ssp file

  oms_newModel("model")
  oms_addSystem("model.root", "system_wc")
  oms_addSubModel("model.root.A", "A.fmu")
  oms_duplicateVariant("model", "varA") // varA will be the current variant
  oms_duplicateVariant("varA", "varB") // varB will be the current variant
  oms_activateVariant("varB", "varA") // Reactivate the variant varB to varA
  oms_activateVariant("varA", "model") // Reactivate the variant varA to model

#END#
