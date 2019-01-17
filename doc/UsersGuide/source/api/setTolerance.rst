#CAPTION#
setTolerance
------------

Sets the tolerance for a given system or component.
#END#

#LUA#
.. code-block:: lua

  status = oms_setTolerance(cref, tolerance)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms_setTolerance(cref, tolerance)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setTolerance(const char* cref, double tolerance);

#END#

#DESCRIPTION#
#END#
