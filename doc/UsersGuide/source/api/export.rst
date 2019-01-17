#CAPTION#
export
------

Exports a composite model to a SPP file.
#END#

#LUA#
.. code-block:: lua

  status = oms_export(cref, filename)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms_export(cref, filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_export(const char* cref, const char* filename);

#END#

#DESCRIPTION#
#END#
