#CAPTION#
getString
---------

Get string value.


Memory is allocated for `value`. The caller is responsible to free it
using the C-API. The Lua and Python bindings take care of the memory
and the caller doesn't need to call free.
#END#

#LUA#
.. code-block:: lua

  value, status = oms_getString(cref)

#END#

#PYTHON#
.. code-block:: python

  value, status = oms.getString(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getString(const char* cref, char** value);

#END#


#DESCRIPTION#
#END#
