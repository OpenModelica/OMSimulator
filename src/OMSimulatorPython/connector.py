from lxml import etree as ET

from OMSimulator import namespace, types
from OMSimulator.variable import Causality, SignalType


class Connector:
  def __init__(self, name : str, causality : Causality, signal_type : SignalType):
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
  def getCausalityToString(causality_ : Causality) -> str:
    causality_map = {
        Causality.input: "input",
        Causality.output: "output",
        Causality.parameter: "parameter",
        Causality.calculatedParameter: "calculatedParameter"
    }
    return causality_map.get(causality_, "unknown")

  @staticmethod
  def getSignalTypeToString(signalTypes : SignalType) -> str:
    signalTypes_map = {
        SignalType.real: "Real",
        SignalType.integer: "Integer",
        SignalType.boolean: "Boolean",
        SignalType.enum: "Enumeration",
        SignalType.string: "String"
    }
    return signalTypes_map.get(signalTypes, "unknown")

  @staticmethod
  def getCausalityFromString(kind: str) -> Causality:
        """Converts a kind string to the corresponding causality enum"""
        kind_map = {
            "input": Causality.input,
            "output": Causality.output,
            "parameter": Causality.parameter,
            "calculatedParameter": Causality.calculatedParameter
        }
        return kind_map.get(kind)

  @staticmethod
  def getSignalTypeFromString(signal_type_str: str) -> SignalType:
        """Converts a signal type string to the corresponding signal type enum"""
        signal_type_map = {
            "ssc:Real": SignalType.real,
            "ssc:Integer": SignalType.integer,
            "ssc:Boolean": SignalType.boolean,
            "ssc:Enum": SignalType.enum,
            "ssc:String": SignalType.string
        }
        return signal_type_map.get(signal_type_str)
