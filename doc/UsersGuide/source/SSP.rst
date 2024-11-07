.. index:: SSP
.. _omsimulator-ssp-support :

SSP Support
===========

Composite models are imported and exported in the *System Structure Description (SSD)* format, which is part of the *System Structure and Parameterization (SSP)* standard.

.. index:: SSP; Bus connections

Bus Connections
---------------

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
