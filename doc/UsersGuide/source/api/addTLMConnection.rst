#CAPTION#
addTLMConnection
----------------

Adds an external model to a TLM composite model.
#END#

#LUA#
.. code-block:: lua

  status = oms2_addTLMConnection(cref, from, to, delay, alpha, Zf, Zfr)

#END#

#PYTHON#
.. code-block:: python

  status = session.addTLMConnection(cref, from, to, delay, alpha, Zf, Zfr)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_addTLMConnection(const char* cref, const char* from, const char* to, double delay, double alpha, double Zf, double Zfr);


#END#

#DESCRIPTION#
#END#
