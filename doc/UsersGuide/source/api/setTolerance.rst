#CAPTION#
setTolerance
------------

Sets the tolerance for a given system or component.
#END#

#LUA#
.. code-block:: lua

  status = oms_setTolerance(const char* cref, double tolerance)
  status = oms_setTolerance(const char* cref, double absoluteTolerance, double relativeTolerance)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setTolerance(const char* cref, double tolerance)
  status = oms.setTolerance(const char* cref, double absoluteTolerance, double relativeTolerance)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setTolerance(const char* cref, double absoluteTolerance, double relativeTolerance);

#END#

#DESCRIPTION#
#END#
