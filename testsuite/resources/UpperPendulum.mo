model UpperPendulum
  inner Modelica.Mechanics.MultiBody.World world(enableAnimation = false, animateWorld = false, animateGravity = false, n = {-1, 0, 0}) annotation(
    Placement(visible = true, transformation(origin = {-71.9212, 77.3399}, extent = {{-12, -12}, {12, 12}}, rotation = 0)));
  Modelica.Mechanics.MultiBody.Joints.Spherical spherical1 annotation(
    Placement(visible = true, transformation(origin = {-6.89655, 77.3399}, extent = {{-12, -12}, {12, 12}}, rotation = 0)));
  Modelica.Mechanics.MultiBody.Parts.BodyCylinder bodycylinder1(r_shape = {0, 0, 0}, r = {0, 0, 0.5}) annotation(
    Placement(visible = true, transformation(origin = {48, 78}, extent = {{-12, -12}, {12, 12}}, rotation = 0)));
  FMITLM.FMITLM_Interface_3D.FMITLMInterface3D fmi annotation(
    Placement(visible = true, transformation(origin = {88, 78}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
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
  connect(bodycylinder1.frame_b, fmi.frame_a) annotation(
    Line(points = {{60, 78}, {81, 78}}, color = {95, 95, 95}));
  connect(spherical1.frame_b, bodycylinder1.frame_a) annotation(
    Line(points = {{5.10345, 77.3399}, {31.0345, 77.3399}, {31.0345, 78}, {36, 78}}));
  connect(world.frame_b, spherical1.frame_a) annotation(
    Line(points = {{-59.9212, 77.3399}, {-45.8128, 77.3399}, {-45.8128, 77.3399}, {-18.8966, 77.3399}}));
end UpperPendulum;