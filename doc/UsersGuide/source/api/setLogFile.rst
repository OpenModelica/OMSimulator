#CAPTION#
setLogFile
----------

Redirects logging output to file or std streams. The warning/error counters are reset.

filename="" to redirect to std streams and proper filename to redirect to file.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setLogFile(filename)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setLogFile(const char* filename);

#END#

#DESCRIPTION#
#END#
