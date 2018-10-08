.. index:: SSP

SSP Support
===========

Composite models are imported and exported in the *System Structure Description (SSD)* format, which is part of the *System Structure and Parameterization (SSP)* standard.

.. index:: SSP; Bus connections

Bus Connections
###############

Bus connections are saved as annotations to the SSD file. Bus connectors are only allowed in weakly coupled and strongly coupled systems. Bus connections can exist in any system type. Bus connectors are used to hide SSD connectors and bus connections are used to hide existing SSD connections in the graphical user interface. It is not required that all connectors referenced in a bus are connected. One bus may be connected to multiple other buses, and also to SSD connectors.

The example below contains a root system with two subsystems, ``WC1`` and ``WC2``. Bus connector ``WC1.bus1`` is connected to ``WC2.bus2``. Bus connector ``WC2.bus2`` is also connected to SSD connector ``WC1.C3``.

.. code-block:: xml
  :emphasize-lines: 10-19,27-36,47-56

  <?xml version="1.0" encoding="UTF-8"?>
  <ssd:SystemStructureDescription name="Test" version="Draft20180219">
    <ssd:System name="Root">
      <ssd:Elements>
        <ssd:System name="WC2">
          <ssd:Connectors>
            <ssd:Connector name="C1" kind="input" type="Real"/>
            <ssd:Connector name="C2" kind="output" type="Real"/>
          </ssd:Connectors>
          <ssd:Annotations>
            <ssc:Annotation type="org.openmodelica">
              <oms:Bus name="bus2">
                <oms:Signals>
                  <oms:Signal name="C1"/>
                  <oms:Signal name="C2"/>
                </oms:Signals>
              </oms:Bus>
            </ssc:Annotation>
          </ssd:Annotations>
        </ssd:System>
        <ssd:System name="WC1">
          <ssd:Connectors>
            <ssd:Connector name="C1" kind="output" type="Real"/>
            <ssd:Connector name="C2" kind="input" type="Real"/>
            <ssd:Connector name="C3" kind="input" type="Real"/>
          </ssd:Connectors>
          <ssd:Annotations>
            <ssc:Annotation type="org.openmodelica">
              <oms:Bus name="bus1">
                <oms:Signals>
                  <oms:Signal name="C1"/>
                  <oms:Signal name="C2"/>
                </oms:Signals>
              </oms:Bus>
            </ssc:Annotation>
          </ssd:Annotations>
        </ssd:System>
      </ssd:Elements>
      <ssd:Connections>
        <ssd:Connection startElement="WC2" startConnector="C1"
                        endElement="WC1" endConnector="C1"/>
        <ssd:Connection startElement="WC2" startConnector="C2"
                        endElement="WC1" endConnector="C2"/>
        <ssd:Connection startElement="WC2" startConnector="C2"
                        endElement="WC1" endConnector="C3"/>
      </ssd:Connections>
      <ssd:Annotations>
        <ssc:Annotation type="org.openmodelica">
          <oms:Connections>
            <oms:Connection startElement="WC1" startConnector="bus1"
                            endElement="WC2" endConnector="bus2"/>
            <oms:Connection startElement="WC2" startConnector="bus2"
                            endElement="WC1" endConnector="C3"/>
          </oms:Connections>
        </ssc:Annotation>
      </ssd:Annotations>
    </ssd:System>
  </ssd:SystemStructureDescription>

.. index:: SSP; TLM Systems

TLM Systems
###########

TLM systems are only allowed on top-level. SSD annotations are used to specify the system type inside the ``ssd:SimulationInformation`` tag, as shown in the example below. Attributes ``ip``, ``managerport`` and ``monitorport`` defines the socket communication, used both to exchange data with external tools and with internal simulation threads.

.. code-block:: xml
  :emphasize-lines: 4-8

  <?xml version="1.0"?>
  <ssd:System name="tlm">
    <ssd:SimulationInformation>
     <ssd:Annotations>
        <ssd:Annotation type="org.openmodelica">
          <oms:TlmMaster ip="127.0.1.1" managerport="11111" monitorport="11121"/>
        </ssd:Annotation>
     </ssd:Annotations>
    </ssd:SimulationInformation>
    <ssd:Elements>
      <ssd:System name="weaklycoupled">
        <ssd:SimulationInformation>
          <ssd:FixedStepMaster description="oms-ma" stepSize="1e-1" />
        </ssd:SimulationInformation>
      </ssd:System>
    </ssd:Elements>
  </ssd:System>


.. index:: SSP; TLM connections

TLM Connections
###############

TLM connections are implemented without regular SSD connections. TLM connections are only allowed in TLM systems. TLM connectors are only allowed in weakly coupled or strongly coupled systems. Both connectors and connections are implemented as SSD annotations in the System tag.

The example below shows a TLM system containing two weakly coupled systems, ``wc1`` and ``wc2``. System ``wc1`` contains two TLM connectors, one of type 1D signal and one of type 1D mechanical. System ``wc2`` contains only a 1D signal type connector. The two 1D signal connectors are connected to each other in the TLM top-level system.

.. code-block:: xml
  :emphasize-lines: 8-17,26-43,46-56

  <?xml version="1.0"?>
  <ssd:System name="tlm">
    <ssd:Elements>
      <ssd:System name="wc2">
        <ssd:Connectors>
          <ssd:Connector name="y" kind="input" type="Real" />
        </ssd:Connectors>
        <ssd:Annotations>
          <ssd:Annotation type="org.openmodelica">
            <oms:Bus name="bus2" type="tlm" domain="signal"
                     dimension="1" interpolation="none">
              <oms:Signals>
                <oms:Signal name="y" tlmType="value" />
              </oms:Signals>
            </oms:Bus>
          </ssd:Annotation>
        </ssd:Annotations>
      </ssd:System>
      <ssd:System name="wc1">
        <ssd:Connectors>
          <ssd:Connector name="y" kind="output" type="Real" />
          <ssd:Connector name="x" kind="output" type="Real" />
          <ssd:Connector name="v" kind="output" type="Real" />
          <ssd:Connector name="f" kind="input" type="Real" />
        </ssd:Connectors>
        <ssd:Annotations>
          <ssd:Annotation type="org.openmodelica">
            <oms:Bus name="bus1" type="tlm" domain="signal"
                     dimension="1" interpolation="none">
              <oms:Signals>
                <oms:Signal name="y" tlmType="value" />
              </oms:Signals>
            </oms:Bus>
            <oms:Bus name="bus2" type="tlm" domain="mechanical"
                     dimension="1" interpolation="none">
              <oms:Signals>
                <oms:Signal name="x" tlmType="state" />
                <oms:Signal name="v" tlmType="flow" />
                <oms:Signal name="f" tlmType="effort" />
              </oms:Signals>
            </oms:Bus>
          </ssd:Annotation>
        </ssd:Annotations>
      </ssd:System>
    </ssd:Elements>
    <ssd:Annotations>
      <ssd:Annotation type="org.openmodelica">
        <oms:Connections>
          <oms:Connection startElement="wc1" startConnector="bus1"
                          endElement="wc2" endConnector="bus2"
                          delay="0.001000" alpha="0.300000"
                          linearimpedance="100.000000"
                          angularimpedance="0.000000" />
        </oms:Connections>
      </ssd:Annotation>
    </ssd:Annotations>
  </ssd:System>


.. index::SSP; TLM Connector Types

Depending on the type of TLM bus connector (dimension, domain and interpolation), connectors need to be assigned to different tlm variable types. Below is the complete list of supported TLM bus types and their respective connectors.

**1D signal**

=============  ==============
  tlmType       causality  
=============  ==============
 ``"value"``    input/output
=============  ==============

**1D physical (no interpolation)**

==============  ==============
  tlmType         causality 
==============  ==============
 ``"state"``        output
 ``"flow"``         output
 ``"effort"``       input
==============  ==============

**1D physical (coarse-grained interpolation)**

================  ==============
  tlmType         causality 
================  ==============
 ``"state"``        output
 ``"flow"``         output
 ``"wave"``         input
 ``"impedance"``    input
================  ==============

**1D physical (fine-grained interpolation)**

================  ==============
  tlmType         causality 
================  ==============
 ``"state"``        output
 ``"flow"``         output
 ``"wave1"``        input
 ``"wave2"``        input
 ``"wave3"``        input
 ``"wave4"``        input
 ``"wave5"``        input
 ``"wave6"``        input
 ``"wave7"``        input
 ``"wave8"``        input
 ``"wave9"``        input
 ``"wave10"``       input
 ``"time1"``        input
 ``"time2"``        input
 ``"time3"``        input
 ``"time4"``        input
 ``"time5"``        input
 ``"time6"``        input
 ``"time7"``        input
 ``"time8"``        input
 ``"time9"``        input
 ``"time10"``       input
 ``"impedance"``    input
================  ==============

**3D physical (no interpolation)**

==================  ==============
  tlmType             causality 
==================  ==============
``"state1"``          output
``"state2"``          output
``"state3"``          output
``"A11"``             output
``"A12"``             output
``"A13"``             output
``"A21"``             output
``"A22"``             output
``"A23"``             output
``"A31"``             output
``"A32"``             output
``"A33"``             output
``"flow1"``           output
``"flow2"``           output
``"flow3"``           output
``"flow4"``           output
``"flow5"``           output
``"flow6"``           output
``"effort1"``         input
``"effort2"``         input
``"effort3"``         input
``"effort4"``         input
``"effort5"``         input
``"effort6"``         input
==================  ==============

 


**3D physical (coarse-grained interpolation)**

======================  ==============
  tlmType                causality 
======================  ==============
``"state1"``              output
``"state2"``              output
``"state3"``              output
``"A11"``                 output
``"A12"``                 output
``"A13"``                 output
``"A21"``                 output
``"A22"``                 output
``"A23"``                 output
``"A31"``                 output
``"A32"``                 output
``"A33"``                 output
``"flow1"``               output
``"flow2"``               output
``"flow3"``               output
``"flow4"``               output
``"flow5"``               output
``"flow6"``               output
``"wave1"``               input
``"wave2"``               input
``"wave3"``               input
``"wave4"``               input
``"wave5"``               input
``"wave6"``               input
``"linearimpedance"``     input
``"angularimpedance"``    input
======================  ==============

**3D physical (fine-grained interpolation)**

======================  ==============
  tlmType                causality 
======================  ==============
``"state1"``              output
``"state2"``              output
``"state3"``              output
``"A11"``                 output
``"A12"``                 output
``"A13"``                 output
``"A21"``                 output
``"A22"``                 output
``"A23"``                 output
``"A31"``                 output
``"A32"``                 output
``"A33"``                 output
``"flow1"``               output
``"flow2"``               output
``"flow3"``               output
``"flow4"``               output
``"flow5"``               output
``"flow6"``               output
``"wave1_1"``             input
``"wave1_2"``             input
``"wave1_3"``             input
``"wave1_4"``             input
``"wave1_5"``             input
``"wave1_6"``             input
``"wave2_1"``             input
``"wave2_2"``             input
``"wave2_3"``             input
``"wave2_4"``             input
``"wave2_5"``             input
``"wave2_6"``             input
``"wave3_1"``             input
``"wave3_2"``             input
``"wave3_3"``             input
``"wave3_4"``             input
``"wave3_5"``             input
``"wave3_6"``             input
``"wave4_1"``             input
``"wave4_2"``             input
``"wave4_3"``             input
``"wave4_4"``             input
``"wave4_5"``             input
``"wave4_6"``             input
``"wave5_1"``             input
``"wave5_2"``             input
``"wave5_3"``             input
``"wave5_4"``             input
``"wave5_5"``             input
``"wave5_6"``             input
``"wave6_1"``             input
``"wave6_2"``             input
``"wave6_3"``             input
``"wave6_4"``             input
``"wave6_5"``             input
``"wave6_6"``             input
``"wave7_1"``             input
``"wave7_2"``             input
``"wave7_3"``             input
``"wave7_4"``             input
``"wave7_5"``             input
``"wave7_6"``             input
``"wave8_1"``             input
``"wave8_2"``             input
``"wave8_3"``             input
``"wave8_4"``             input
``"wave8_5"``             input
``"wave8_6"``             input
``"wave9_1"``             input
``"wave9_2"``             input
``"wave9_3"``             input
``"wave9_4"``             input
``"wave9_5"``             input
``"wave9_6"``             input
``"wave10_1"``            input
``"wave10_2"``            input
``"wave10_3"``            input
``"wave10_4"``            input
``"wave10_5"``            input
``"wave10_6"``            input
``"time1"``               input
``"time2"``               input
``"time3"``               input
``"time4"``               input
``"time5"``               input
``"time6"``               input
``"time7"``               input
``"time8"``               input
``"time9"``               input
``"time10"``              input
``"linearimpedance"``     input
``"angularimpedance"``    input
======================  ==============

