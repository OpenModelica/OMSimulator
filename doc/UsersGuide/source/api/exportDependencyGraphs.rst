#CAPTION#
exportDependencyGraphs
----------------------

Export the dependency graphs of a given model to dot files.
#END#

#LUA#
.. code-block:: lua

  status = oms3_exportDependencyGraphs(cref, initialization, simulation)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_exportDependencyGraphs(cref, initialization, simulation)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation);

#END#

#DESCRIPTION#
#END#
