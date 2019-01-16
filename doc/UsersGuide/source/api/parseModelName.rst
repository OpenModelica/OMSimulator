#CAPTION#
parseModelName
--------------

Parses the model name from a given SSD representation.

Memory is allocated for ident. The caller is responsible to call free if using
the C-API. The Lua and Python bindings takes care of the memory and the caller
doesn't need to call free.
#END#

#LUA#
.. code-block:: lua

  ident, status = oms3_parseModelName(contents)

#END#

#PYTHON#
.. code-block:: python

  ident, status = session.oms3_parseModelName(contents)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_parseModelName(const char* contents, char** cref);

#END#

#DESCRIPTION#
#END#
