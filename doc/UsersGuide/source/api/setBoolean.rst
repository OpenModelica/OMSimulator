#CAPTION#
setBoolean
----------

Sets the value of a given boolean signal.
#END#

#LUA#
.. code-block:: lua

  status = oms_setBoolean(cref, value)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setBoolean(cref, value)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setBoolean(const char* cref, bool value);

#END#

#OMC#
.. code-block:: Modelica

  status = oms_setBoolean(cref, value);

#END#

#DESCRIPTION#
#END#
