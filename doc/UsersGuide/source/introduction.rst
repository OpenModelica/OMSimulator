Introduction
============

The OMSimulator project is a FMI-based co-simulation tool that supports ordinary (i.e., non-delayed) and TLM connections.
It supports large-scale simulation and virtual prototyping using models from multiple sources utilizing the FMI standard.
It is integrated into OpenModelica but also available stand-alone, i.e., without dependencies to Modelica specific
models or technology. OMSimulator provides an industrial-strength open-source FMI-based modelling
and simulation tool. Input/output ports of FMUs can be connected, ports can be grouped to buses, FMUs can be
parameterized and composed, and composite models can be exported according to the (preliminary) SSP (System
Structure and Parameterization) standard. Efficient FMI based simulation is provided for both model-exchange and
co-simulation. TLM-based tool connection is provided for a range of applications, e.g., Adams, Simulink, Beast,
Dymola, and OpenModelica. Moreover, optional TLM (Transmission Line Modelling) domain-specific connectors
are also supported, providing additional numerical stability to co-simulation. An external API is available
for use from other tools and scripting languages such as :ref:`Python <oms-python>` and :ref:`Lua <oms-lua>`.
