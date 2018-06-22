#CAPTION#
exportCompositeStructure
------------------------

Export the composite structure of a given model to a dot file.
#END#

#LUA#
.. code-block:: lua

  status = oms2_exportCompositeStructure(cref, filename)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_exportCompositeStructure(const char* cref, const char* filename);

#END#

#DESCRIPTION#
The dot file can be translated to an png file using this command:

`dot -Gsplines=none <model.dot> | neato -n -Gsplines=ortho -Tpng -o<model.png>`
#END#
