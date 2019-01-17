#CAPTION#
setMaxLogFileSize
-----------------

Sets maximum log file size in MB. If the file exceeds this limit, the logging
will continue on stdout.
#END#

#LUA#
.. code-block:: lua

  oms_setMaxLogFileSize(size)

#END#

#PYTHON#
.. code-block:: python

  session.oms_setMaxLogFileSize(size)

#END#

#CAPI#
.. code-block:: c

  void oms_setMaxLogFileSize(const unsigned long size);

#END#

#DESCRIPTION#
#END#
