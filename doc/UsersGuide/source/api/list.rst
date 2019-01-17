#CAPTION#
list
----

Lists the SSD representation of a given model, system, or component.

Memory is allocated for it. The caller is responsible to free it using the
C-API. The Lua and Python bindings take care of the memory and the caller
doesn't need to call free.
#END#

#LUA#
.. code-block:: lua

  contents, status = oms_list(cref)

#END#

#PYTHON#
.. code-block:: python

  contents, status = oms.list(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_list(const char* cref, char** contents);

#END#

#DESCRIPTION#
#END#
