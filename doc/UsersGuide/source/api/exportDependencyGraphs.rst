#CAPTION#
exportDependencyGraphs
----------------------

Export the dependency graphs of a given model to a dot file.
#END#

#LUA#
.. code-block:: lua

  status = oms2_exportDependencyGraphs(cref, initialization, simulation)

#END#

#PYTHON#
.. code-block:: python

  status = session.exportDependencyGraphs(cref, initialization, simulation)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation);

#END#

#DESCRIPTION#
#END#
