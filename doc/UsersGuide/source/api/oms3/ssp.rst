SSP
---

The ``ssp`` module provides a high-level interface for creating, importing,
and managing SSP (System Structure and Parameterization) models.
SSP models are used to describe complex system architectures by connecting multiple components and
defining their parameters, enabling seamless simulation and co-simulation workflows.

.. code-block:: python

  from OMSimulator import SSP, Settings
  Settings.suppressPath = True

  # Create a new, empty SSP model instance
  model = SSP()

  # Create a new, empty SSP model with a custom model name and root system name
  model = SSP(model_name="model", system_name="root")

  # Load an existing SSP model from a file
  model = SSP("PIController.ssp")

  # list the ssp components
  model.list()

``model_name`` and ``system_name`` are optional. ``model_name`` defaults to
``"default"`` and ``system_name`` defaults to ``model_name``. The model name is
the first element of every component reference, so a model created with
``model_name="model"`` is addressed as ``CRef('model', 'Add1')``. Both arguments
are ignored when an existing SSP file is loaded, because the names are then read
from the file.

Once created or loaded, an ``SSP`` model instance allows you to:

- Add and configure components.
- Connect signals between components.
- Define parameters and experiment settings.
- Export the model for simulation or further analysis.