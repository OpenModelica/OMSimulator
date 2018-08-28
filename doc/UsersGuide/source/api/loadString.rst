#CAPTION#
loadString
----------

Loads a composite model from xml representation. If a model already exists with the same identifier, it will be updated.
#END#

#LUA#
.. code-block:: lua

  status, ident = oms2_loadString(contents)

#END#

#PYTHON#
.. code-block:: python

  status, ident = session.loadString(contents)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_loadString(const char* contents, char** ident);

#END#

#DESCRIPTION#
#END#
