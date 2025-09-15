initialize
----------

Initializes an instantiated SSP model and prepares it for simulation.

   **Notes**:
    * Must be called **after** `instantiate()` and **before** `simulate()`.
    * Applies all parameter and connector start values at runtime.
    * Performs consistency checks on component connections and initial conditions.
    * Initializes all FMU components and sets the model to a “ready-to-simulate” state.
    * Does not perform the actual simulation; only prepares the model for it.

**Syntax:**

.. code-block:: python

   initialize()

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

   # Initialize the model before simulation
   instantiated_model.initialize()
