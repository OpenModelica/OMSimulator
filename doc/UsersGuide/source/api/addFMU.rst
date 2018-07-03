#CAPTION#
addFMU
------

Adds a new FMU instance to a given FMI model.
#END#

#LUA#
.. code-block:: lua

  status = oms2_addFMU(modelIdent, fmuPath, fmuIdent)

#END#

#PYTHON#
.. code-block:: python

  status = session.addFMU(modelIdent, fmuPath, fmuIdent)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_addFMU(const char* modelIdent, const char* fmuPath, const char* fmuIdent);

#END#

#DESCRIPTION#
#END#
