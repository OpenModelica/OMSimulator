#CAPTION#
saveModel
---------

Loads a FMI composite model from xml representation.
#END#

#LUA#
.. code-block:: lua

  status = oms2_saveModel(filename, ident)

#END#

#PYTHON#
.. code-block:: python

  status = session.saveModel(filename, ident)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_saveModel(const char* filename, const char* ident);

#END#

#DESCRIPTION#
#END#
