#CAPTION#
loadModel
---------

Loads a FMI composite model from xml file. If a model already exists with the same identifier, it will be updated.
#END#

#LUA#
.. code-block:: lua

  status, ident = oms2_loadModel(filename)

#END#

#PYTHON#
.. code-block:: python

  status, ident = session.loadModel(filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_loadModel(const char* filename, char** ident);

#END#

#DESCRIPTION#
#END#
