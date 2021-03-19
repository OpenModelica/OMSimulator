#CAPTION#
setTLMPositionAndOrientation
----------------------------

Sets initial position and orientation for a TLM 3D interface.
#END#

#LUA#
.. code-block:: lua

  status = oms_setTLMPositionAndOrientation(cref, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setTLMPositionAndOrientation(cref, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_setTLMPositionAndOrientation(cref, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33);

#END#

#DESCRIPTION#
#END#
