#CAPTION#
getTime
-------

Get the current simulation time from the model.
#END#

#LUA#
.. code-block:: lua

  time, status = oms_getTime(cref)

#END#

#PYTHON#
.. code-block:: python

  time, status = oms.getTime(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getTime(const char* cref, double* time);

#END#

#OMC#
.. code-block:: modelica

  (time, status) := oms_getTime(cref);

#END#

#DESCRIPTION#
#END#
