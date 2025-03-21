from lxml import etree as ET

from OMSimulator import namespace, types


class Connector:
  def __init__(self, name : str, causality : types.Causality, signal_type : types.SignalType):
    self.name = name
    self.causality = causality
    self.signal_type = signal_type
    self.unit = None

  def getCref(self):
    return self.name

  def getSignalType(self):
     return self.signal_type

  def getCausality(self):
     return self.causality

  def setUnit(self, unit: str):
    self.unit = unit

  def list(self, prefix=""):
    print(f"|{prefix} * ({self.name}, {self.getCausalityToString(self.causality)}, {self.getSignalTypeToString(self.signal_type)})")
    pass

  def exportToSSD(self, node):
    connector_node = ET.SubElement(node, namespace.tag("ssd", "Connector"))
    connector_node.set("name", self.name)
    connector_node.set("kind", Connector.getCausalityToString(self.causality))
    connectors_type = ET.SubElement(connector_node, namespace.tag("ssc", Connector.getSignalTypeToString(self.signal_type)))
    if self.unit is not None:
      connectors_type.set("unit", self.unit)

  @staticmethod
  def getCausalityToString(causality_ : types.Causality) -> str:
    causality_map = {
        types.Causality.input: "input",
        types.Causality.output: "output",
        types.Causality.parameter: "parameter",
        types.Causality.calculatedParameter: "calculatedParameter"
    }
    return causality_map.get(causality_, "unknown")

  @staticmethod
  def getSignalTypeToString(signalTypes : types.SignalType) -> str:
    signalTypes_map = {
        types.SignalType.real: "Real",
        types.SignalType.integer: "Integer",
        types.SignalType.boolean: "Boolean",
        types.SignalType.enum: "Enumeration",
        types.SignalType.string: "String"
    }
    return signalTypes_map.get(signalTypes, "unknown")

  @staticmethod
  def getCausalityFromString(kind: str) -> types.Causality:
        """Converts a kind string to the corresponding causality enum"""
        kind_map = {
            "input": types.Causality.input,
            "output": types.Causality.output,
            "parameter": types.Causality.parameter,
            "calculatedParameter": types.Causality.calculatedParameter
        }
        return kind_map.get(kind)

  @staticmethod
  def getSignalTypeFromString(signal_type_str: str) -> types.SignalType:
        """Converts a signal type string to the corresponding signal type enum"""
        signal_type_map = {
            "ssc:Real": types.SignalType.real,
            "ssc:Integer": types.SignalType.integer,
            "ssc:Boolean": types.SignalType.boolean,
            "ssc:Enum": types.SignalType.enum,
            "ssc:String": types.SignalType.string
        }
        return signal_type_map.get(signal_type_str)
