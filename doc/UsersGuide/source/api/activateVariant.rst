#CAPTION#
activateVariant
---------------

This API provides support to activate a multi-variant modelling from an ssp file [(e.g). SystemStructure.ssd, VarA.ssd, VarB.ssd ] from a ssp file.
By default when importing a ssp file the default variant will be "SystemStructure.ssd". The users can be able to switch between other variants by
using this API and make changes to that particular variant and simulate them.

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_activateVariant(const char* crefA, const char* crefB);

#END#
