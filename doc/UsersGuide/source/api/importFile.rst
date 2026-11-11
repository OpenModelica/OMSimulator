#CAPTION#
importFile
----------

Imports a composite model from a SSP file.
#END#


#CAPI#
.. code-block:: c

  oms_status_enu_t oms_importFile(const char* filename, char** cref);

#END#

#OMC#
.. code-block:: modelica

  (cref, status) := oms_importFile(filename);

#END#

#DESCRIPTION#
.. warning::

  This function is deprecated and will be removed in a future version.
  Use the Python API instead:

  .. code-block:: python

    import OMSimulator
    model = OMSimulator.SSP("xxx.ssp")

#END#
