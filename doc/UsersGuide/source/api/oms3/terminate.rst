terminate and delete
--------------------

Terminates the simulation of an instantiated SSP model.It releases internal simulation resources,
closes loggers, and ensures that the model state is marked as terminated.

**Syntax:**

.. code-block:: python

   terminate()
   delete()

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

   # Initialize and run simulation
   instantiated_model.initialize()
   instantiated_model.simulate()

   # Terminate the simulation
   instantiated_model.terminate()
   # Clean up resources
   instantiated_model.delete()
