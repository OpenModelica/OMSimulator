import os
import random
import zipfile

from lxml import etree as ET

from OMSimulator import namespace
from OMSimulator.connector import Connector
from OMSimulator.ssv import SSV
from OMSimulator.unit import UnitDefinitions
from OMSimulator.values import Values
from OMSimulator.variables import Variable


class FMU:
  def __init__(self, fmupath : str, cref = None, ssp_zip=None):
    self.cref = cref
    self.fmupath = fmupath
    self.connectors = []
    self.variables = []
    self.stateVariables = []
    self.value = Values()
    self.fmuGuid = {}
    self.fmuResourcePath = {}
    self.unitDefinitions = []
    if ssp_zip:
      self.loadFromSSP(ssp_zip, fmupath)
    else:
      self.parseModelDescription()
    self.makeConnector()
    self.parameterResources = {}

  def getCref(self):
    return self.cref

  def getUnitDefinitions(self):
    return self.unitDefinitions

  def getParameterResources(self, cref=None):
    if cref:
      return self.parameterResources.get(cref)
    else:
      return self.parameterResources

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
      else:
        raise TypeError(f"Unknown instance type: {type(item)}")

  ## protected function
  def _addResources(self, resources):
    """Retrieve the ValueWithUnit object."""
    for store in [resources.value.realStartValues, resources.value.integerStartValues, resources.value.booleanStartValues, resources.value.stringStartValues]:
      for key, value in store.items():
        var = self.checkVarExist(key)
        if not var:
          raise KeyError(f"Variable '{key}' does not exist in the variables list when adding SSV resources: {resources.filename}")
        if var.unit:
          value.unit = var.unit
          value.unitDefinition = var.unitDefinition
    self.parameterResources[resources.filename] = resources

  def getFmuPath(self):
    return self.fmupath

  def getUniqueId(self):
    return str(f"{random.randint(1, 9999):04d}")

  def getFMUResourcePath(self, cref):
    return self.fmuResourcePath.get(cref)

  def getParameters(self):
    return self.value.getAllParameters()

  def getStateVariables(self):
    stateVars = []
    for var in self.stateVariables:
      stateVars.append({
          "name": var.name,
          "valueReference": var.valueReference,
          "variability": var.variablity,
          "causality": var.causality
      })
    return stateVars  # Returns a list of dictionaries

  def loadFromSSP(self, ssp_zip, fmu_path):
    """
    Extract and read necessary files from FMU inside an SSP archive.
    :param ssp_zip: Opened zipfile.ZipFile object.
    :param fmu_path: Path to FMU inside the ZIP.
    """
    with ssp_zip.open(fmu_path) as fmu_file:
      with zipfile.ZipFile(fmu_file) as fmu:
        file_list = fmu.namelist()
        self.fmupath = fmu_path
        if "modelDescription.xml" in fmu.namelist():
          with fmu.open("modelDescription.xml") as xml_file:
            self.parseModelDescriptionHelper(xml_file)
        else:
          print("Error: modelDescription.xml not found in FMU", fmu_path)

  def parseModelDescription(self):
    try:
      with zipfile.ZipFile(self.fmupath, 'r') as fmu:
        if "modelDescription.xml" in fmu.namelist():
          with fmu.open("modelDescription.xml") as xml_file:
            self.parseModelDescriptionHelper(xml_file)
        else:
          print("Error: modelDescription.xml not found in FMU.", self.fmupath)
    except zipfile.BadZipFile:
      print("Error: The provided file is not a valid ZIP archive.")
    except ET.ParseError:
      print("Error: Failed to parse modelDescription.xml.")
    except Exception as e:
      print(f"Unexpected error: {e}")

  def parseModelDescriptionHelper(self, xml_file : str):
    tree = ET.parse(xml_file)
    root = tree.getroot()
    guid = root.get("guid")
    # self.fmuGuid[self.cref] = guid
    # Parse UnitDefinitions
    self.parseUnitDefinitions(root)
    # Find the ModelVariables section
    model_variables = root.find("ModelVariables")
    if model_variables is not None:
      for scalar_var in model_variables.findall("ScalarVariable"):
        name = scalar_var.get("name")
        valueReference = scalar_var.get("valueReference")
        causality = scalar_var.get("causality", "local")
        variability = scalar_var.get("variability", "continuous")
        var_type = None
        unit = None
        start = None
        derivative_index = -1
        for child in scalar_var:
          var_type = child.tag
          if "unit" in child.attrib:
            unit = child.attrib["unit"]
          if "start" in child.attrib:
            start = child.attrib["start"]
          if "derivative" in child.attrib:
            derivative_index = int(child.attrib["derivative"])
            if variability == "continuous":
              self.stateVariables.append(self.variables[derivative_index-1])
          break
        variable = Variable(name, valueReference, causality, variability, var_type, unit, start)
        ## store the unit definitions of a variable if unit exist
        if unit:
          for unit in self.unitDefinitions:
            if unit.name == variable.unit:
              variable.setUnitDefinitions(unit)
        self.variables.append(variable)
    else:
      print("Error: ModelVariables section not found.")

  def parseUnitDefinitions(self, root):
    """
    Parse the UnitDefinitions section from the modelDescription.xml
    """
    unit_definitions = root.find("UnitDefinitions")
    if unit_definitions is not None:
      for unit_elem in unit_definitions.findall("Unit"):
        name = unit_elem.get("name")
        base_units = {}
        # Iterate over BaseUnit child elements and extract their attributes
        for base_unit_elem in unit_elem.findall("BaseUnit"):
          for key, value in base_unit_elem.attrib.items():
            base_units[key] = value
        # Create a Unit object and add it to the Units object
        unit = UnitDefinitions(name, base_units)
        self.unitDefinitions.append(unit)

  def makeConnector(self):
    for var in self.variables:
      if var.isInput() or var.isOutput() or var.isParameter() or var.isCalculatedParameter():
        connector = Connector(var.name, var.getCausalityTypeFromString(var.causality), var.getSignalTypeFromString(var.type))
        connector.setUnit(var.unit)
        self.connectors.append(connector)

  def checkVarExist(self, cref: str):
    for var in self.variables:
      if var.name == cref:
        return var
    return None

  def setValue(self, cref:str, value):
    # Check if the variable exists
    var = self.checkVarExist(cref)
    if not var:
      raise KeyError(f"Variable '{cref}' does not exist in the variables list.")

    ## set the start value provided by user
    var.setStartValue(value)

    if isinstance(value, float):
      self.value.setReal(var)
    elif isinstance(value, int):
      self.value.setInteger(var)
    elif isinstance(value, bool):
      self.value.setBoolean(var)
    elif isinstance(value, str):
      self.value.setString(var)

  def list(self, prefix=""):
    print(f"|{prefix}  ├── FMU: ({self.cref})")
    print(f"|{prefix}  |   ├── path: {self.fmupath}")

    if len(self.connectors) > 0:
      print(f"|{prefix}  |   ├── Connectors:")
      ## export component connectors
      last_prefix = prefix + "  |   |  "  # This is the prefix for nested elements
      for connector in self.connectors:
        connector.list(prefix = last_prefix)

    last_prefix = prefix + "   |       "  # This is the prefix for nested elements

    ## list parameters inline
    if not self.value.empty():
      print(f"|{prefix}  |   ├── ParameterBindings:")
      print(f"|{prefix}  |   |   ├── inline:")
      last_prefix = prefix + "  |   |"  # This is the prefix for nested elements

      self.value.list(prefix=last_prefix)

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        print(f"|{prefix}  |   ├── ParameterBindings:")
        print(f"|{prefix}  |   |   ├── {resources.filename}:")
        last_prefix = prefix + "  |   |"  # This is the prefix for nested elements
        resources.list(prefix = last_prefix)

  def exportToSSD(self, cref, node):
    component_node = ET.SubElement(node, namespace.tag("ssd", "Component"))
    component_node.set("name", cref)
    component_node.set("type", "application/x-fmu-sharedlibrary")
    component_node.set("source", "resources/"+ cref + ".fmu")

    if len(self.connectors) > 0:
      connectors_node = ET.SubElement(component_node, namespace.tag("ssd", "Connectors"))
      ## export component connectors
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)

    ## export parameter bindings
    self.value.exportToSSD(component_node)

    ## export parameters binding to ssd file with reference to ssv file
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        resources.exportToSSD(component_node)

  def exportParameterResources(self, resourceDir = None, printOutput = False):
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        ssv_node = ET.Element(namespace.tag("ssv", "ParameterSet"),
                                   nsmap={"ssc": "http://ssp-standard.org/SSP1/SystemStructureCommon",
                                          "ssv": "http://ssp-standard.org/SSP1/SystemStructureParameterValues"},
                                   version = "2.0",
                                   name = "parameters")

        resources.exportToSSV(ssv_node)
        xml = ET.tostring(ssv_node, encoding="utf-8", xml_declaration=True, pretty_print=True).decode("utf-8")

        if printOutput:
          print(xml)
        if resourceDir:
          ssvfilePath = os.path.join(resourceDir, key)
          with open(ssvfilePath, "w", encoding="utf-8") as file:
            file.write(xml)
