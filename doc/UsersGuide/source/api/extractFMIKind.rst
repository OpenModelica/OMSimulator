#CAPTION#
extractFMIKind
--------------

Extracts the FMI kind of a given FMU from the file system.
#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_extractFMIKind(const char* filename, oms_fmi_kind_enu_t* kind);

#END#

#OMC#
.. code-block:: Modelica

  (kind,status) := oms_extractFMIKind(filename);

#END#

#DESCRIPTION#
#END#
