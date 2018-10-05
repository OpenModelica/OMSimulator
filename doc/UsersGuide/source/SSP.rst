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
