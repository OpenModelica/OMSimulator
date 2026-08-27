#CAPTION#
exportSnapshot
--------------

Lists the SSD representation of a given model, system, or component.

Memory is allocated for `contents`. The caller is responsible to free it using
the C-API. The Lua and Python bindings take care of the memory and the caller
doesn't need to call free.
#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_exportSnapshot(const char* cref, char** contents);

#END#

#OMC#
.. code-block:: modelica

  (contents, status) := oms_exportSnapshot(cref);

#END#

#DESCRIPTION#
#END#
