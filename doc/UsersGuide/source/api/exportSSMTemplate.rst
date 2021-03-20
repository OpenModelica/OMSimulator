#CAPTION#
exportSSMTemplate
-----------------

Exports all signals that have start values of one or multiple FMUs to a SSM file that are read from modelDescription.xml with a mapping entry. The mapping entry specifies a single mapping between a parameter in the source and a parameter of the system or component being parameterized.
The mapping entry contains two attributes namely source and target. The source attribute will be empty and needs to be manually mapped by the users associated with the parameter name defined in the SSV file, the target contains the name of parameter in the system or component to be parameterized.
The function can be called for a top level model or a certain FMU component. If called for a top level model, start values of all FMUs are exported to the SSM file. If called for a component, start values of just this FMU are exported to the SSM file.
#END#

#LUA#
.. code-block:: lua

  status = oms_exportSSMTemplate(cref, filename)

#END#

#PYTHON#
.. code-block:: python

  status = oms.exportSSMTemplate(cref, filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_exportSSMTemplate(const char* cref, const char* filename)

#END#

#DESCRIPTION#
#END#
