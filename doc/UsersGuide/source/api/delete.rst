#CAPTION#
delete
------

Deletes a connector, component, system, or model object.
#END#

#LUA#
.. code-block:: lua

  status = oms_delete(cref)

#END#

#PYTHON#
.. code-block:: python

  status = oms.delete(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_delete(const char* cref);

#END#


#OMC#
.. code-block:: Modelica

  status := oms_delete(cref);

#END#

#DESCRIPTION#
#END#
