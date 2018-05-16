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

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

#include "OMSimulator.h"
#include "OMSysIdent.h"
#include "Eigen/Core"
#ifdef ROOT_VERSION
#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TROOT.h"
#include "TFitResult.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"
#include "TVector.h"
#endif // ROOT_VERSION

// Macro for basic assertion testing (works conveniently with ctest, see CMakeLists.txt)
#define ASSERT(X)  if(!(X)) {fprintf(stderr, "%s:%d Assertion '" #X "' FAILED\n", __FILE__, __LINE__); exit(1);}

// Macros for brief code
#define MODELIDENT "test_Lin2DimODE_cs_Fit"
#define FMUIDENT "Lin2DimODE"
#define VARCREF(x) MODELIDENT "." FMUIDENT ":" x

using std::abs;
using Eigen::Dynamic;
using Eigen::RowMajor;
typedef Eigen::Matrix<double, Dynamic, 1> Vector;
typedef Eigen::Matrix<double, Dynamic, Dynamic, RowMajor> Matrix;


void the_reference_ode(const char* oms_modelident, const std::vector<std::string>& p_name, const Vector& p, const Vector& t, const std::vector<std::string>& x_name, Matrix& x) {
  for (int i=0; i < t.size(); ++i) {
    for (int j=0; j < p_name.size(); ++j) {
        oms2_setReal(p_name[j].c_str(), p[j]);
    }
    oms2_initialize(oms_modelident);

    if (t[i] > 0) oms2_stepUntil(oms_modelident, t[i]);

    for (int j=0; j < x_name.size(); ++j) {
      oms2_getReal(x_name[j].c_str(), &(x(i, j)));
    }
    oms2_reset(oms_modelident);
  }
}


const int kNumSeries = 1;
const int kNumObservations = 11;
const char* inputvars[] = {""};
const char* measurementvars[] = {VARCREF("x1"), VARCREF("x2")};
const std::vector<std::string> parametervars = {VARCREF("a11"), VARCREF("a12"), VARCREF("a21"), VARCREF("a22"), VARCREF("x1_start"), VARCREF("x2_start")};


int test_Lin2DimODE_cs_Fit()
{
  const char* version = oms2_getVersion();
  oms_status_enu_t status;
  //std::cout << version << std::endl;

  // set observation times
  Vector data_time = Vector::Zero(kNumObservations);
  for (int i=1; i < kNumObservations; ++i) {
    data_time[i] = data_time[i-1] + 0.1;
  }

  oms2_setLogFile("test_Lin2DimODE_cs_Fit.log");
  status = oms2_setTempDirectory(".");
  ASSERT(status == oms_status_ok);

  const char* oms_modelident = MODELIDENT;
  status = oms2_newFMIModel(oms_modelident);
  ASSERT(status == oms_status_ok);
  status = oms2_addFMU(oms_modelident, "../FMUs/Lin2DimODE_cs.fmu", FMUIDENT);
  // oms_setTolerance(model, 1e-5); // FIXME How to set the tolerance with the oms2 API?
  status = oms2_setStopTime(oms_modelident, data_time[kNumObservations - 1]); // needed?

  // compute reference
  // der(x1) = a11*x1 + a12*x2;
  // der(x2) = a21*x1 + a22*x2;
  // x1(0) = x1_start;
  // x2(0) = x2_start;
  Vector p_ref(6);
  p_ref << -1, 0.1, 0.1, -0.9, 1, 1.1;
  const std::vector<std::string> mesvars = std::vector<std::string>(measurementvars, measurementvars+2);
  Matrix x_ref = Matrix::Zero(kNumObservations, mesvars.size());
  the_reference_ode(oms_modelident, parametervars, p_ref, data_time, mesvars, x_ref);
  std::cout << "i \t t \t x1 \t x2\n";
  for (int i=0; i < data_time.size(); ++i) {
    std::cout << i << "\t" << data_time[0] << "\t" << x_ref(i, 0) << "\t" << x_ref(i, 1) << std::endl;
  }


  void* fitmodel = omsi_newSysIdentModel(oms_modelident);
  double* t_ref = new double[kNumObservations];
  for (int i=0; i<kNumObservations; ++i) t_ref[i] = data_time[i];
  status = omsi_initialize(fitmodel, kNumSeries, t_ref, kNumObservations, inputvars, 0,
  measurementvars, 2);
  ASSERT(status == oms_status_ok);
  delete[] t_ref;

  for (int i=0; i<parametervars.size(); ++i) {
    status = omsi_addParameter(fitmodel, parametervars[i].c_str(), 1.5*p_ref[i]);
    ASSERT(status == oms_status_ok);
  }

  double* x_mes = new double[kNumObservations];
  for (int i=0; i<2; ++i) {
    for (int j=0; j<kNumObservations; ++j) {
      x_mes[j] = x_ref(j, i);
    }
    status = omsi_addMeasurement(fitmodel, 0, measurementvars[i], x_mes, kNumObservations);
    ASSERT(status == oms_status_ok);
  }
  delete[] x_mes;

  // give a summary of the  object
  omsi_describe(fitmodel);

  status = omsi_solve(fitmodel, "BriefReport");
  ASSERT(status == oms_status_ok);
  omsi_simodelstate_t fitmodelstate;
  status = omsi_getState(fitmodel, &fitmodelstate);
  ASSERT(status == oms_status_ok);
  ASSERT(fitmodelstate == omsi_simodelstate_convergence);
  std::cout << "parameter\treference\testimated\t(ref - est)/ref\n";
  Vector p_est(parametervars.size());
  for (int i=0; i<parametervars.size(); ++i) {
    double startvalue, estimatedvalue;
    status = omsi_getParameter(fitmodel, parametervars[i].c_str(), &startvalue, &estimatedvalue);
    ASSERT(status == oms_status_ok);
    std::cout << parametervars[i] << "\t" << p_ref[i] << "\t" << estimatedvalue << "\t" << (p_ref[i] - estimatedvalue) / p_ref[i] << std::endl;
    // Assure that solution is close to the reference up to an (arbitrarily chosen) factor
    ASSERT(abs(estimatedvalue) > (abs(p_ref[i]) - 0.01*abs(p_ref[i])) && abs(estimatedvalue) < (abs(p_ref[i]) + 0.01*abs(p_ref[i])));
    p_est[i] = estimatedvalue;
  }

// Create graphs if exected by CERN Root
#ifdef ROOT_VERSION
  // Solve ODE with estimated parameters
  Matrix x_est = Matrix::Zero(kNumObservations, mesvars.size());
  the_reference_ode(oms_modelident, parametervars, p_est, data_time, mesvars, x_est);
  // Convert to root data structures
  TVectorF t_t_ref(kNumObservations), t_x1_ref(kNumObservations), t_x2_ref(kNumObservations), t_x1_est(kNumObservations), t_x2_est(kNumObservations);
  for (int i=0; i<kNumObservations; ++i) {
    t_t_ref[i] = data_time[i];
    t_x1_ref[i] = x_ref(i,0);
    t_x2_ref[i] = x_ref(i,1);
    t_x1_est[i] = x_est(i,0);
    t_x2_est[i] = x_est(i,1);
  }

  // The canvas on which we'll draw the graph
  TCanvas *c1 = new TCanvas("c1","test_Lin2DimODE_Fit",700,500);
  c1->SetGrid();
  auto mg = new TMultiGraph();
  TGraph *gexpect1 = new TGraph(t_t_ref, t_x1_ref);
  gexpect1->SetLineColor(kBlue);
  gexpect1->SetMarkerStyle(2);
  // gexpect1->SetLineWidth(50);
  mg->Add(gexpect1);
  TGraph *gexpect2 = new TGraph(t_t_ref, t_x2_ref);
  gexpect2->SetLineColor(kGreen);
  gexpect2->SetMarkerStyle(2);
  mg->Add(gexpect2);
  TGraph *gest1 = new TGraph(t_t_ref, t_x1_est);
  gest1->SetLineColor(kRed);
  gest1->SetMarkerStyle(4);
  mg->Add(gest1);
  TGraph *gest2 = new TGraph(t_t_ref, t_x2_est);
  gest2->SetLineColor(kGray);
  gest2->SetMarkerStyle(4);
  mg->Add(gest2);
  mg->Draw("apl");
  // Build and Draw a legend
  TLegend leg(.7,.7,.9,.9,"Ceres fitting experiment");
  leg.SetFillColor(0);
  gexpect1->SetFillColor(0);
  gexpect2->SetFillColor(0);
  gest1->SetFillColor(0);
  gest2->SetFillColor(0);
  leg.AddEntry(gexpect1,"Exp. x1");
  leg.AddEntry(gexpect2,"Exp. x2");
  leg.AddEntry(gest1,"Fitted x1");
  leg.AddEntry(gest2,"Fitted x2");
  leg.DrawClone("Same");
#endif // ROOT_VERSION

  omsi_freeSysIdentModel(fitmodel);
  oms2_unloadModel(oms_modelident);
  return 0;
}

int main() {
  return test_Lin2DimODE_cs_Fit();
}
