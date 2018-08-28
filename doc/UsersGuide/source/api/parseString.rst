#CAPTION#
parseString
-----------

Parses a composite model from xml representation.
Memory is allocated for ident. The caller is responsible to call free if using the C-API.
The Lua and Python bindings takes care of the memory and the caller doesn't need to call free.
#END#

#LUA#
.. code-block:: lua

  status, ident = oms2_parseString(contents)

#END#

#PYTHON#
.. code-block:: python

  status, ident = session.parseString(contents)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_parseString(const char* contents, char** ident);

#END#

#DESCRIPTION#
#END#
