simulate
--------

Runs the simulation of an instantiated SSP model.

   **Notes**:
    * Must be called **after** `initialize()`.
    * Simulation uses start values from SSV files and any parameters overridden via `setValue`.
    * Logged results are saved in the format defined by :code:`setResultFile()` (commonly .mat).
    * Simulation is performed on the active variant of the SSP model.

**Syntax:**

.. code-block:: python

   simulate()

**Example usage**:

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings

   Settings.suppressPath = True

   # Load SSP and instantiate model
   model = SSP("SimpleSimulation2.ssp")
   instantiated_model = model.instantiate()

   # Apply result file and runtime parameters
   instantiated_model.setResultFile("SimpleSimulation6_res.mat")
   instantiated_model.setValue(CRef("default", "Gain1", "k"), 2.0)
   instantiated_model.setValue(CRef("default", "Gain1", "u"), 6.0)

   # Initialize before simulation
   instantiated_model.initialize()

   # Run the simulation
   instantiated_model.simulate()
