#CAPTION#
duplicateVariant
----------------

This API provides support to develop a multi-variant modelling in OMSimulator [(e.g). SystemStructure.ssd, VarA.ssd, VarB.ssd ].
When duplicating a variant, the new variant becomes the current variant and all the changes made by the users are applied
to the new variants only, and all the ssv and ssm resources associated with the new variant will be given new name based on the variant name provided by the user.
This allows the bundling of multiple variants of a system structure definition referencing a similar set of packaged resources as a single SSP.
However there must still be one SSD file named SystemStructure.ssd at the root of the ZIP archive which will be considered
as default variant.

#END#

#LUA#
.. code-block:: lua

  status = oms_duplicateVariant(crefA, crefB)

#END#

#PYTHON#
.. code-block:: python

  status = oms.duplicateVariant(crefA, crefB)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_duplicateVariant(const char* crefA, const char* crefB);

#END#

#DESCRIPTION#
An example of creating a multi-variant modelling is presented below

.. code-block:: lua

  oms_newModel("model")
  oms_addSystem("model.root", "system_wc")
  oms_addSubModel("model.root.A", "A.fmu")
  oms_setReal("model.root.A.param1", "10")
  oms_duplicateVariant("model", "varB")
  oms_addSubModel("varB.root.B" ,"B.fmu")
  oms_setReal("varB.root.A.param2", "20")
  oms_export("varB", "variant.ssp")

The variant.ssp file will have the following structure

.. code-block::

  Variant.ssp
      SystemStructure.ssd
      varB.ssd
      resources\
          A.fmu
          B.fmu

#END#
