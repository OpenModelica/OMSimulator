#CAPTION#
doStep
------

Simulates a macro step of the given composite model. The step size
will be determined by the master algorithm and is limited by the
definied minimal and maximal step sizes.
#END#

#PYTHON#
.. code-block:: python

  status = oms.doStep(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_doStep(const char* cref);

#END#

#DESCRIPTION#
#END#
