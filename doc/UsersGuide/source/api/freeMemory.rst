#CAPTION#
freeMemory
----------

Free the memory allocated by some other API. Pass the object for which memory
is allocated. You can directly call free on the object or can use this
function.
#END#

#LUA#
.. code-block:: lua

  oms_freeMemory(obj)

#END#

#PYTHON#
.. code-block:: python

  oms.freeMemory(obj)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_freeMemory(void* obj);

#END#

#DESCRIPTION#
#END#
