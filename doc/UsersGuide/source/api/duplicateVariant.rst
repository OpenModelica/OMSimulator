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

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_duplicateVariant(const char* crefA, const char* crefB);

#END#
