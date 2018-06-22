.. index:: OMSysIdent

OMSysIdent
==========

OMSysIdent is a module for the parameter estimation of behavioral models
(wrapped as FMUs) on top of the OMSimulator API.
It uses the Ceres solver (http://ceres-solver.org/) for the optimization task.

It is an optional module which can be disabled. Please check the build files
for your platform for the respective flags.

.. index:: OMSysIdent; Examples

Examples
########

There are examples in the testsuite below the subfolder `OMSysIdent` which use
the scripting API. In addition there are examples which directly use
the C API within the module's source code folder (`src/OMSysIdentLib`).

Below is a basic example from the testsuite (`HelloWorld_cs_Fit.lua`) which
uses the Lua scripting API. It determines the parameters for the following "hello world"
style Modelica model:

.. code-block:: Modelica

  model HelloWorld
    parameter Real a = -1;
    parameter Real x_start = 1;
    Real x(start=x_start, fixed=true);
  equation
    der(x) = a*x;
  end HelloWorld;

The goal is to estimate the value of the coefficent `a` and the initial value
`x_start` of the state variable `x`. Instead of real measurements, the script
simply uses simulation data generated from the `HelloWorld` examples as
measurement data. The array `data_time` contains the time instants at which a sample is
taken and the array `data_x` contains the value of `x` that corresponds to the
respective time instant.

The estimation parameters are defined by calls to function `omsi_addParameter(..)`
in which the name of the parameter and a first guess for the parameter's
value is stated.

.. code-block:: lua
  :caption: HelloWorld_cs_Fit.lua
  :name: HelloWorld_cs_Fit-lua

  oms2_setLogFile("HelloWorld_cs_Fit.log")
  oms2_setTempDirectory(".")
  oms2_newFMIModel("HelloWorld_cs_Fit")
  -- instantiate FMU
  -- add FMU
  status = oms2_addFMU("HelloWorld_cs_Fit", "../FMUs/HelloWorld_cs.fmu", "HelloWorld")

  -- create system identification model for model
  simodel = omsi_newSysIdentModel("HelloWorld_cs_Fit");
  -- omsi_describe(simodel)

  -- Data generated from simulating HelloWorld.mo for 1.0s with Euler and 0.1s step size
  kNumSeries = 1;
  kNumObservations = 11;
  data_time = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};
  inputvars = {};
  measurementvars = {"HelloWorld_cs_Fit.HelloWorld:x"};
  data_x = {1, 0.9, 0.8100000000000001, 0.7290000000000001, 0.6561, 0.5904900000000001, 0.5314410000000001, 0.4782969000000001, 0.43046721, 0.387420489, 0.3486784401};

  omsi_initialize(simodel, kNumSeries, data_time, inputvars,
    measurementvars)
  -- omsi_describe(simodel)

  omsi_addParameter(simodel, "HelloWorld_cs_Fit.HelloWorld:x_start", 0.5);
  omsi_addParameter(simodel, "HelloWorld_cs_Fit.HelloWorld:a", -0.5);
  omsi_addMeasurement(simodel, 0, "HelloWorld_cs_Fit.HelloWorld:x", data_x);
  -- omsi_describe(simodel)

  omsi_setOptions_max_num_iterations(simodel, 25)
  omsi_solve(simodel, "BriefReport")

  status, simodelstate = omsi_getState(simodel);
  print(status, simodelstate)

  status, startvalue1, estimatedvalue1 = omsi_getParameter(simodel, "HelloWorld_cs_Fit.HelloWorld:a")
  status, startvalue2, estimatedvalue2 = omsi_getParameter(simodel, "HelloWorld_cs_Fit.HelloWorld:x_start")
  print("HelloWorld.a startvalue=" .. startvalue1 .. ", estimatedvalue=" .. estimatedvalue1)
  print("HelloWorld.x_start startvalue=" .. startvalue2 .. ", estimatedvalue=" .. estimatedvalue2)
  is_OK1 = estimatedvalue1 > -1.1 and estimatedvalue1 < -0.9
  is_OK2 = estimatedvalue2 > 0.9 and estimatedvalue2 < 1.1
  print("HelloWorld.a estimation is OK: " .. tostring(is_OK1))
  print("HelloWorld.x_start estimation is OK: " .. tostring(is_OK2))

  omsi_freeSysIdentModel(simodel)
  oms2_unloadModel("HelloWorld_cs_Fit")

Running the script generates the following console output:

::

  iter      cost      cost_change  |gradient|   |step|    tr_ratio  tr_radius  ls_iter  iter_time  total_time
   0  4.034320e-01    0.00e+00    2.19e+00   0.00e+00   0.00e+00  1.00e+04        0    2.35e-02    2.35e-02
   1  3.821520e-02    3.65e-01    4.11e-01   9.87e-01   9.06e-01  2.15e+04        1    2.90e-02    5.25e-02
   2  6.837776e-04    3.75e-02    5.19e-02   3.58e-01   9.83e-01  6.46e+04        1    2.77e-02    8.02e-02
   3  1.354499e-07    6.84e-04    6.08e-04   4.58e-02   1.00e+00  1.94e+05        1    2.96e-02    1.10e-01
   4  5.854620e-15    1.35e-07    1.09e-07   7.22e-04   1.00e+00  5.82e+05        1    3.08e-02    1.41e-01
   5  1.160287e-25    5.85e-15    2.26e-13   1.59e-07   1.00e+00  1.74e+06        1    2.86e-02    1.69e-01
  Ceres Solver Report: Iterations: 6, Initial cost: 4.034320e-01, Final cost: 1.160287e-25, Termination: CONVERGENCE

  =====================================
  Total duration for parameter estimation: 169msec.
  Result of parameter estimation (check 'Termination' status above whether solver converged):

  HelloWorld_cs_Fit.HelloWorld:a(start=-0.5, *estimate*=-1.04807)
  HelloWorld_cs_Fit.HelloWorld:x_start(start=0.5, *estimate*=1)

  =====================================
  0	convergence
  HelloWorld.a startvalue=-0.5, estimatedvalue=-1.0480741793778
  HelloWorld.x_start startvalue=0.5, estimatedvalue=0.99999999999972
  HelloWorld.a estimation is OK: true
  HelloWorld.x_start estimation is OK: true
  info:    Logging information has been saved to "HelloWorld_cs_Fit.log"

.. index:: OMSysIdent; Scripting Commands

Lua Scripting Commands
######################

.. include:: OMSysIdentLua.inc
