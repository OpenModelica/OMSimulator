#CAPTION#
exportSSVTemplate
-----------------

Exports all signals that have start values of one or multiple FMUs to a SSV file that are read from modelDescription.xml. The function can be called for a top level model or a certain FMU component.
If called for a top level model, start values of all FMUs are exported to the SSV file. If called for a component, start values of just this FMU are exported to the SSV file.
#END#


#CAPI#
.. code-block:: c

  oms_status_enu_t oms_exportSSVTemplate(const char* cref, const char* filename)

#END#

#DESCRIPTION#
#END#
