#CAPTION#
setLogFile
----------

Redirects logging output to file or std streams. The warning/error counters are
reset.

filename="" to redirect to std streams and proper filename to redirect to file.
#END#

#LUA#
.. code-block:: lua

  status = oms_setLogFile(filename)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setLogFile(filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setLogFile(const char* filename);

#END#

#OMC#
.. code-block:: modelica

  status := oms_setLogFile(filename);

#END#

#DESCRIPTION#
#END#
