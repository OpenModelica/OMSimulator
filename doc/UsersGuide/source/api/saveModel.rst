#CAPTION#
saveModel
---------

Dumps SSD representation of a given FMI composite model to a file.
#END#

#LUA#
.. code-block:: lua

  status = oms2_saveModel(ident, filename)

#END#

#PYTHON#
.. code-block:: python

  status = session.saveModel(ident, filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_saveModel(const char* ident, const char* filename);

#END#

#DESCRIPTION#
#END#
