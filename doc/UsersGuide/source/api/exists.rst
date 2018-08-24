#CAPTION#
exists
------

This function returns 1 if a given cref exists in the scope, otherwise 0. It can be used to check for composite models, sub-models such as FMUs, and solver instances.
#END#

#LUA#
.. code-block:: lua

  oms2_exists(cref)

The following table describes the input values:

#END#

#PYTHON#
.. code-block:: python

  session.exists(cref)

#END#

#CAPI#
.. code-block:: c

  int oms2_exists(const char* cref);

#END#

#DESCRIPTION#
#END#
