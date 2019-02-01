#CAPTION#
importString
------------

Imports a composite model from a string.
#END#

#LUA#
.. code-block:: lua

  cref, status = oms_importString(contents)

#END#

#PYTHON#
.. code-block:: python

  cref, status = oms.importString(contents)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_importString(const char* contents, char** cref);

#END#

#DESCRIPTION#
#END#
