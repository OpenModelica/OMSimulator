#CAPTION#
setResultFile
-------------

Set the result file of the simulation.
#END#

#LUA#
.. code-block:: lua

  status = oms3_setResultFile(cref, filename)
  status = oms3_setResultFile(cref, filename, bufferSize)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_setResultFile(cref, filename)
  status = session.oms3_setResultFile(cref, filename, bufferSize)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_setResultFile(const char* cref, const char* filename, int bufferSize);

#END#

#DESCRIPTION#
#END#
