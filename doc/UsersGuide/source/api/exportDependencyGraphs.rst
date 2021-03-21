#CAPTION#
exportDependencyGraphs
----------------------

Export the dependency graphs of a given model to dot files.
#END#

#LUA#
.. code-block:: lua

  status = oms_exportDependencyGraphs(cref, initialization, event, simulation)

#END#

#PYTHON#
.. code-block:: python

  status = oms.exportDependencyGraphs(cref, initialization, event, simulation)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_exportDependencyGraphs(const char* cref, const char* initialization, const char* event, const char* simulation);

#END#

#OMC#
.. code-block:: modelica

  status := oms_exportDependencyGraphs(cref, initialization, event, simulation);

#END#

#DESCRIPTION#
#END#
