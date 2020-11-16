#CAPTION#
setSignalFilter
---------------

This function specifies the signal filter. The signal filter is used to
determine which signals will eventually be exported to the result file.
#END#

#LUA#
.. code-block:: lua

  status = oms_setSignalFilter(cref, regex)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setSignalFilter(cref, regex)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setSignalFilter(const char* cref, const char* regex);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_setSignalFilter(cref, regex);

#END#

#DESCRIPTION#
The second argument, i.e. regex, is a regular expression (C++11).
`".*"` and `"(.)*"` can be used to hit all variables.
#END#
