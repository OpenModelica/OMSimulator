instantiate
-----------

Instantiates an SSP model for simulation by creating an internal runtime representation.
After instantiation, users can modify parameter values on the instantiated model and
specify the simulation result file before running the simulation.

   **Notes**:
    * All start values from SSV files and inline SSP :code:`setValue` calls are applied initially.
    * Values set via :code:`setValue` on the instantiated model override the initial values.
    * The instantiated model is independent of the SSP file on disk
    * After instantiation, simulation methods such as :code:`initialize()`, :code:`simulate()`, :code:`terminate()`, and :code:`delete()` can be used.
    * The instantiated model should be deleted using :code:`delete()` to free resources after simulation.
    * :code:`setResultFile()` should be called before simulation to define the output file for logged results.

**Syntax:**

.. code-block:: python

   instantiated_model = model.instantiate() -> InstantiatedSSP

**Returns:**
  - (*InstantiatedSSP*): A runtime object representing the SSP model, ready for simulation.

**Example usage**:

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings

   # import SSP
   model2 = SSP("SimpleSimulation2.ssp")
   model2.list()

   # Instantiate the model for simulation
   instantiated_model = model2.instantiate()  # Start values from SSV and inline setValue are applied

   # Set result file
   instantiated_model.setResultFile("SimpleSimulation6_res.mat")

   # Override parameter values at runtime
   instantiated_model.setValue(CRef("default", "Gain1", "k"), 2.0)
   instantiated_model.setValue(CRef("default", "Gain1", "u"), 6.0)
