#CAPTION#
replaceSubModel
---------------

Replaces an existing fmu component, with a new component provided by the user,
When replacing the fmu checks are made in all ssp concepts like in ssd, ssv and ssm, so that connections and parameter settings
are not lost. It is possible that the namings of inputs and parameters match, but the start values might have been changed,
in such cases new start values will be applied in ssd, ssv and ssm. In case if the Types of inputs and outputs and parameters
differed, then the variables are updated according to the new changes and the connections will be removed with warning messages to user.
In case when replacing a fmu, if the fmu contains parameter mapping associated with the ssv file, then only the ssm file entries
are updated and the start values in the ssv files will not be changed.

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_replaceSubModel(const char* cref, const char* fmuPath);

#END#
