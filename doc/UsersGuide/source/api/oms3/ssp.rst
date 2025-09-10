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

  # Load an existing SSP model from a file
  model = SSP("PIController.ssp")

  # list the ssp components
  model.list()

Once created or loaded, an ``SSP`` model instance allows you to:

- Add and configure components.
- Connect signals between components.
- Define parameters and experiment settings.
- Export the model for simulation or further analysis.