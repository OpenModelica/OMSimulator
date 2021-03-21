#CAPTION#
terminate
---------

Terminates a given composite model.
#END#

#LUA#
.. code-block:: lua

  status = oms_terminate(cref)

#END#

#PYTHON#
.. code-block:: python

  status = oms.terminate(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_terminate(const char* cref);

#END#

#OMC#
.. code-block:: modelica

  status := oms_terminate(cref);

#END#

#DESCRIPTION#
#END#
