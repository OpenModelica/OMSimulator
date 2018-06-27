# -*- coding: utf-8 -*-
"""Python module for parameter estimation based on OMSimulator.

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


class OMSysIdent:
    """Parameter estimation based on OMSimulator (composite) model."""
    def __init__(self,ident):
        """Create an em empty model for parameter estimation.

        Args:
            ident (string): Name of the model instance.

        Returns:
            SysIdent model object.

        """
        if (sys.platform == 'win32'):
            self.obj=cdll.LoadLibrary("OMSimulator.dll")
        else:
            self.obj=cdll.LoadLibrary("libOMSimulator.so")

        self.obj.omsi_newSysIdentModel.argtypes = [ctypes.c_char_p]
        self.obj.omsi_newSysIdentModel.restype = ctypes.c_void_p

        self.obj.omsi_freeSysIdentModel.argtypes = [ctypes.c_void_p]

        self.obj.omsi_describe.argtypes = [ctypes.c_void_p]
        self.obj.omsi_describe.restype = ctypes.c_int

        self.obj.omsi_initialize.argtypes = [
            ctypes.c_void_p, ctypes.c_int,
            ndpointer(dtype=np.float64,ndim=1,flags='C_CONTIGUOUS'), ctypes.c_int,
            ctypes.POINTER(ctypes.c_char_p), ctypes.c_int,
            ctypes.POINTER(ctypes.c_char_p), ctypes.c_int]
        self.obj.omsi_initialize.restype = ctypes.c_int

        self.obj.omsi_addMeasurement.argtypes = [
            ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p,
            ndpointer(dtype=np.float64,ndim=1,flags='C_CONTIGUOUS'), ctypes.c_int]
        self.obj.omsi_addMeasurement.restype = ctypes.c_int

        self.obj.omsi_addParameter.argtypes = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double]
        self.obj.omsi_addParameter.restype = ctypes.c_int

        self.obj.omsi_getParameter.argtype = [ctypes.c_void_p, ctypes.c_char_p,
            ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
        self.obj.omsi_getParameter.restype = ctypes.c_int

        self.obj.omsi_solve.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.obj.omsi_solve.restype = ctypes.c_int

        self.obj.omsi_setOptions_max_num_iterations.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.obj.omsi_setOptions_max_num_iterations.restype = ctypes.c_int

        self.obj.omsi_getState.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int)]
        self.obj.omsi_getState.restype = ctypes.c_int

        self.simodel = self.obj.omsi_newSysIdentModel(ident)
        self.ident = ident

    def __del__(self):
        """Destructor will free external C object."""
        self.obj.omsi_freeSysIdentModel(self.simodel)

    def initialize(self, nSeries, time, inputvars, measurementvars):
        fq_inputvars = list(map(lambda x: self.ident + "." + x, inputvars))
        invars = (ctypes.c_char_p * len(fq_inputvars))()
        invars[:] = fq_inputvars
        fq_measurementvars = list(map(lambda x: self.ident + "." + x, measurementvars))
        mesvars = (ctypes.c_char_p * len(fq_measurementvars))()
        mesvars[:] = fq_measurementvars
        return self.obj.omsi_initialize(self.simodel, nSeries,
            time, len(time),
            invars, len(fq_inputvars),
            mesvars, len(fq_measurementvars))

    def describe(self):
        """Print summary of SysIdent model"""
        return self.obj.omsi_describe(self.simodel)

    def addMeasurement(self, iSeries, var, values):
        fq_var = self.ident + "." + var
        return self.obj.omsi_addMeasurement(self.simodel, iSeries,
            fq_var, values, len(values))

    def addParameter(self, var, startvalue):
        fq_var = self.ident + "." + var
        return self.obj.omsi_addParameter(self.simodel, fq_var, startvalue)

    def getParameter(self, var):
        fq_var = self.ident + "." + var
        startvalue = ctypes.c_double()
        estimatedvalue = ctypes.c_double()
        status = self.obj.omsi_getParameter(
            self.simodel, fq_var, ctypes.byref(startvalue), ctypes.byref(estimatedvalue))
        return (status, startvalue.value, estimatedvalue.value)

    def solve(self, reporttype):
        return self.obj.omsi_solve(self.simodel, reporttype)

    def setOptions_max_num_iterations(self, max_num_iterations):
        return self.obj.omsi_setOptions_max_num_iterations(self.simodel, max_num_iterations)

    def getState(self):
        """Get state of SysIdent model object.

        Returns:
            int: oms_status code
            int: omsi_simodelstate code

        """
        state = ctypes.c_int()
        status = self.obj.omsi_getState(self.simodel, ctypes.byref(state))
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
    def omsi_simodelstate_str(state):
        """Mapping of enum C-API state code (omsi_simodelstate_t) to string.

        Args:
            state (int): State of SysIdent model.

        Returns:
            String representation of state code.

        C enum::

            typedef enum {
              omsi_simodelstate_constructed,    //!< After omsi_newSysIdentModel
              omsi_simodelstate_initialized,    //!< After omsi_initialize
              omsi_simodelstate_convergence,    //!< After omsi_solve if Ceres minimizer returned with ceres::TerminationType::CONVERGENCE
              omsi_simodelstate_no_convergence, //!< After omsi_solve if Ceres minimizer returned with ceres::TerminationType::NO_CONVERGENCE
              omsi_simodelstate_failure         //!< After omsi_solve if Ceres minimizer returned with ceres::TerminationType::FAILURE
            } omsi_simodelstate_t;

        """
        omsi_simodelstate_t_dict = {0: "omsi_simodelstate_constructed", 1: "omsi_simodelstate_initialized", 2: "omsi_simodelstate_convergence", 3: "omsi_simodelstate_no_convergence", 4: "omsi_simodelstate_failure"}
        return omsi_simodelstate_t_dict[state]
