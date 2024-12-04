#CAPTION#
getTolerance
------------

Gets the tolerance of a given system or component.
#END#

#LUA#
.. code-block:: lua

  relativeTolerance, status = oms_getTolerance(cref)

#END#

#PYTHON#
.. code-block:: python

  relativeTolerance, status = oms.getTolerance(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getTolerance(const char* cref, double* relativeTolerance);

#END#

#OMC#
.. code-block:: modelica

  (relativeTolerance, status) := oms_getTolerance(cref);

#END#

#DESCRIPTION#
#END#
