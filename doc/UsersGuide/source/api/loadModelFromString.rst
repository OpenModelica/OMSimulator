#CAPTION#
loadModelFromString
-------------------

Loads a FMI composite model from xml representation. If a model already exists with the same identifier, it will be updated.
#END#

#LUA#
.. code-block:: lua

  status, ident = oms2_loadModelFromString(contents)

#END#

#PYTHON#
.. code-block:: python

  status, ident = session.loadModelFromString(filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_loadModelFromString(const char* contents, char** ident);

#END#

#DESCRIPTION#
#END#
