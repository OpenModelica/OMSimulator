#CAPTION#
setTLMLoggingLevel
------------------

Specifies the debug logging level for OMTLMSimulator

0 disabled, 1 fatal, 2 fatal+warning, 3 fatal+warning+info, 4 fatal+warning+info+debug
#END#

#LUA#
.. code-block:: lua

  oms2_setTLMLoggingLevel(cref, level)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setTLMLoggingLevel(const char* cref, const int level);

#END#

#DESCRIPTION#
#END#
