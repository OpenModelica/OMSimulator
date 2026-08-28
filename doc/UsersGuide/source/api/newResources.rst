#CAPTION#
newResources
------------

Adds a new empty resources to the SSP. The resource file is a ".ssv" file where the parameter values set by the users using
"oms_setReal()", "oms_setInteger()" and "oms_setReal()" are writtern to the file. Currently only ".ssv" files can be created.

The filename of the resource file is provided by the users using colon suffix at the end of cref. (e.g) ":root.ssv"
#END#


#CAPI#
.. code-block:: c

  oms_status_enu_t oms_newResources(const char* cref)

#END#


#DESCRIPTION#

#END#
