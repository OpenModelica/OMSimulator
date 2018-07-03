#CAPTION#
setResultFile
-------------

Set the result file of the simulation.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setResultFile(cref, filename)
  status = oms2_setResultFile(cref, filename, bufferSize)

#END#

#PYTHON#
.. code-block:: python

  status = session.setResultFile(cref, filename)
  status = session.setResultFile(cref, filename, bufferSize)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setResultFile(const char* cref, const char* filename, int bufferSize);

#END#

#DESCRIPTION#
#END#
