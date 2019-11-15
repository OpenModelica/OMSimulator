#CAPTION#
setCommandLineOption
--------------------

Sets special flags.
#END#

#LUA#
.. code-block:: lua

  status = oms_setCommandLineOption(cmd)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setCommandLineOption(cmd)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setCommandLineOption(const char* cmd);

#END#

#OMC#
.. code-block:: Modelica

  status = oms_setCommandLineOption(cmd);

#END#

#DESCRIPTION#

Available flags:

.. literalinclude :: omsimulator-help.inc
   :language: bash

#END#
