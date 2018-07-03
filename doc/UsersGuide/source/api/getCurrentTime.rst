#CAPTION#
getCurrentTime
--------------

Get current simulation time.
#END#

#LUA#
.. code-block:: lua

  time, status = oms2_getCurrentTime(model)

#END#

#PYTHON#
.. code-block:: python

  time, status = session.getCurrentTime(model)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_getCurrentTime(const char* model, double* time);

#END#

#DESCRIPTION#
#END#
