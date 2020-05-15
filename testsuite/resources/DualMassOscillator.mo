package DualMassOscillator
  model System1
    parameter Real x1_start = 0.0;

    parameter Real c1 = 0.2 "damping constant";
    parameter Real k1 = 10.0 "spring constant";
    parameter Real m1 = 1.0;

    Modelica.Mechanics.Translational.Components.Fixed fixed(s0=0) annotation(
      Placement(visible = true, transformation(origin = {-70, 0}, extent = {{10, -10}, {-10, 10}}, rotation = -90)));
    Modelica.Mechanics.Translational.Components.Mass mass1(m=m1, s(fixed=true, start=x1_start), v(fixed=true, start=0), L=0) annotation(
      Placement(transformation(extent={{-14, -10}, {6, 10}})));
    Modelica.Mechanics.Translational.Components.SpringDamper sd1(c=k1, d=c1, s_rel0=0) annotation(
      Placement(transformation(extent={{-50, -10}, {-30, 10}})));
    Modelica.Mechanics.Translational.Sensors.PositionSensor positionSensor annotation(
      Placement(transformation(extent={{20, -40}, {40, -20}})));
    Modelica.Mechanics.Translational.Sensors.SpeedSensor speedSensor annotation(
      Placement(transformation(extent={{20, -60}, {40, -40}})));
    Modelica.Mechanics.Translational.Sensors.AccSensor accSensor annotation(
      Placement(transformation(extent={{20, -80}, {40, -60}})));
    Modelica.Mechanics.Translational.Sources.Force force annotation(
      Placement(transformation(extent={{40, -10}, {20, 10}})));
    Modelica.Blocks.Interfaces.RealInput F annotation(
      Placement(transformation(extent={{120, -10}, {100, 10}}), iconTransformation(extent={{120, -10}, {100, 10}})));
    Modelica.Blocks.Interfaces.RealOutput x1 annotation(
      Placement(transformation(extent={{100, -40}, {120, -20}}), iconTransformation(extent={{100, -40}, {120, -20}})));
    Modelica.Blocks.Interfaces.RealOutput v1 annotation(
      Placement(transformation(extent={{100, -60}, {120, -40}}), iconTransformation(extent={{100, -60}, {120, -40}})));
    Modelica.Blocks.Interfaces.RealOutput a1 annotation(
      Placement(transformation(extent={{100, -80}, {120, -60}}), iconTransformation(extent={{100, -80}, {120, -60}})));
  equation
    connect(fixed.flange, sd1.flange_a) annotation(
      Line(points={{-70, 0},  {-50, 0}}, color={0, 127, 0}));
    connect(sd1.flange_b, mass1.flange_a) annotation(
      Line(points={{-30, 0}, {-22, 0}, {-14, 0}}, color={0, 127, 0}));
    connect(mass1.flange_b, positionSensor.flange) annotation(
      Line(points={{6, 0}, {12, 0}, {12, -30}, {20, -30}}, color={0, 127, 0}));
    connect(mass1.flange_b, speedSensor.flange) annotation(
      Line(points={{6, 0}, {12, 0}, {12, -50}, {20, -50}}, color={0, 127, 0}));
    connect(mass1.flange_b, accSensor.flange) annotation(
      Line(points={{6, 0}, {12, 0}, {12, -70}, {20, -70}}, color={0, 127, 0}));
    connect(force.flange, mass1.flange_b) annotation(
      Line(points={{20, 0}, {6, 0}}, color={0, 127, 0}));
    connect(accSensor.a, a1) annotation(
      Line(points={{41, -70}, {110, -70}}, color={0, 0, 127}));
    connect(speedSensor.v, v1) annotation(
      Line(points={{41, -50}, {110, -50}}, color={0, 0, 127}));
    connect(positionSensor.s, x1) annotation(
      Line(points={{41, -30}, {110, -30}}, color={0, 0, 127}));
    connect(force.f, F) annotation(
      Line(points={{42, 0}, {110, 0}}, color={0, 0, 127}));
  annotation(
    Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(coordinateSystem(preserveAspectRatio=false)));
  end System1;

  model System2
    parameter Real x2_start = 0.5;

    parameter Real c2 = 0.5 "damping constant";
    parameter Real c3 = 0.3 "damping constant";
    parameter Real k2 = 10.0 "spring constant";
    parameter Real k3 = 20.0 "spring constant";
    parameter Real m2 = 1.0;

    Real x2 = mass2.s;
    Real v2 = mass2.v;
    Real a2 = mass2.a;

    Modelica.Mechanics.Translational.Components.Fixed fixed(s0=0) annotation(
      Placement(visible = true, transformation(origin = {90, -20},extent = {{-10, -10}, {10, 10}}, rotation = 90)));
    Modelica.Mechanics.Translational.Components.Mass mass2(m=m2, s(fixed=true, start=x2_start), v(fixed=true, start=0), L=0) annotation(
      Placement(visible = true, transformation(extent = {{20, -30}, {40, -10}}, rotation = 0)));
    Modelica.Mechanics.Translational.Components.SpringDamper sd2(c=k2, d=c2, s_rel0=0) annotation(
      Placement(visible = true, transformation(extent = {{-10, -30}, {10, -10}}, rotation = 0)));
    Modelica.Mechanics.Translational.Components.SpringDamper sd3(c=k3, d=c3, s_rel0=0) annotation(
      Placement(visible = true, transformation(extent = {{50, -30}, {70, -10}}, rotation = 0)));
    Modelica.Blocks.Interfaces.RealOutput F annotation(
      Placement(transformation(extent={{-100, -10}, {-120, 10}})));
    Modelica.Blocks.Interfaces.RealInput x1 annotation(
      Placement(transformation(extent={{-120, -40}, {-100, -20}})));
    Modelica.Blocks.Interfaces.RealInput v1 annotation(
      Placement(transformation(extent={{-120, -60}, {-100, -40}})));
    Modelica.Blocks.Interfaces.RealInput a1 annotation(
      Placement(transformation(extent={{-120, -80}, {-100, -60}})));
    Modelica.Mechanics.Translational.Sensors.ForceSensor forceSensor annotation(
      Placement(visible = true, transformation(extent = {{-20, -10}, {-40, -30}}, rotation = 0)));
    Modelica.Mechanics.Translational.Sources.Move move annotation(
      Placement(visible = true, transformation(extent = {{-68, -30}, {-48, -10}}, rotation = 0)));
  equation
    connect(forceSensor.f, F) annotation(
      Line(points = {{-22, -9}, {-22, 0}, {-110, 0}}, color = {0, 0, 127}));
    connect(x1, move.u[1]) annotation(
      Line(points = {{-110, -30}, {-80, -30}, {-80, -20}, {-70, -20}}, color = {0, 0, 127}));
    connect(v1, move.u[2]) annotation(
      Line(points = {{-110, -50}, {-80, -50}, {-80, -20}, {-70, -20}}, color = {0, 0, 127}));
    connect(a1, move.u[3]) annotation(
      Line(points = {{-110, -70}, {-80, -70}, {-80, -20}, {-70, -20}}, color = {0, 0, 127}));
    connect(sd2.flange_b, mass2.flange_a) annotation(
      Line(points = {{10, -20}, {20, -20}}, color = {0, 127, 0}));
    connect(mass2.flange_b, sd3.flange_a) annotation(
      Line(points = {{40, -20}, {50, -20}}, color = {0, 127, 0}));
    connect(sd3.flange_b, fixed.flange) annotation(
      Line(points = {{70, -20}, {90, -20}}, color = {0, 127, 0}));
    connect(forceSensor.flange_a, sd2.flange_a) annotation(
      Line(points = {{-20, -20}, {-10, -20}}, color = {0, 127, 0}));
    connect(move.flange, forceSensor.flange_b) annotation(
      Line(points = {{-48, -20}, {-40, -20}}, color = {0, 127, 0}));
  annotation(
    Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(coordinateSystem(preserveAspectRatio=false)));
  end System2;

  model System1Eq
    parameter Real x1_start = 0.0;

    parameter Real c1 = 0.2 "damping constant";
    parameter Real k1 = 10.0 "spring constant";
    parameter Real m1 = 1.0;

    Real x(fixed=true, start=x1_start);
    Real v(fixed=true, start=0) = der(x);

    Modelica.Blocks.Interfaces.RealInput F annotation(
      Placement(transformation(extent={{120, -10}, {100, 10}}), iconTransformation(extent={{120, -10}, {100, 10}})));
    Modelica.Blocks.Interfaces.RealOutput x1 = x annotation(
      Placement(transformation(extent={{100, -40}, {120, -20}}), iconTransformation(extent={{100, -40}, {120, -20}})));
    Modelica.Blocks.Interfaces.RealOutput v1 = v annotation(
      Placement(transformation(extent={{100, -60}, {120, -40}}), iconTransformation(extent={{100, -60}, {120, -40}})));
  equation
    m1*der(v) = -k1*x - c1*v + F;
  annotation(
    Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(coordinateSystem(preserveAspectRatio=false)));
  end System1Eq;

  model System2Eq
    parameter Real x2_start = 0.5;

    parameter Real c2 = 0.5 "damping constant";
    parameter Real c3 = 0.3 "damping constant";
    parameter Real k2 = 10.0 "spring constant";
    parameter Real k3 = 20.0 "spring constant";
    parameter Real m2 = 1.0;

    Real x2(fixed=true, start=x2_start);
    Real v2(fixed=true, start=0.0) = der(x2);

    Modelica.Blocks.Interfaces.RealOutput F = -k2*x1 - c2*v1 + k2*x2 + c2*v2 annotation(
      Placement(transformation(extent={{-100, -10}, {-120, 10}})));
    Modelica.Blocks.Interfaces.RealInput x1 annotation(
      Placement(transformation(extent={{-120, -40}, {-100, -20}})));
    Modelica.Blocks.Interfaces.RealInput v1 annotation(
      Placement(transformation(extent={{-120, -60}, {-100, -40}})));
  equation
    m2*der(v2) = -k3*x2 - c3*v2 - F;
  annotation(
    Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(coordinateSystem(preserveAspectRatio=false)));
  end System2Eq;

  model ReferenceSystem
    extends Modelica.Icons.Example;
    parameter Real x1_start = 0.0;
    parameter Real x2_start = 0.5;

    parameter Real c1 = 0.2 "damping constant";
    parameter Real c2 = 0.5 "damping constant";
    parameter Real c3 = 0.3 "damping constant";
    parameter Real k1 = 10.0 "spring constant";
    parameter Real k2 = 10.0 "spring constant";
    parameter Real k3 = 20.0 "spring constant";
    parameter Real m1 = 1.0;
    parameter Real m2 = 1.0;

    Real x1 = mass1.s;
    Real v1 = mass1.v;
    Real a1 = mass1.a;
    Real x2 = mass2.s;
    Real v2 = mass2.v;
    Real a2 = mass2.a;
    Real F = -k2*x1 - c2*v1 + k2*x2 + c2*v2;

    Real E = Ek + Ep;
    Real Ek = 1/2*m1*v1^2 + 1/2*m2*v2^2;
    Real Ep = 1/2*k1*x1^2 + 1/2*k2*(x2-x1)^2 + 1/2*k3*(-x2)^2;

    Modelica.Mechanics.Translational.Components.Fixed fixed(s0=0) annotation(
      Placement(transformation(extent={{-10,10}, {10,-10}}, rotation=90,origin={-86,0})));
    Modelica.Mechanics.Translational.Components.Mass mass1(m=m1, v(fixed=true, start=0), L=0, s(fixed=true, start=x1_start)) annotation(
      Placement(transformation(extent={{-40,-10}, {-20,10}})));
    Modelica.Mechanics.Translational.Components.SpringDamper sd1(c=k1, d=c1, s_rel0= 0) annotation(
      Placement(transformation(extent={{-70,-10}, {-50,10}})));
    Modelica.Mechanics.Translational.Components.Fixed fixed1(s0=0) annotation(
      Placement(transformation(extent={{-10,-10}, {10,10}}, rotation=90, origin={86,0})));
    Modelica.Mechanics.Translational.Components.Mass mass2(m=m2, v(fixed=true, start=0), L=0, s(fixed=true, start=x2_start)) annotation(
      Placement(transformation(extent={{20,-10}, {40,10}})));
    Modelica.Mechanics.Translational.Components.SpringDamper sd3(c=k3, d=c3, s_rel0=0) annotation(
      Placement(transformation(extent={{50,-10}, {70,10}})));
    Modelica.Mechanics.Translational.Components.SpringDamper sd2(c=k2, d=c2, s_rel0=0) annotation(
      Placement(transformation(extent={{-10,-10}, {10,10}})));
  equation
    connect(fixed.flange, sd1.flange_a) annotation(
      Line(points = {{-86, 0}, {-86, 0}, {-70, 0}}, color = {0, 127, 0}));
    connect(sd1.flange_b, mass1.flange_a) annotation(
      Line(points = {{-50, 0}, {-40, 0}}, color = {0, 127, 0}));
    connect(sd2.flange_b, mass2.flange_a) annotation(
      Line(points = {{10, 0}, {10, 0}, {20, 0}}, color = {0, 127, 0}));
    connect(mass2.flange_b, sd3.flange_a) annotation(
      Line(points = {{40, 0}, {40, 0}, {50, 0}}, color = {0, 127, 0}));
    connect(sd3.flange_b, fixed1.flange) annotation(
      Line(points = {{70, 0}, {70, 0}, {86, 0}}, color = {0, 127, 0}));
    connect(mass1.flange_b, sd2.flange_a) annotation(
      Line(points = {{-20, 0}, {-20, 0}, {-10, 0}}, color = {0, 127, 0}));
  annotation(
    Icon(coordinateSystem(preserveAspectRatio=false, extent={{-100, -100}, {100, 100}})), Diagram(coordinateSystem(preserveAspectRatio=false, extent={{-100, -20}, {100, 20}})));
  end ReferenceSystem;

  model ReferenceSystemEq
    extends Modelica.Icons.Example;
    parameter Real x1_start = 0.0;
    parameter Real x2_start = 0.5;

    parameter Real c1 = 0.2 "damping constant";
    parameter Real c2 = 0.5 "damping constant";
    parameter Real c3 = 0.3 "damping constant";
    parameter Real k1 = 10.0 "spring constant";
    parameter Real k2 = 10.0 "spring constant";
    parameter Real k3 = 20.0 "spring constant";
    parameter Real m1 = 1.0;
    parameter Real m2 = 1.0;

    Real x1(start=x1_start, fixed=true);
    Real v1(start=0.0, fixed=true) = der(x1);
    Real a1 = der(v1);
    Real x2(start=x2_start, fixed=true);
    Real v2(start=0.0, fixed=true) = der(x2);
    Real a2 = der(v2);
    Real F = -k2*x1 - c2*v1 + k2*x2 + c2*v2;

    Real E = Ek + Ep;
    Real Ek = 1/2*m1*v1^2 + 1/2*m2*v2^2;
    Real Ep = 1/2*k1*x1^2 + 1/2*k2*(x2-x1)^2 + 1/2*k3*(-x2)^2;
  equation
    m1*a1 + (c1+c2)*v1 - c2*v2 + (k1+k2)*x1 - k2*x2 = 0.0;
    m2*a2 - c2*v1 + (c2+c3)*v2 - k2*x1 + (k2+k3)*x2 = 0.0;
  end ReferenceSystemEq;

  model CoupledSystem
    extends Modelica.Icons.Example;
    DualMassOscillator.System1 system1 annotation(
      Placement(visible = true, transformation(origin = {-30, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    DualMassOscillator.System2 system2 annotation(
      Placement(visible = true, transformation(origin = {30, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));

    Real E = Ek + Ep;
    Real Ek = 1/2*system1.m1*system1.v1^2 + 1/2*system2.m2*system2.v2^2;
    Real Ep = 1/2*system1.k1*system1.x1^2 + 1/2*system2.k2*(system2.x2-system1.x1)^2 + 1/2*system2.k3*(-system2.x2)^2;
  equation
    connect(system2.F, system1.F) annotation(
      Line(points = {{18, 0}, {-18, 0}, {-18, 0}, {-18, 0}}, color = {0, 0, 127}));
    connect(system1.x1, system2.x1) annotation(
      Line(points = {{-18, -4}, {18, -4}, {18, -4}, {18, -4}}, color = {0, 0, 127}));
    connect(system1.v1, system2.v1) annotation(
      Line(points = {{-18, -6}, {18, -6}, {18, -6}, {18, -6}}, color = {0, 0, 127}));
    connect(system1.a1, system2.a1) annotation(
      Line(points = {{-18, -8}, {18, -8}, {18, -8}, {18, -8}}, color = {0, 0, 127}));
  end CoupledSystem;

  model CoupledSystemEq
    extends Modelica.Icons.Example;
    DualMassOscillator.System1Eq system1 annotation(
      Placement(visible = true, transformation(origin = {-30, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    DualMassOscillator.System2Eq system2 annotation(
      Placement(visible = true, transformation(origin = {30, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));

    Real E = Ek + Ep;
    Real Ek = 1/2*system1.m1*system1.v1^2 + 1/2*system2.m2*system2.v2^2;
    Real Ep = 1/2*system1.k1*system1.x1^2 + 1/2*system2.k2*(system2.x2-system1.x1)^2 + 1/2*system2.k3*(-system2.x2)^2;
  equation
    connect(system2.F, system1.F) annotation(
      Line(points = {{18, 0}, {-18, 0}, {-18, 0}, {-18, 0}}, color = {0, 0, 127}));
    connect(system1.x1, system2.x1) annotation(
      Line(points = {{-18, -4}, {18, -4}, {18, -4}, {18, -4}}, color = {0, 0, 127}));
    connect(system1.v1, system2.v1) annotation(
      Line(points = {{-18, -6}, {18, -6}, {18, -6}, {18, -6}}, color = {0, 0, 127}));
  end CoupledSystemEq;
  annotation(
    uses(Modelica(version="3.2.3")));
end DualMassOscillator;
