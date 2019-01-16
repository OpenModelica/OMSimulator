#CAPTION#
setCommandLineOption
--------------------

Sets special flags.
#END#

#LUA#
.. code-block:: lua

  status = oms3_setCommandLineOption(cmd)

#END#

#PYTHON#
.. code-block:: python

  status = oms3_setCommandLineOption(cmd)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_setCommandLineOption(const char* cmd);

#END#

#DESCRIPTION#

Available flags:

.. literalinclude :: omsimulator-help.inc
   :language: bash

#END#
