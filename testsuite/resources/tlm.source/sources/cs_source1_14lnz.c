/* Linearization */
#include "cs_source1_model.h"
#if defined(__cplusplus)
extern "C" {
#endif

const char *cs_source1_linear_model_frame()
{
  return "model linear_source1\n  parameter Integer n = 0; // states\n  parameter Integer k = 0; // top-level inputs\n  parameter Integer l = 1; // top-level outputs\n"
  "  parameter Real x0[0] = {%s};\n"
  "  parameter Real u0[0] = {%s};\n"
  "  parameter Real A[0,0] = zeros(0,0);%s\n"
  "  parameter Real B[0,0] = zeros(0,0);%s\n"
  "  parameter Real C[1,0] = zeros(1,0);%s\n"
  "  parameter Real D[1,0] = zeros(1,0);%s\n"
  "  Real x[0];\n"
  "  input Real u[0];\n"
  "  output Real y[1];\n"
  "\n    Real 'y_y' = y[1];\n\n"
  "equation\n  der(x) = A * x + B * u;\n  y = C * x + D * u;\nend linear_source1;\n";
}
const char *cs_source1_linear_model_datarecovery_frame()
{
  return "model linear_source1\n  parameter Integer n = 0; // states\n  parameter Integer k = 0; // top-level inputs\n  parameter Integer l = 1; // top-level outputs\n  parameter Integer nz = 2; // data recovery variables\n"
  "  parameter Real x0[0] = {%s};\n"
  "  parameter Real u0[0] = {%s};\n"
  "  parameter Real z0[2] = {%s};\n"
  "  parameter Real A[0,0] = zeros(0,0);%s\n"
  "  parameter Real B[0,0] = zeros(0,0);%s\n"
  "  parameter Real C[1,0] = zeros(1,0);%s\n"
  "  parameter Real D[1,0] = zeros(1,0);%s\n"
  "  parameter Real Cz[2,0] = zeros(2,0);%s\n"
  "  parameter Real Dz[2,0] = zeros(2,0);%s\n"
  "  Real x[0];\n"
  "  input Real u[0];\n"
  "  output Real y[1];\n"
  "  output Real z[2];\n"
  "\n  Real 'y_y' = y[1];\n  Real 'z_$cse1' = z[1];\n  Real 'z_y' = z[2];\n\n"
  "equation\n  der(x) = A * x + B * u;\n  y = C * x + D * u;\n  z = Cz * x + Dz * u;\nend linear_source1;\n";
}
#if defined(__cplusplus)
}
#endif

