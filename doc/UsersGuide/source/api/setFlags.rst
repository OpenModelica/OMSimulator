#CAPTION#
setFlags
--------

Sets special flags.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setFlags(cref, flags)

#END#

#PYTHON#
.. code-block:: python

  status = session.setFlags(cref, flags)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setFlags(const char* cref, const char* flags);

#END#

#DESCRIPTION#

Available flags:

.. csv-table::
  :header: "Name", "Description"
  :widths: 10, 65

  "+fetchAllVars", "fetches all variables of the given FMU before performaing the local integration (doStep)."
  "-fetchAllVars", "Disabling `+fetchAllVars` again."

#END#
