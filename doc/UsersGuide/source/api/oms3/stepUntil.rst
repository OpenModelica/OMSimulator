stepUntil
--------

Runs the simulation of an instantiated SSP model or a fmu until a specified time point or in a simulation loop.

   **Notes**:
    * Must be called **after** `initialize()`.
    * Simulation uses start values from SSV files and any parameters overridden via `setValue`.
    * Logged results are saved in the format defined by :code:`setResultFile()` (commonly .mat).
    * Simulation is performed on the active variant of the SSP model.

**Syntax:**

.. code-block:: python

   stepUntil(stopTime: float)

**Example usage**:

.. code-block:: python

  from OMSimulator import FMU, Variable, SignalType, Causality

  fmu = FMU('../resources/Modelica.Blocks.Sources.Sine.fmu', instanceName='Sine')

  fmu.instantiate()
  fmu.setResultFile('Sine_res.mat')

  fmu.initialize()

  current_time = 0.0
  stop_time = 1.0
  step_size = 0.1
  while current_time < stop_time:
    # advance simulation to the next time
    current_time = round(current_time + step_size, 10) # avoid floating point arithmetic issues
    fmu.stepUntil(current_time)
    print(f"info:    time: {current_time}, y: {fmu.getValue('y')}", flush=True)

  fmu.terminate()
  fmu.delete()