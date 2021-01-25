.. index:: OMEdit

Graphical Modelling
===================

OMSimulator has an optional dependency to OpenModelica in order to utilize the
graphical modelling editor OMEdit. This feature requires to install the full
OpenModelica tool suite, which includes OMSimulator. The independent
stand-alone version doesn't provide any graphical modelling editor.

See also `FMI documentation <https://openmodelica.org/doc/OpenModelicaUsersGuide/latest/fmitlm.html>`_.

.. figure :: images/omedit_01.png
  :name: omedit-mainwindow-browsers

  OMEdit MainWindow and Browsers.

New OMSimulator Model
---------------------

A new and empty OMSimulator model can be created from the OMSimulator menu
item.

.. figure :: images/omedit_02.png

  OMEdit: New OMSimulator Model

That will pop-up a dialog to enter the names of the model and the root system.

.. figure :: images/omedit_03.png

  OMEdit: New OMSimulator Model Dialog

Add System
----------

A weakly coupled system (co-simulation) can integrate strongly coupled system
(model exchange). Therefore, the weakly coupled system must to be selected from
the Libraries Browser and the respective menu item can be selected:

.. figure :: images/omedit_04.png

  OMEdit: Add System

That will pop-up a dialog to enter the names of the new system.

.. figure :: images/omedit_05.png

  OMEdit: Add System Dialog

Add SubModel
------------

A sub-model is typically an FMU, but it also can be result file. In order to
import a sub-model, the respective system must be selected and the action can
be selected from the menu bar:

.. figure :: images/omedit_06.png

  OMEdit: Add SubModel

That will pop-up a dialog to enter the names of the new sub-model.

.. figure :: images/omedit_07.png

  OMEdit: Add SubModel Dialog

Simulate
--------

Select the simulate item from the OMSimulator menu.
