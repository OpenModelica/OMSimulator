#CAPTION#
getSubModelPath
---------------

Returns the path of a given component.
#END#

#PYTHON#
.. code-block:: python

  path, status = oms.getSubModelPath(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getSubModelPath(const char* cref, char** path);

#END#

#OMC#
.. code-block:: Modelica

  (path, status) := oms_getSubModelPath(cref);

#END#

#DESCRIPTION#
#END#
