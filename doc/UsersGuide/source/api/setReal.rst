#CAPTION#
setReal
-------

Sets the value of a given real signal.
#END#

#LUA#
.. code-block:: lua

  status = oms_setReal(cref, value)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setReal(cref, value)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setReal(const char* cref, double value);

#END#

#OMC#
.. code-block:: Modelica

  status = oms_setReal(cref, value);

#END#

#DESCRIPTION#
This function can be called in different model states:

- Before instantiation: `setReal` can be used to set start values or to define
  initial unknowns (e.g. parameters, states). The values are not immediately
  applied to the simulation unit, since it isn’t actually instantiated.
- After instantiation and before initialization: Same as before instantiation,
  but the values are applied immediately to the simulation unit.
- After initialization: Can be used to force external inputs, which might cause
  discrete changes of continuous signals.
#END#
