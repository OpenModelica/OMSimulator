Solver
------

OMSimulator supports different solver methods depending on the FMI kind of the
components in the system. Solvers are configured by name in the Simulation
Setup dialog and assigned to individual FMU components.

Available Solvers
~~~~~~~~~~~~~~~~~

**Co-Simulation (CS) Solvers**

Co-simulation solvers act as masters that drive the co-simulation protocol.
They are used for FMUs that implement the co-simulation interface (CS FMUs).

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Method
     - Description
   * - ``oms_ma``
     - **Master Algorithm** — fixed-step co-simulation master. The step size
       is controlled by the ``fixedStepSize`` setting.
   * - ``oms_mav``
     - **Master Algorithm with Variable Step** — variable-step co-simulation
       master. Uses ``initialStepSize``, ``minimumStepSize``, and
       ``maximumStepSize`` to control the step size, and ``relativeTolerance``
       for error control.
   * - ``oms_mav2``
     - **Master Algorithm with Variable Step (v2)** — an improved variable-step
       co-simulation master with enhanced error estimation. Supports the same
       settings as ``oms_mav``.

**Model-Exchange (ME) Solvers**

Model-exchange solvers are ODE integrators used for FMUs that implement the
model-exchange interface (ME FMUs). They integrate the continuous-time
equations provided by the FMU.

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Method
     - Description
   * - ``cvode``
     - **CVODE** — variable-step, variable-order ODE solver from the SUNDIALS
       suite. Suitable for stiff and non-stiff systems. Supports
       ``initialStepSize``, ``minimumStepSize``, ``maximumStepSize``, and
       ``relativeTolerance``.
   * - ``euler``
     - **Explicit Euler** — simple fixed-step explicit Euler integrator.
       Suitable for non-stiff systems. Controlled by ``fixedStepSize``.

Solver Settings
~~~~~~~~~~~~~~~

The following settings can be configured for each named solver:

.. list-table::
   :header-rows: 1
   :widths: 25 20 55

   * - Setting
     - Applies to
     - Description
   * - ``relativeTolerance``
     - ``oms_mav``, ``oms_mav2``, ``cvode``
     - Relative error tolerance used for step-size control.
   * - ``fixedStepSize``
     - ``oms_ma``, ``euler``
     - The fixed communication / integration step size.
   * - ``initialStepSize``
     - ``oms_mav``, ``oms_mav2``, ``cvode``
     - The initial step size at the start of simulation.
   * - ``minimumStepSize``
     - ``oms_mav``, ``oms_mav2``, ``cvode``
     - The minimum allowed step size.
   * - ``maximumStepSize``
     - ``oms_mav``, ``oms_mav2``, ``cvode``
     - The maximum allowed step size.

Solver Assignment
~~~~~~~~~~~~~~~~~

Each FMU component in a system can be assigned one named solver configuration.
The available solvers are filtered based on the FMI kind of the component:

- **CS FMUs** — only co-simulation solvers (``oms_ma``, ``oms_mav``,
  ``oms_mav2``) are available.
- **ME FMUs** — only model-exchange solvers (``cvode``, ``euler``) are
  available.
- **me+cs FMUs** — all solvers are available.

If no solver is assigned to a component, the system default ``oms_ma`` is applied.

**Example**

The following example creates two named solver configurations — one using the
fixed-step Euler method and one using the variable-step CVODE solver — and
assigns them to individual components.

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings

   Settings.suppressPath = True

   model = SSP()

   # Add FMU resources
   model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
   model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

   # Instantiate components
   model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
   model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')
   model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')
   model.addComponent(CRef('default', 'Gain2'), 'resources/Gain.fmu')

   # Define solver configurations
   solver1 = {'name': 'solver1', 'method': 'euler', 'fixedStepSize': 1e-3}
   model.newSolver(solver1)

   solver2 = {'name': 'solver2', 'method': 'cvode', 'relativeTolerance': 1e-4,
              'initialStepSize': 1e-6, 'minimumStepSize': 1e-12, 'maximumStepSize': 0.001}
   model.newSolver(solver2)

   # Assign solvers to components
   model.setSolver(CRef('default', 'Add1'), 'solver1')
   model.setSolver(CRef('default', 'Add2'), 'solver2')
   model.setSolver(CRef('default', 'Gain1'), 'solver1')
   model.setSolver(CRef('default', 'Gain2'), 'solver2')
