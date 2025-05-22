import logging
from lxml import etree as ET
from pathlib import Path

from OMSimulator.component import Component
from OMSimulator.connection import Connection, ConnectionGeometry
from OMSimulator.connector import Connector, ConnectorGeometry
from OMSimulator.unit import Unit
from OMSimulator.variable import Causality, SignalType
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator import namespace, CRef
from collections import defaultdict

logger = logging.getLogger(__name__)

def _setParameters(parameterValues: dict, obj):
  if len(parameterValues) > 0:
    for key, (value, unit, description) in parameterValues.items():
      obj.value.setValue(key, value, unit, description)

def parseDefaultExperiment(node, root):
  default_experiment = node.find("ssd:DefaultExperiment", namespaces=namespace.ns)
  if default_experiment is None:
    return
  root.startTime = default_experiment.get("startTime")
  root.stopTime = default_experiment.get("stopTime")
  ##TODO parse ssd:annotation


def parseElements(node, resources = None):
  """Extract components from <ssd:Elements> section"""
  from OMSimulator.system import System, SystemGeometry

  elements = {}
  elements_node = node.find("ssd:Elements", namespaces=namespace.ns)
  if elements_node is None:
    return elements

  # Parse the sub system <ssd:system> in the <ssd:Elements> section
  for system in elements_node.findall("ssd:System", namespaces=namespace.ns):
    name = CRef(system.get("name"))
    description = system.get("description")
    elements[name] = System(name)
    elements[name].description = description
    elements[name].connectors = Connector.importFromNode(system)
    elements[name].elementgeometry = ElementGeometry.importFromNode(system)
    elements[name].systemgeometry = SystemGeometry.importFromNode(system)
    parseParameterBindings(system, elements[name], resources)
    solvers = parseAnnotations(system)
    if solvers:
      for solver in solvers:
        elements[name].solver = solver.get("name")
    elements[name].elements = parseElements(system, resources)  # recursively parse nested elements in the sub-system
    Connection.importFromNode(system, elements[name]) # parse connections for the sub-system

  for component in elements_node.findall("ssd:Component", namespaces=namespace.ns):
    name = CRef(component.get("name"))
    comp_type = component.get("type")
    source = component.get("source")
    description = component.get("description")
    ## check implementation
    match component.get("implementation", "any"):
      case "any":
        implementation = "me_cs"
      case "ModelExchange":
        implementation = "me"
      case "CoSimulation":
        implementation = "cs"
      case _:
        raise ValueError(f"Unknown FMU implementation type: {implementation}")

    elements[name] = Component(name, source)
    elements[name].implementation = implementation
    elements[name].description = description
    elements[name].connectors = Connector.importFromNode(component)
    elements[name].elementgeometry = ElementGeometry.importFromNode(component)
    parseParameterBindings(component, elements[name], resources)
    solvers = parseAnnotations(component)
    if solvers:
      for solver in solvers:
        elements[name].solver = solver.get("name")

  return elements

def parseParameterBindings(node, obj, resources):
  """Extract and print system parameters"""
  parameter_bindings = node.find("ssd:ParameterBindings", namespaces=namespace.ns)
  if parameter_bindings is not None:
    for binding in parameter_bindings.findall("ssd:ParameterBinding", namespaces=namespace.ns):
      source = binding.get("source")
      if binding.get("source"):
        ## use the instantiated ssv class to set the parameter Resources
        if source not in resources:
          logger.warning(f"SSV file not found: {source}")
        obj.parameterResources.append(source)
      else:
        values = binding.find("ssd:ParameterValues", namespaces=namespace.ns)
        if values is not None:
          param_set = values.find("ssv:ParameterSet", namespaces=namespace.ns)
          if param_set is not None:
            parameters = param_set.find("ssv:Parameters", namespaces=namespace.ns)
            parameterValues = parseParameterBindingHelper(parameters)
            _setParameters(parameterValues, obj)

          units = param_set.find("ssv:Units", namespaces=namespace.ns)
          Unit.importFromNode(param_set, obj, tagname="ssv:Units")

def parseSSV(filename):
  tree = ET.parse(filename)
  root = tree.getroot()
  validateSSP(root, filename, "SystemStructureParameterValues.xsd")
  parameters = root.find("ssv:Parameters", namespaces=namespace.ns)
  return parseParameterBindingHelper(parameters)

def parseSSM(filename):
  tree = ET.parse(filename)
  root = tree.getroot()
  validateSSP(root, filename, "SystemStructureParameterMapping.xsd")
  mappingEntry = defaultdict(list)
  for mapping in root.findall("ssm:MappingEntry", namespaces=namespace.ns):
    source = mapping.get("source")
    target = mapping.get("target")
    mappingEntry[source].append(target)
  return mappingEntry

def parseParameterBindingHelper(parameters):
  if parameters is not None:
    parameterValues={}
    for param in parameters.findall("ssv:Parameter", namespaces=namespace.ns):
      name = CRef(param.get("name"))
      description = param.get("description")
      value_types = {
                      "ssv:Real": float,
                      "ssv:Integer": int,
                      "ssv:Boolean": lambda v: v.lower() == "true",  # Convert "true"/"false" to bool
                      "ssv:String": str
                    }
      for value_type, cast_func in value_types.items():
        value_element = param.find(value_type, namespaces=namespace.ns)
        if value_element is not None:
          value = value_element.get("value")
          unit = value_element.get("unit")
          parameterValues[name] = (cast_func(value), unit, description)  # Convert to correct type
          break  # Stop after first found type
    return parameterValues

def validateSSP(root, filename : str, schema_file : str):
  module_dir = Path(__file__).parent
  schema = ET.XMLSchema(file=Path(module_dir, 'schema/ssp', schema_file))
  if not schema.validate(root):
    message = f"Failed to validate {Path(filename).name} against schemafile {schema_file}"
    for entry in schema.error_log:
      message += "\n%s (line %d, column %d): %s" % (entry.level_name, entry.line, entry.column, entry.message)
    raise Exception(message)

def exportAnnotations(node, solvers):
  """Export annotations to the XML node"""
  ssd_annotation_node = ET.SubElement(node, namespace.tag("ssd", "Annotations"))
  annotation_node = ET.SubElement(ssd_annotation_node, namespace.tag("ssc", "Annotation"))
  annotation_node.set("type", "org.openmodelica")
  oms_annotation_node = ET.SubElement(annotation_node, namespace.tag("oms", "Annotations"))
  if (isinstance(solvers, list)):
    for solver in solvers:
      oms_simulationInformation_node = ET.SubElement(oms_annotation_node, namespace.tag("oms", "SimulationInformation"))
      for key, value in solver.items():
        oms_simulationInformation_node.set(key, str(value))
  else:
    oms_simulationInformation_node = ET.SubElement(oms_annotation_node, namespace.tag("oms", "SimulationInformation"))
    oms_simulationInformation_node.set("name", solvers)

def parseAnnotations(node):
  """Extract and print system annotations"""
  annotations_node = node.find("ssd:Annotations", namespaces=namespace.ns)
  if annotations_node is None:
    return []

  solvers = []
  for annotation in annotations_node.findall("ssc:Annotation", namespaces=namespace.ns):
    type = annotation.get("type")
    if type == "org.openmodelica":
      oms_annotation = annotation.find("oms:Annotations", namespaces=namespace.ns)
      if oms_annotation is not None:
        oms_simulationInformation = oms_annotation.findall("oms:SimulationInformation", namespaces=namespace.ns)
        for sim in oms_simulationInformation:
          solvers.append(sim.attrib)

  return solvers
