#CAPTION#
addSignalsToResults
-------------------

Add all variables that match the given regex to the result file.
#END#

#LUA#
.. code-block:: lua

  status = oms2_addSignalsToResults(cref, regex)

#END#

#PYTHON#
.. code-block:: python

  status = session.addSignalsToResults(cref, regex)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_addSignalsToResults(const char* cref, const char* regex);

#END#

#DESCRIPTION#
The second argument, i.e. regex, is considered as a regular expression (C++11). `".*"` and `"(.)*"` can be used to hit all variables.
#END#
