getValue
--------

Retrieves the current value of a parameter or variable from a system, sub-system, or
component within the SSP model.

This method is typically used to query parameter values that have been set using
:code:`setValue`, or to inspect default values provided by FMUs or the SSP model.
It can be applied both before and after simulation, depending on whether the SSP has
been instantiated and executed.

   **Notes**:
    * If called before instantiation, it will return the value stored in the SSP model
      (either default or set explicitly by the user).
    * If called after instantation, it return the updated value depending on the FMU state.
    * Works for system-level, sub-system, and component parameters.
    * Returns a Python type corresponding to the variable type (``float`` for Real,
      ``int`` for Integer, ``bool`` for Boolean, etc.).

**Syntax:**

.. code-block:: python

   getValue(cref) -> value

**Parameters:**
  - ``cref`` (*CRef*): Reference to the parameter or variable whose value should be retrieved.
    For example, ``CRef("default", "Add1", "k1")`` refers to parameter ``k1`` of component ``Add1``.

**Returns:**
  - (*float* | *int* | *bool* | *str*): The current value of the requested parameter or variable.

**Example usage**:

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings

   Settings.suppressPath = True

   # Load an existing SSP model
   model = SSP("setValue2.ssp")

   # Retrieve values from top-level system parameter
   print(f"info:    default.param1 : {model.getValue(CRef('default', 'param1'))}", flush=True)

   # Retrieve values from FMU components
   print(f"info:    Add1.k1 : {model.getValue(CRef('default', 'Add1', 'k1'))}", flush=True)
   print(f"info:    Add2.k2 : {model.getValue(CRef('default', 'Add2', 'k2'))}", flush=True)
