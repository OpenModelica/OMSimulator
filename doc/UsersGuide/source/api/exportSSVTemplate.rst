#CAPTION#
exportSSVTemplate
-----------------

Export start values of a FMU read from modeldescription.xml to a .ssv file. It can be called on a given model or components.
#END#

#LUA#
.. code-block:: lua

  status = oms_exportSSVTemplate(cref, filename)

#END#

#PYTHON#
.. code-block:: python

  status = oms.exportSSVTemplate(cref, filename)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_exportSSVTemplate(const char* cref, const char* filename)

#END#

#OMC#
.. code-block:: Modelica

  # not available

#END#

#DESCRIPTION#
#END#
