#CAPTION#
listModel
---------

Lists the contents of a composite model.
Memory is allocated for contents. The caller is responsible to call free if using the C-API.
The Lua and Python bindings takes care of the memory and the caller doesn't need to call free.
#END#

#LUA#
.. code-block:: lua

  status, contents = oms2_listModel(ident)

#END#

#PYTHON#
.. code-block:: python

  status, contents = session.listModel(ident)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_listModel(const char* ident, char** contents);

#END#

#DESCRIPTION#
#END#
