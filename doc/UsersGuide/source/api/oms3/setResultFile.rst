setResultFile
-------------

Specifies the file where simulation results of an instantiated SSP model will be stored.

   **Notes**:
    * The file path can be relative or absolute.
    * Supported formats depend on the simulator backend (commonly .mat or .csv files).
    * Must be called before `simulate()` to ensure proper logging of results.

**Syntax:**

.. code-block:: python

   setResultFile(file_path)

**Parameters:**
  - ``file_path`` (*str*): Path to the simulation result file.

**Example usage**:

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings

   Settings.suppressPath = True

   # Load SSP and instantiate model
   model = SSP("SimpleSimulation2.ssp")
   instantiated_model = model.instantiate()

   # Specify the result file
   instantiated_model.setResultFile("SimpleSimulation6_res.mat")
