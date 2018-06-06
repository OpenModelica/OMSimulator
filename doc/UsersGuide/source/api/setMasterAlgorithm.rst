#CAPTION#
setMasterAlgorithm
------------------

Set master algorithm variant that shall be used (default: "standard").

Supported master algorithms: "standard"

Experimental master algorithms (no stable API!): "pctpl", "pmrchannela", "pmrchannelcv", "pmrchannelm"
#END#

#LUA#
.. code-block:: lua

  status = oms2_setMasterAlgorithm(ident, masterAlgorithm)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setMasterAlgorithm(const char* ident, const char* masterAlgorithm);

#END#

#DESCRIPTION#
#END#
