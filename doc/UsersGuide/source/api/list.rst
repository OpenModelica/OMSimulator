#CAPTION#
list
----

Lists the SSD representation of a given component.

Memory is allocated for contents. The caller is responsible to call free if
using the C-API. The Lua and Python bindings takes care of the memory and the
caller doesn't need to call free.
#END#

#LUA#
.. code-block:: lua

  contents, status = oms3_list(cref)

#END#

#PYTHON#
.. code-block:: python

  contents, status = session.oms3_list(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_list(const char* cref, char** contents);

#END#

#DESCRIPTION#
#END#
