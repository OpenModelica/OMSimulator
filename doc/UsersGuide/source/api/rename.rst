#CAPTION#
rename
------

Renames a composite model (either FMI or TLM) or sub-model (e.g. FMU instance).
#END#

#LUA#
.. code-block:: lua

  status = oms2_rename(identOld, identNew)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_rename(const char* identOld, const char* identNew);

#END#

#DESCRIPTION#
#END#
