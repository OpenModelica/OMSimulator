package equationPair
  model equation1
   Modelica.Blocks.Interfaces.RealInput x1;
   Modelica.Blocks.Interfaces.RealOutput x2;
  equation
    2*x1^2 + 5*x2 = 42;
    annotation (experiment(StopTime=0));
  end equation1;

  model equation2
   Modelica.Blocks.Interfaces.RealInput x2;
   Modelica.Blocks.Interfaces.RealOutput x1;
  equation
    x1-6*x2=4;
    annotation (experiment(StopTime=0));
  end equation2;

  annotation (uses(Modelica(version="4.0.0")));
end equationPair;
