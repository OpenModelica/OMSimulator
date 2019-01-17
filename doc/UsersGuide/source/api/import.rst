#CAPTION#
import
------

Imports a composite model from a SSP file.
#END#

#LUA#
.. code-block:: lua

  cref, status = oms_import(filename)

#END#

#PYTHON#
.. code-block:: python

  cref, status = session.oms_import(filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_import(const char* filename, char** cref);

#END#

#DESCRIPTION#
#END#
