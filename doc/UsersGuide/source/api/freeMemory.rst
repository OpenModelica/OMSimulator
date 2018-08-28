#CAPTION#
freeMemory
----------

Free the memory allocated by some other API. Pass the object for which memory is allocated.
You can directly call free on the object or can use this function.
#END#

#LUA#
.. code-block:: lua

  oms2_freeMemory(obj)

#END#

#PYTHON#
.. code-block:: python

  session.freeMemory(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_freeMemory(void* obj);

#END#

#DESCRIPTION#
#END#
