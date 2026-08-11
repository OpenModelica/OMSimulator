package QuarterCarModel
  model QuarterCarModel
    Modelica.Mechanics.Translational.Components.Mass wheel( L=0.0,
      m=40,
      s(fixed=true, start=0.0),
      v(fixed=true, start=0.0)) annotation (
      Placement(transformation(extent={{10,-10},{30,10}})));
    Modelica.Mechanics.Translational.Components.Mass chassis(
      L=0.0,
      m=400,
      s(fixed=true, start=0.0),
      v(fixed=true, start=0.0))
      annotation (Placement(transformation(extent={{70,-10},{90,10}})));
    Modelica.Mechanics.Translational.Components.SpringDamper springDamper_chassis(
      s_rel0=0.0,
      c=15e3,
      d=1e3) annotation (Placement(transformation(extent={{40,-10},{60,10}})));
    Modelica.Mechanics.Translational.Components.Spring spring_wheel(c=150e3,
        s_rel0=0.0)
      annotation (Placement(transformation(extent={{-20,-10},{0,10}})));
    Modelica.Mechanics.Translational.Sources.Move ground
      annotation (Placement(transformation(extent={{-50,-10},{-30,10}})));
    Modelica.Blocks.Interfaces.RealInput road
      annotation (Placement(transformation(extent={{-140,-10},{-120,10}})));
    Modelica.Blocks.Sources.Constant road_v(k=0.0)
      annotation (Placement(transformation(extent={{-114,-16},{-104,-6}})));
    Modelica.Blocks.Routing.Multiplex3 multiplex3_1
      annotation (Placement(transformation(extent={{-80,-10},{-60,10}})));
    Modelica.Blocks.Sources.Constant road_a(k=0.0)
      annotation (Placement(transformation(extent={{-114,-36},{-104,-26}})));
  equation
    connect(springDamper_chassis.flange_b, chassis.flange_a)
      annotation (Line(points={{60,0},{60,0},{70,0}}, color={0,127,0}));
    connect(wheel.flange_b, springDamper_chassis.flange_a)
      annotation (Line(points={{30,0},{30,0},{40,0}}, color={0,127,0}));
    connect(spring_wheel.flange_b, wheel.flange_a)
      annotation (Line(points={{0,0},{4,0},{10,0}}, color={0,127,0}));
    connect(ground.flange, spring_wheel.flange_a)
      annotation (Line(points={{-30,0},{-25,0},{-20,0}}, color={0,127,0}));
    connect(road, multiplex3_1.u1[1]) annotation (Line(points={{-130,0},{-100,0},
            {-100,7},{-82,7}}, color={0,0,127}));
    connect(multiplex3_1.y, ground.u)
      annotation (Line(points={{-59,0},{-52,0}}, color={0,0,127}));
    connect(road_v.y, multiplex3_1.u2[1]) annotation (Line(points={{-103.5,-11},
            {-94,-11},{-94,0},{-82,0}}, color={0,0,127}));
    connect(road_a.y, multiplex3_1.u3[1]) annotation (Line(points={{-103.5,-31},
            {-90,-31},{-90,-7},{-82,-7}}, color={0,0,127}));
    annotation (Diagram(coordinateSystem(extent={{-120,-40},{100,40}})), Icon(
          coordinateSystem(extent={{-120,-40},{100,40}})));
  end QuarterCarModel;

  model ReferenceSystem
    Real z_c(fixed=true, start=0.0);
    Real der_z_c(fixed=true, start=0.0);
    Real z_w(fixed=true, start=0.0);
    Real der_z_w(fixed=true, start=0.0);
    Real z;
    parameter Real m_w = 40;
    parameter Real m_c = 400;
    parameter Real k_w = 150e3;
    parameter Real k_c = 15e3;
    parameter Real d_c = 1e3;
  equation
    z = if time > 1.0 then 0.1 else 0.0;
    der(z_c) = der_z_c;
    der(z_w) = der_z_w;
    m_c*der(der_z_c) = k_c*(z_w-z_c) + d_c*(der_z_w-der_z_c);
    m_w*der(der_z_w) = k_w*(z - z_w) - k_c*(z_w-z_c) - d_c*(der_z_w-der_z_c);

    annotation (Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(
        coordinateSystem(preserveAspectRatio=false)),
      experiment(StopTime=5));
  end ReferenceSystem;

  package DisplacementDisplacement
    model Chassis
      parameter Real m_c = 400;
      parameter Real k_c = 15e3;
      parameter Real d_c = 1e3;
      Modelica.Blocks.Interfaces.RealInput z_w
        annotation (Placement(transformation(extent={{-120,20},{-100,40}})));
      Modelica.Blocks.Interfaces.RealInput der_z_w
        annotation (Placement(transformation(extent={{-120,-40},{-100,-20}})));
      Modelica.Blocks.Interfaces.RealOutput z_c(fixed=true, start=0.0)
        annotation (Placement(transformation(extent={{100,20},{120,40}})));
      Modelica.Blocks.Interfaces.RealOutput der_z_c(fixed=true, start=0.0)
        annotation (Placement(transformation(extent={{100,-40},{120,-20}})));
    equation
      der(z_c) = der_z_c;
      m_c*der(der_z_c) = k_c*(z_w-z_c) + d_c*(der_z_w-der_z_c);

      annotation (Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(
            coordinateSystem(preserveAspectRatio=false)));
    end Chassis;

    model Wheel
      parameter Real m_w = 40;
      parameter Real k_w = 150e3;
      parameter Real k_c = 15e3;
      parameter Real d_c = 1e3;

      Modelica.Blocks.Interfaces.RealInput z_c
        annotation (Placement(transformation(extent={{-120,20},{-100,40}})));
      Modelica.Blocks.Interfaces.RealInput der_z_c
        annotation (Placement(transformation(extent={{-120,-40},{-100,-20}})));
      Modelica.Blocks.Interfaces.RealOutput z_w(fixed=true, start=0.0)
        annotation (Placement(transformation(extent={{100,20},{120,40}})));
      Modelica.Blocks.Interfaces.RealOutput der_z_w(fixed=true, start=0.0)
        annotation (Placement(transformation(extent={{100,-40},{120,-20}})));
      Modelica.Blocks.Interfaces.RealInput z
        annotation (Placement(transformation(extent={{-10,-10},{10,10}},
            rotation=90,
            origin={0,-110})));
    equation
      der(z_w) = der_z_w;
      m_w*der(der_z_w) = k_w*(z - z_w) - k_c*(z_w-z_c) - d_c*(der_z_w-der_z_c);
      annotation (Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(
            coordinateSystem(preserveAspectRatio=false)));
    end Wheel;

    model CompositeModel
      Chassis chassis
        annotation (Placement(transformation(extent={{20,-10},{40,10}})));
      Wheel wheel
        annotation (Placement(transformation(extent={{-40,-10},{-20,10}})));
      Modelica.Blocks.Sources.Step step(height=0.1, startTime=1)
        annotation (Placement(transformation(extent={{-80,-60},{-60,-40}})));
      Modelica.Blocks.Sources.Step step1(height=-0.05, startTime=4)
        annotation (Placement(transformation(extent={{-80,-90},{-60,-70}})));
      Modelica.Blocks.Math.Add add
        annotation (Placement(transformation(extent={{-40,-74},{-20,-54}})));
    equation
      connect(wheel.z_w, chassis.z_w) annotation (Line(points={{-19,3},{
              -0.5,3},{-0.5,3},{19,3}}, color={0,0,127}));
      connect(wheel.der_z_w, chassis.der_z_w) annotation (Line(points={{-19,
              -3},{-0.5,-3},{-0.5,-3},{19,-3}}, color={0,0,127}));
      connect(chassis.z_c, wheel.z_c) annotation (Line(points={{41,3},{52,3},
              {52,20},{-50,20},{-50,3},{-41,3}}, color={0,0,127}));
      connect(chassis.der_z_c, wheel.der_z_c) annotation (Line(points={{41,
              -3},{52,-3},{52,-20},{-50,-20},{-50,-3},{-41,-3}}, color={0,0,127}));
      connect(step.y, add.u1) annotation (Line(points={{-59,-50},{-52,-50},{-52,
              -58},{-42,-58}}, color={0,0,127}));
      connect(step1.y, add.u2) annotation (Line(points={{-59,-80},{-52,-80},{
              -52,-70},{-42,-70}}, color={0,0,127}));
      connect(add.y, wheel.z) annotation (Line(points={{-19,-64},{-10,-64},{-10,
              -56},{-10,-40},{-30,-40},{-30,-11}}, color={0,0,127}));
      annotation (Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(
            coordinateSystem(preserveAspectRatio=false)),
        experiment(StopTime=5));
    end CompositeModel;
  end DisplacementDisplacement;

  package DisplacementForce
    model Chassis
      parameter Real m_w = 40;
      parameter Real m_c = 400;
      parameter Real k_w = 150e3;
      parameter Real k_c = 15e3;
      parameter Real d_c = 1e3;

      Modelica.Blocks.Interfaces.RealInput F
        annotation (Placement(transformation(extent={{-120,-10},{-100,10}})));
      Modelica.Blocks.Interfaces.RealOutput z_c(fixed=true, start=0.0)
        annotation (Placement(transformation(extent={{100,20},{120,40}})));
      Modelica.Blocks.Interfaces.RealOutput der_z_c(fixed=true, start=0.0)
        annotation (Placement(transformation(extent={{100,-40},{120,-20}})));
    equation
      der(z_c) = der_z_c;
      m_c*der(der_z_c) = F;

      annotation (Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(
            coordinateSystem(preserveAspectRatio=false)));
    end Chassis;

    model Wheel
      parameter Real m_w = 40;
      parameter Real k_w = 150e3;
      parameter Real k_c = 15e3;
      parameter Real d_c = 1e3;

      Modelica.Blocks.Interfaces.RealInput z_c
        annotation (Placement(transformation(extent={{-120,20},{-100,40}})));
      Modelica.Blocks.Interfaces.RealInput der_z_c
        annotation (Placement(transformation(extent={{-120,-40},{-100,-20}})));
      Modelica.Blocks.Interfaces.RealOutput F
        annotation (Placement(transformation(extent={{100,-10},{120,10}})));
      Modelica.Blocks.Interfaces.RealInput z
        annotation (Placement(transformation(extent={{-10,-10},{10,10}},
            rotation=90,
            origin={0,-110})));

      Real z_w(fixed=true, start=0.0);
      Real der_z_w(fixed=true, start=0.0);
    equation
      der(z_w) = der_z_w;
      m_w*der(der_z_w) = k_w*(z - z_w) - F;
      F = k_c*(z_w-z_c) + d_c*(der_z_w-der_z_c);

      annotation (Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(
            coordinateSystem(preserveAspectRatio=false)));
    end Wheel;

    model CompositeModel
      Chassis chassis
        annotation (Placement(transformation(extent={{20,-10},{40,10}})));
      Wheel wheel
        annotation (Placement(transformation(extent={{-40,-10},{-20,10}})));
      Modelica.Blocks.Sources.Step step(height=0.1, startTime=1)
        annotation (Placement(transformation(extent={{-80,-60},{-60,-40}})));
      Modelica.Blocks.Sources.Step step1(height=-0.05, startTime=4)
        annotation (Placement(transformation(extent={{-80,-90},{-60,-70}})));
      Modelica.Blocks.Math.Add add
        annotation (Placement(transformation(extent={{-40,-74},{-20,-54}})));
    equation
      connect(wheel.F, chassis.F)
        annotation (Line(points={{-19,0},{0,0},{19,0}}, color={0,0,127}));
      connect(chassis.z_c, wheel.z_c) annotation (Line(points={{41,3},{50,3},
              {50,20},{-52,20},{-52,3},{-41,3}}, color={0,0,127}));
      connect(chassis.der_z_c, wheel.der_z_c) annotation (Line(points={{41,
              -3},{50,-3},{50,-20},{-52,-20},{-52,-3},{-41,-3}}, color={0,0,127}));
      connect(step.y, add.u1) annotation (Line(points={{-59,-50},{-52,-50},{-52,
              -58},{-42,-58}}, color={0,0,127}));
      connect(step1.y, add.u2) annotation (Line(points={{-59,-80},{-52,-80},{
              -52,-70},{-42,-70}}, color={0,0,127}));
      connect(add.y, wheel.z) annotation (Line(points={{-19,-64},{-10,-64},{-10,
              -40},{-30,-40},{-30,-11}}, color={0,0,127}));
      annotation (Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(
            coordinateSystem(preserveAspectRatio=false)),
        experiment(StopTime=5));
    end CompositeModel;
  end DisplacementForce;
  annotation (uses(Modelica(version="4.0.0")));
end QuarterCarModel;
