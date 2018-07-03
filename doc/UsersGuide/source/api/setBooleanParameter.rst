#CAPTION#
setBooleanParameter
-------------------

Set boolean value of given parameter.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setBooleanParameter(signal, value);

#END#

#PYTHON#
.. code-block:: python

  status = session.setBooleanParameter(signal, value);

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setBooleanParameter(const char* signal, bool value);

#END#

#DESCRIPTION#
#END#
