model LowerPendulum
  Modelica.Mechanics.MultiBody.Parts.BodyCylinder bodycylinder1(r_shape = {0, 0, 0}, r = {0, 0, 0.5}) annotation(
    Placement(visible = true, transformation(origin = {9.37115, 59.4328}, extent = {{-12, -12}, {12, 12}}, rotation = 0)));
  inner Modelica.Mechanics.MultiBody.World world( animateGravity = false, animateWorld = false,enableAnimation = false, n = {-1, 0, 0}) annotation(
    Placement(visible = true, transformation(origin = {-76.4488, 81.381}, extent = {{-12, -12}, {12, 12}}, rotation = 0)));
  FMITLM.FMITLM_Interface_3D.FMITLMInterface3D fmi annotation(
    Placement(visible = true, transformation(origin = {-32, 60}, extent = {{-10, -10}, {10, 10}}, rotation = 180)));
  output Real r[3];
  output Real v[3];
  output Real w[3];
  output Real A[3,3];
  input Real f[3];
  input Real t[3];
equation
  r = fmi.r;
  v = fmi.v;
  w = fmi.w;
  A = fmi.A;
  fmi.f = f;
  fmi.t = t;
  connect(fmi.frame_a, bodycylinder1.frame_a) annotation(
    Line(points = {{-25, 60}, {-2, 60}}, color = {95, 95, 95}));
end LowerPendulum;