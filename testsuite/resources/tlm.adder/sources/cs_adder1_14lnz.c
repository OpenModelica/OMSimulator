/* Linearization */
#include "cs_adder1_model.h"
#if defined(__cplusplus)
extern "C" {
#endif

const char *cs_adder1_linear_model_frame()
{
  return "model linear_adder1\n  parameter Integer n = 0; // states\n  parameter Integer k = 2; // top-level inputs\n  parameter Integer l = 1; // top-level outputs\n"
  "  parameter Real x0[0] = {%s};\n"
  "  parameter Real u0[2] = {%s};\n"
  "  parameter Real A[0,0] = zeros(0,0);%s\n"
  "  parameter Real B[0,2] = zeros(0,2);%s\n"
  "  parameter Real C[1,0] = zeros(1,0);%s\n"
  "  parameter Real D[1,2] = [%s];\n"
  "  Real x[0];\n"
  "  input Real u[2](start= u0);\n"
  "  output Real y[1];\n"
  "\n    Real 'u_x1' = u[1];\n  Real 'u_x2' = u[2];\n  Real 'y_y' = y[1];\n\n"
  "equation\n  der(x) = A * x + B * u;\n  y = C * x + D * u;\nend linear_adder1;\n";
}
const char *cs_adder1_linear_model_datarecovery_frame()
{
  return "model linear_adder1\n  parameter Integer n = 0; // states\n  parameter Integer k = 2; // top-level inputs\n  parameter Integer l = 1; // top-level outputs\n  parameter Integer nz = 3; // data recovery variables\n"
  "  parameter Real x0[0] = {%s};\n"
  "  parameter Real u0[2] = {%s};\n"
  "  parameter Real z0[3] = {%s};\n"
  "  parameter Real A[0,0] = zeros(0,0);%s\n"
  "  parameter Real B[0,2] = zeros(0,2);%s\n"
  "  parameter Real C[1,0] = zeros(1,0);%s\n"
  "  parameter Real D[1,2] = [%s];\n"
  "  parameter Real Cz[3,0] = zeros(3,0);%s\n"
  "  parameter Real Dz[3,2] = [%s];\n"
  "  Real x[0];\n"
  "  input Real u[2](start= u0);\n"
  "  output Real y[1];\n"
  "  output Real z[3];\n"
  "\n  Real 'u_x1' = u[1];\n  Real 'u_x2' = u[2];\n  Real 'y_y' = y[1];\n  Real 'z_x1' = z[1];\n  Real 'z_x2' = z[2];\n  Real 'z_y' = z[3];\n\n"
  "equation\n  der(x) = A * x + B * u;\n  y = C * x + D * u;\n  z = Cz * x + Dz * u;\nend linear_adder1;\n";
}
#if defined(__cplusplus)
}
#endif

