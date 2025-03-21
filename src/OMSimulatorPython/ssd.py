import os

from lxml import etree as ET

from OMSimulator import namespace, utils
from OMSimulator.connection import Connection
from OMSimulator.connector import Connector
from OMSimulator.fmu import FMU
from OMSimulator.ssv import SSV
from OMSimulator.unit import UnitDefinitions
from OMSimulator.values import Values
from OMSimulator.variables import Variable


class SSD:
  # Only these attributes are allowed, to disable dynamic creation of attributes in python
  __slots__ = ["_name", "connectors", "components", "connections", "value", "parameterResources", "startTime", "stopTime"]

  def __init__(self, name: str):
    self._name = name
    self.connectors = []
    self.components = {}  # Dictionary to store multiple components objects by cref
    self.connections = [] # list to store all connection
    self.value = Values()
    self.parameterResources = {}
    self.startTime = 0.0
    self.stopTime = 1.0

  @staticmethod
  def importFromFile(filename: str):
    try:
      temp_dir = os.path.dirname(filename)

      tree = ET.parse(filename)
      root = tree.getroot()
      variant_name = root.get("name")
      ssd = SSD(variant_name)

      system = root.find('ssd:System', namespaces=namespace.ns)
      cref = system.get("name")

      ## parse connectors
      connectors = utils.parseConnectors(system)
      ssd.add(connectors)

      ## parse parameter Bindings
      utils.parseParameterBindings(system, ssd, temp_dir)

      ## parse element section
      fmus = utils.parseElements(system, temp_dir)
      ssd.add(fmus)

      ## parse connection section
      utils.parseConnection(system, ssd)

      ## parse default experiment
      utils.parseDefaultExperiment(root, ssd)

      return ssd

    except ET.ParseError as e:
      print(f"Error parsing SystemStructure.ssd: {e}")

  @property
  def name(self):
    return self._name

  def getConnectors(self):
     return self.connectors

  def getComponents(self, cref = None):
    if cref is None:
      return self.components
    else:
      return self.components[cref]

  def getParameters(self):
    return self.value.getAllParameters()

  def getParameterResources(self):
    return self.parameterResources

  def _addResources(self, resources):
    ## check in connectors if the variables exists and raise error
    for store in [resources.value.realStartValues, resources.value.integerStartValues, resources.value.booleanStartValues, resources.value.stringStartValues]:
      for key, value in store.items():
        var = self.checkVarExist(key)
        if not var:
          raise KeyError(f"Variable '{key}' does not exist in the variables list when adding SSV resources: {resources.filename}")
    self.parameterResources[resources.filename] = resources

  ## generic function to add different class types based on the class instances
  def add(self, element):
    # Ensure 'elements' is always a list also support different class instances
    if not isinstance(element, list):
      elements = [element]
    else:
      elements = element

    for item in elements:
      if isinstance(item, SSV):
        self._addResources(item)
      elif isinstance(item, Connector):
        self._addConnector(item)
      elif isinstance(item, FMU):
        self._addComponent(item)
      else:
        raise TypeError(f"SSD class does not support adding instance type of : {type(item)}")

  def _addConnector(self, connector):
    self.connectors.append(connector)

  def _addComponent(self, fmu):
    cref = fmu.getCref()
    if cref is None:
      raise ValueError("FMU cref cannot be None")
    self.components[cref] = fmu  # Store FMUs using their cref

  def addConnection(self, startElement : str, startConnector : str, endElement : str, endConnector : str):
    connection = Connection(startElement, startConnector, endElement, endConnector)
    self.connections.append(connection)

  def split_variable(self, variable: str):
    "Splits 'element.connector' at the last dot, ensuring exactly two parts."
    if '.' in variable:
      return variable.rsplit('.', 1)  # Splits at the last dot
    else:
      return "", variable  # No dot found, return (element, empty connector)

  def checkVarExist(self, cref: str):
    for var in self.connectors:
      if var.name == cref:
        return var
    return None

  def setValue(self, cref:str, value):

    # Check if the variable exists
    var = self.checkVarExist(cref)
    if not var:
      raise KeyError(f"Variable '{cref}' does not exist in the Connector list.")

    var = Variable(cref)
    var.startValue = value

    if isinstance(value, float):
      self.value.setReal(var)
    elif isinstance(value, int):
      self.value.setInteger(var)
    elif isinstance(value, bool):
      self.value.setBoolean(var)
    elif isinstance(value, str):
      self.value.setString(var)

  def list(self, prefix=""):
    print(f"{prefix}|--   System: {self.name}")
    print(f"{prefix}|--   Connectors:")
    last_prefix = prefix + "     |"  # This is the prefix for nested elements
    for connector in self.connectors:
      connector.list(prefix=last_prefix)

    ## list parameters inline
    if not self.value.empty():
      print(f"{prefix}|--   ParameterBindings:")
      print(f"{prefix}|     |-- inline:")
      last_prefix = prefix + "     |"  # This is the prefix for nested elements
      self.value.list(prefix= last_prefix)

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        print(f"{prefix}|--   ParameterBindings:")
        print(f"{prefix}|     |-- {resources.filename}:")
        last_prefix = prefix + "     |"  # This is the prefix for nested elements
        resources.list(prefix = last_prefix)

    ## list elements
    if len(self.components) > 0:
      print(f"{prefix}|--   Components:")
      last_prefix = prefix + "   "  # This is the prefix for nested elements
      for key, component in self.components.items():
        component.list(last_prefix)

    ## list connections
    if len(self.connections) > 0:
      print(f"{prefix}|--   Connections:")
      last_prefix = prefix + "     |"  # This is the prefix for nested elements
      for connection in self.connections:
        connection.list(prefix=last_prefix)
        pass

  def export(self, filename: str) -> None:
    node = self.exportToSSD()
    with open(filename, 'w', encoding='utf-8') as file:
      xml = ET.tostring(node, encoding='utf-8', xml_declaration=True, pretty_print=True).decode('utf-8')
      file.write(xml)

  def exportToSSD(self):
    node = ET.Element(
        namespace.tag("ssd", "SystemStructureDescription"),
        nsmap=namespace.ns,  # Define the namespace map correctly
        name=self.name,
        version="2.0"
    )

    ## export system
    system_node = ET.SubElement(node, namespace.tag("ssd", "System"), attrib={"name": self.name})
    if len(self.connectors) > 0 :
      connectors_node = ET.SubElement(system_node, namespace.tag("ssd", "Connectors"))
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)
      ## export parameter bindings
      self.value.exportToSSD(system_node)

      ## export parameters binding to ssd file with reference to ssv file
      if len(self.parameterResources) > 0:
        for key, resources in self.parameterResources.items():
          resources.exportToSSD(system_node)

    ## export elements
    if len(self.components) > 0:
      element_node = ET.SubElement(system_node, namespace.tag("ssd", "Elements"))
      for key, component in self.components.items():
        component.exportToSSD(key, element_node)

    ## export connections
    if len(self.connections) > 0:
      connections_node = ET.SubElement(system_node, namespace.tag("ssd", "Connections"))
      for connection in self.connections:
        connection.exportToSSD(connections_node)

    ## TODO export Annotations
    ## oms_annotation_node = self.exportAnnotation(system_node)

    self.exportUnitDefinitions(node)
    self.exportDefaultExperiment(node)
    return node

  def exportUnitDefinitions(self, node):
    if len(self.components) > 0:
      unitsToExport = {}
      for key, component in self.components.items():
        for item in component.unitDefinitions:
          if item.name not in unitsToExport:
            unitsToExport[item.name] = item.baseUnits

      if len(unitsToExport) > 0:
        units_node = ET.SubElement(node, namespace.tag("ssd", "Units"))
        unitDefinitions = UnitDefinitions()
        unitDefinitions.exportToSSD(unitsToExport, units_node)

  def exportDefaultExperiment(self, node):
    default_experiment = ET.SubElement(node, namespace.tag("ssd", "DefaultExperiment"))
    default_experiment.set("startTime", str(self.startTime))
    default_experiment.set("stopTime", str(self.stopTime))

  def exportParameterResources(self, resourceDir = None, printOutput = False):
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        ssv_node = ET.Element(namespace.tag("ssv", "ParameterSet"),
                                   nsmap={"ssc": "http://ssp-standard.org/SSP1/SystemStructureCommon",
                                          "ssv": "http://ssp-standard.org/SSP1/SystemStructureParameterValues"},
                                   version = "2.0",
                                   name = "parameters")
        resources.exportToSSV(ssv_node)
        xml = ET.tostring(ssv_node, encoding='utf-8', xml_declaration=True, pretty_print=True).decode('utf-8')
        if printOutput:
          print(xml)
        if resourceDir:
          ssvfilePath = os.path.join(resourceDir, key)
          with open(ssvfilePath, "w", encoding="utf-8") as file:
            file.write(xml)

    if len(self.components) > 0:
      for key, component in self.components.items():
        component.exportParameterResources(resourceDir, printOutput)

  def exportAnnotation(self, node):
    annotations_node = ET.SubElement(node, namespace.tag("ssd", "Annotations"))
    annotation_node = ET.SubElement(annotations_node, namespace.tag("ssc", "Annotation"))
    annotation_node.set("type", "org.openmodelica")
    oms_annotation_node = ET.SubElement(annotation_node, namespace.tag("oms", "Annotations"))
    return oms_annotation_node
