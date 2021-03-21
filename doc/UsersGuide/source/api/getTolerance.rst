#CAPTION#
getTolerance
------------

Gets the tolerance of a given system or component.
#END#

#LUA#
.. code-block:: lua

  absoluteTolerance, relativeTolerance, status = oms_getTolerance(cref)

#END#

#PYTHON#
.. code-block:: python

  absoluteTolerance, relativeTolerance, status = oms.getTolerance(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getTolerance(const char* cref, double* absoluteTolerance, double* relativeTolerance);

#END#

#OMC#
.. code-block:: modelica

  (absoluteTolerance, relativeTolerance, status) := oms_getTolerance(cref);

#END#

#DESCRIPTION#
#END#
