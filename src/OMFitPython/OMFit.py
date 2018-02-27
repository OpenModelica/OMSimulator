# -*- coding: utf-8 -*-
"""Python module for parameter estimation based OMSimulator.

Todo:
    * Complete documentation, e.g., following the example given at http://sphinxcontrib-napoleon.readthedocs.io/en/latest/example_google.html

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html
.. _NumPy Documentation HOWTO:
   https://github.com/numpy/numpy/blob/master/doc/HOWTO_DOCUMENT.rst.txt
.. _ctypes -- A foreign function library for Python:
   https://docs.python.org/3.6/library/ctypes.html
.. _ctypes tutorial:
   http://python.net/crew/theller/ctypes/tutorial.html
.. _numpy support for C-Types:
   https://docs.scipy.org/doc/numpy/reference/routines.ctypeslib.html
"""
import numpy as np
from numpy.ctypeslib import ndpointer
import ctypes, sys
from ctypes import cdll

# Implementation notes
# author: Bernhard Thiele
# I'm not an expert in python and even less in its foreign function interface,
# above are some notes that guided my implementation and my understanding of the
# involved issues. Please fix me where needed...
#
# For mapping `char const* const*` to python one may use `ctypes.POINTER(c_char_p)`
# If there is no const guarantee, one should use
# ctypes.POINTER(ctypes.POINTER(ctypes.c_char) together with ctypes.create_string_buffer(..)
# see http://code.activestate.com/lists/python-list/704158/


class OMFit:
    """Parameter estimation based on OMSimulator (composite) model."""
    def __init__(self,model):
        """Create an em empty model for parameter estimation.

        Args:
            model (object): The OMS (composite) model.

        Returns:
            Fitting model object.

        """
        if (sys.platform == 'win32'):
            self.obj=cdll.LoadLibrary("OMFit.dll")
        else:
            self.obj=cdll.LoadLibrary("libOMFit.so")
        self.setCtypesArguments()
        self.setResultTypes()
        self.fitmodel = self.obj.omsfit_newFitModel(model)

    def __del__(self):
        """Destructor will free external C object."""
        self.obj.omsfit_freeFitModel(self.fitmodel)

    def setResultTypes(self):
        self.obj.omsfit_newFitModel.restype = ctypes.c_void_p
        self.obj.omsfit_describe.restype = ctypes.c_int
        self.obj.omsfit_initialize.restype = ctypes.c_int
        self.obj.omsfit_addMeasurement.restype = ctypes.c_int
        self.obj.omsfit_addParameter.restype = ctypes.c_int
        self.obj.omsfit_getParameter.restype = ctypes.c_int
        self.obj.omsfit_solve.restype = ctypes.c_int
        self.obj.omsfit_setOptions_max_num_iterations.restype = ctypes.c_int
        self.obj.omsfit_getState.restype = ctypes.c_int

    def setCtypesArguments(self):
        self.obj.omsfit_newFitModel.argtypes = [ctypes.c_void_p]
        self.obj.omsfit_freeFitModel.argtypes = [ctypes.c_void_p]
        self.obj.omsfit_describe.argtypes = [ctypes.c_void_p]
        self.obj.omsfit_initialize.argtypes = [
            ctypes.c_void_p, ctypes.c_int,
            ndpointer(dtype=np.float64,ndim=1,flags='C_CONTIGUOUS'), ctypes.c_int,
            ctypes.POINTER(ctypes.c_char_p), ctypes.c_int,
            ctypes.POINTER(ctypes.c_char_p), ctypes.c_int]
        self.obj.omsfit_addMeasurement.argtypes = [
            ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p,
            ndpointer(dtype=np.float64,ndim=1,flags='C_CONTIGUOUS'), ctypes.c_int]
        self.obj.omsfit_addParameter.argtypes = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double]
        self.obj.omsfit_getParameter.argtype = [ctypes.c_void_p, ctypes.c_char_p,
            ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
        self.obj.omsfit_solve.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.obj.omsfit_setOptions_max_num_iterations.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.obj.omsfit_getState.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int)]


    def initialize(self, nSeries, time, inputvars, measurementvars):
        invars = (ctypes.c_char_p * len(inputvars))()
        invars[:] = inputvars
        mesvars = (ctypes.c_char_p * len(measurementvars))()
        mesvars[:] = measurementvars
        return self.obj.omsfit_initialize(self.fitmodel, nSeries,
            time, len(time),
            invars, len(inputvars),
            mesvars, len(measurementvars))

    def describe(self):
        """Print summary of fitting model"""
        return self.obj.omsfit_describe(self.fitmodel)

    def addMeasurement(self, iSeries, var, values):
        return self.obj.omsfit_addMeasurement(self.fitmodel, iSeries,
            var, values, len(values))

    def addParameter(self, var, startvalue):
        return self.obj.omsfit_addParameter(self.fitmodel, var, startvalue)

    def getParameter(self, var):
        startvalue = ctypes.c_double()
        estimatedvalue = ctypes.c_double()
        status = self.obj.omsfit_getParameter(
            self.fitmodel, var, ctypes.byref(startvalue), ctypes.byref(estimatedvalue))
        return (status, startvalue.value, estimatedvalue.value)

    def solve(self, reporttype):
        return self.obj.omsfit_solve(self.fitmodel, reporttype)

    def setOptions_max_num_iterations(self, max_num_iterations):
        return self.obj.omsfit_setOptions_max_num_iterations(self.fitmodel, max_num_iterations)

    def getState(self):
        """Get state of fitting model object.

        Returns:
            int: oms_status code
            int: omsfit_fitmodelstate code

        """
        state = ctypes.c_int()
        status = self.obj.omsfit_getState(self.fitmodel, ctypes.byref(state))
        return (status, state.value)

    @staticmethod
    def oms_status_str(status):
        """Mapping of enum C-API status code (oms_status_enu_t) to string.

        Args:
            status (int): The C-API status code.

        Returns:
            String representation of status code.

        C enum::

            typedef enum {
              oms_status_ok,
              oms_status_warning,
              oms_status_discard,
              oms_status_error,
              oms_status_fatal,
              oms_status_pending
            } oms_status_enu_t;

        """
        oms_status_t_dict = {0: "oms_status_ok", 1: "oms_status_warning", 2: "oms_status_discard", 3: "oms_status_error", 4: "oms_status_fatal", 5: "oms_status_pending"}
        return oms_status_t_dict[status]

    @staticmethod
    def omsfit_fitmodelstate_str(state):
        """Mapping of enum C-API state code (omsfit_modelstate_t) to string.

        Args:
            state (int): State of fitting model.

        Returns:
            String representation of state code.

        C enum::

            typedef enum {
              omsfit_fitmodelstate_constructed,    //!< After omsfit_newFitModel
              omsfit_fitmodelstate_initialized,    //!< After omsfit_initialize
              omsfit_fitmodelstate_convergence,    //!< After omsfit_solve if Ceres minimizer returned with ceres::TerminationType::CONVERGENCE
              omsfit_fitmodelstate_no_convergence, //!< After omsfit_solve if Ceres minimizer returned with ceres::TerminationType::NO_CONVERGENCE
              omsfit_fitmodelstate_failure         //!< After omsfit_solve if Ceres minimizer returned with ceres::TerminationType::FAILURE
            } omsfit_fitmodelstate_t;

        """
        omsfit_fitmodelstate_t_dict = {0: "omsfit_fitmodelstate_constructed", 1: "omsfit_fitmodelstate_initialized", 2: "omsfit_fitmodelstate_convergence", 3: "omsfit_fitmodelstate_no_convergence", 4: "omsfit_fitmodelstate_failure"}
        return omsfit_fitmodelstate_t_dict[state]
