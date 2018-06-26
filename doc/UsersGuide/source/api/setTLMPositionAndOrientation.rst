#CAPTION#
setTLMPositionAndOrientation
----------------------------

Sets initial position and orientation for a TLM 3D interface.
#END#

#LUA#
.. code-block:: lua

  status oms2_setTLMPositionAndOrientation(cref, ifc, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setTLMPositionAndOrientation(const char *cref, const char *ifc, double x1, double x2, double x3, double A11, double A12, double A13, double A21, double A22, double A23, double A31, double A32, double A33);

#END#

#DESCRIPTION#
#END#
