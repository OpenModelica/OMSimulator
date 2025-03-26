from lxml import etree as ET

from OMSimulator import namespace
from OMSimulator.variable import Causality, SignalType
from OMSimulator.cref import CRef
from typing import Union


class Connector:
  def __init__(self, name : Union[str, CRef], causality : Causality, signal_type : SignalType):
    self.name = CRef(name)
    self.causality = causality if isinstance(causality, Causality) else Causality[causality]
    self.signal_type = signal_type if isinstance(signal_type, SignalType) else SignalType[signal_type]
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
    connector_node.set("name", str(self.name))
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
        SignalType.Real: "Real",
        SignalType.Integer: "Integer",
        SignalType.Boolean: "Boolean",
        SignalType.Enum: "Enumeration",
        SignalType.String: "String"
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
            "Real": SignalType.Real,
            "Integer": SignalType.Integer,
            "Boolean": SignalType.Boolean,
            "Enum": SignalType.Enum,
            "String": SignalType.String
        }
        return signal_type_map.get(signal_type_str)
