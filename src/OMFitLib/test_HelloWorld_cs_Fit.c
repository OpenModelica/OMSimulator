/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "OMSimulator.h"
#include "OMFit.h"

// Data generated from simulating HelloWorld.mo for 1.0s with Euler and 0.1s step size
const int kNumSeries = 1;
const int kNumObservations = 11;
const double data_time[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};
const char* inputvars[] = {""};
const char* measurementvars[] = {"HelloWorld.x"};
const double data_x[] = {1, 0.9, 0.8100000000000001, 0.7290000000000001, 0.6561, 0.5904900000000001, 0.5314410000000001, 0.4782969000000001, 0.43046721, 0.387420489, 0.3486784401};

// Macro for basic assertion testing (works conveniently with ctest, see CMakeLists.txt)
#define ASSERT(X)  if(!(X)) {fprintf(stderr, "%s:%d Assertion '" #X "' FAILED", __FILE__, __LINE__); exit(1);}

int test_HelloWorld_cs_Fit()
{
  const char* version = oms_getVersion();
  oms_status_enu_t status;
  //std::cout << version << std::endl;

  void* model = oms_newModel();
  oms_setTempDirectory(".");
  oms_instantiateFMU(model, "../FMUs/HelloWorld_cs.fmu", "HelloWorld");
  oms_setTolerance(model, 1e-5);

  void* fitmodel = omsfit_newFitModel(model);
  status = omsfit_initialize(fitmodel, kNumSeries, data_time, kNumObservations, inputvars, 0,
  measurementvars, 1);
  ASSERT(status == oms_status_ok);
  status = omsfit_addParameter(fitmodel, "HelloWorld.x_start", 0.5);
  ASSERT(status == oms_status_ok);
  status = omsfit_addParameter(fitmodel, "HelloWorld.a", -0.5);
  ASSERT(status == oms_status_ok);
  status = omsfit_addMeasurement(fitmodel, 0, "HelloWorld.x", data_x, kNumObservations);
  ASSERT(status == oms_status_ok);
  // give a summary of the  object
  omsfit_describe(fitmodel);

  status = omsfit_solve(fitmodel, "BriefReport");
  ASSERT(status == oms_status_ok);
  omsfit_fitmodelstate_t fitmodelstate;
  status = omsfit_getState(fitmodel, &fitmodelstate);
  ASSERT(status == oms_status_ok);
  ASSERT(fitmodelstate == omsfit_fitmodelstate_convergence);
  double startvalue, estimatedvalue;
  status = omsfit_getParameter(fitmodel, "HelloWorld.x_start", &startvalue, &estimatedvalue);
  ASSERT(status == oms_status_ok);
  ASSERT(estimatedvalue > 0.99 && estimatedvalue < 1.01);
  printf("HelloWorld.x_start: startvalue %f, estimatedvalue %f\n", startvalue, estimatedvalue);
  status = omsfit_getParameter(fitmodel, "HelloWorld.a", &startvalue, &estimatedvalue);
  ASSERT(status == oms_status_ok);
  ASSERT(estimatedvalue > -1.1 && estimatedvalue < -0.9);
  printf("HelloWorld.a: startvalue %f, estimatedvalue %f\n", startvalue, estimatedvalue);

  omsfit_freeFitModel(fitmodel);
  oms_unload(model);
  return 0;
}

int main() {
  return test_HelloWorld_cs_Fit();
}
