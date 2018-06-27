#CAPTION#
removeSignalsFromResults
------------------------

Removes all variables that match the given regex to the result file.
#END#

#LUA#
.. code-block:: lua

  status = oms2_removeSignalsFromResults(cref, regex)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_removeSignalsFromResults(const char* cref, const char* regex);

#END#

#DESCRIPTION#
The second argument, i.e. regex, is considered as a regular expression (C++11). `".*"` and `"(.)*"` can be used to hit all variables.
#END#
