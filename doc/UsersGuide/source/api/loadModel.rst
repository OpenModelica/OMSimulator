#CAPTION#
loadModel
---------

Loads a FMI composite model from xml file.
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
