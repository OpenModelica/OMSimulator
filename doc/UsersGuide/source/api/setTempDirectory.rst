#CAPTION#
setTempDirectory
----------------

Set new temp directory.
#END#

#LUA#
.. code-block:: lua

  status = oms_setTempDirectory(newTempDir)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setTempDirectory(newTempDir)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setTempDirectory(const char* newTempDir);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_setTempDirectory(newTempDir);

#END#

#DESCRIPTION#
#END#
