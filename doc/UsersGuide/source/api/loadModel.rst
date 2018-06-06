#CAPTION#
loadModel
---------

Loads a FMI composite model from xml representation.
#END#

#LUA#
.. code-block:: lua

  ident, status = oms2_loadModel(filename)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_loadModel(const char* filename, char** ident);

#END#

#DESCRIPTION#
#END#
