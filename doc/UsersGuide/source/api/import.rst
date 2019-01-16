#CAPTION#
import
------

Imports a composite model from a SSP file.
#END#

#LUA#
.. code-block:: lua

  cref, status = oms3_import(filename)

#END#

#PYTHON#
.. code-block:: python

  cref, status = session.oms3_import(filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_import(const char* filename, char** cref);

#END#

#DESCRIPTION#
#END#
