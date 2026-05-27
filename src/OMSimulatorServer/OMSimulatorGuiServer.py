#!/usr/bin/env python3
import sys

sys.path.insert(0, "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib")  # add the path to the OMSimulatorPython package

import argparse
import traceback
import zmq
from OMSimulator import SSP, CRef, System, Component, ComponentTable, Connector, Causality, SignalType
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator.connector import ConnectorGeometry
from OMSimulator.connection import ConnectionGeometry

class OMSGuiServer:
  def __init__(self, endpoint):
    self._context = zmq.Context()
    self._socket = self._context.socket(zmq.REP)
    self._socket.connect(endpoint)
    # poller (non-blocking)
    self._poller = zmq.Poller()
    self._poller.register(self._socket, zmq.POLLIN)
    self.model = None
    self.inst = None
    print("OMS ZMQ Server started at", endpoint, flush=True)

  # main loop
  def run(self):
    try:
      while True:
        socks = dict(self._poller.poll(500))  # 500 ms
        if self._socket not in socks:
          continue
        msg = self._socket.recv_json()
        ##batch support
        if "batch" in msg:
          for cmd in msg["batch"]:
            self.handle(cmd)
          reply = {"status": "ack"}
        else:
          try:
            reply = self.handle(msg)
          except Exception as e:
            print(f"Unhandled exception in handle({msg.get('method', '?')}): {e}", flush=True)
            traceback.print_exc()
            reply = {"status": "failed", "method": msg.get("method", ""), "error": str(e)}
        self._socket.send_json(reply)
        if reply.get("status") == "shutdown":
          self.close()  # reply is sent — safe to close the REP socket now
          break
    except KeyboardInterrupt:
      print("Ctrl+C received, shutting down...", flush=True)
    finally:
      self.close()

  # dispatcher
  def handle(self, msg):
    method = msg.get("method")
    args = msg.get("args", {})
    try:
      return self._dispatch(method, args)
    except Exception as e:
      traceback.print_exc()
      return {"status": "failed", "method": method or "unknown", "error": str(e)}

  def _dispatch(self, method, args):
    # ---------- new model ----------
    if method == "newModel":
      self.model = SSP()
      ## set the model name and the system name to the same value
      self.model.activeVariant.name = args.get("name", "default")
      self.model.activeVariant.system.name = args.get("system_name", "default")
      return {"status": "ok", "method": method}

    # --------- add system ----------
    if method == "addSystem":
      try:
        cref_parts = list(args["cref"])
        model_name = self.model.activeVariant.name
        root_name = self.model.activeVariant.system.name
        # OMEdit sends ["Root", "subsystem"] because it uses LibraryTreeItem nameStructure.
        # Python SSP.addSystem expects ["test", "subsystem"].
        if cref_parts and cref_parts[0] == root_name:
            cref_parts = [model_name] + cref_parts[1:]
        elif cref_parts and cref_parts[0] != model_name:
            cref_parts = [model_name] + cref_parts
        cref = CRef(*cref_parts)
        self.model.addSystem(cref)
        return {"status": "ok", "method": method}
      except Exception as e:
        print(f"addSystem failed: {e}", flush=True)
        return {"status": "failed", "method": method, "error": str(e)}

    # ---------- export snapshot ----------
    if method == "exportSnapshot":
      xml = self.model.activeVariant.export(filename = None)
      return {"status": "ok", "method": method, "xml": xml}

    # ---------- export ----------
    if method == "export":
      print("exporting model to file", args, flush=True)
      self.model.export(args["file"])
      return {"status": "ok", "method": method}

    # ---------- import ----------
    if method == "importFile":
      print("importing model from file", args, flush=True)
      self.model = SSP(args["file"])
      print("model imported, active variant:", self.model.activeVariant.name, flush=True)
      return {"status": "ok", "method": method, "modelName": self.model.activeVariant.name}

    # ---------- add resource ----------
    if method == "addResource":
      self.model.addResource(args["source"], new_name=args["new_name"])
      return {"status": "ok", "method": method}

    # ---------- stop time ----------
    if method == "setStopTime":
      self.model.activeVariant.stopTime = args["value"]
      return {"status": "ok", "method": method}

    # ---------- add component ----------
    if method == "addComponent":
      self.model.addResource(args["source"], new_name=args["new_name"])
      cref = CRef(*args["cref"])
      self.model.addComponent(cref, args["new_name"])
      return {"status": "ok", "method": method}

    # ---------- add component ----------
    if method == "addConnector":
      CAUSALITY_MAP = {
          "Parameter": Causality.parameter,
          "Input": Causality.input,
          "Output": Causality.output,
      }

      SIGNALTYPE_MAP = {
          "Real": SignalType.Real,
          "Integer": SignalType.Integer,
          "Boolean": SignalType.Boolean,
          "String": SignalType.String,
          "Enum": SignalType.Enumeration,
      }
      causality = CAUSALITY_MAP[args["causality"]]
      signal_type = SIGNALTYPE_MAP[args["type"]]
      cref = CRef(*args["cref"])
      self.model.addConnector(cref, Connector(args["name"], causality, signal_type))
      return {"status": "ok", "method": method}

    # --------- getElements ----------
    if method == "getElements":
        json_elements = self.serializeElement(self.model.activeVariant.system)
        return {"status": "ok", "method": method, "elements": [json_elements]}

    # --------- setElementGeometry ----------
    if method == "setElementGeometry":
      try:
        cref = CRef(*args["cref"])
        root_name = self.model.activeVariant.system.name
        if cref.is_root() and str(cref.first()) == root_name:
          element = self.model.activeVariant.system
        else:
          element = self.model.getElement(cref)
        geometry = args["geometry"]
        element.elementgeometry = ElementGeometry(
          x1=geometry.get("x1", -10.0),
          y1=geometry.get("y1", -10.0),
          x2=geometry.get("x2", 10.0),
          y2=geometry.get("y2", 10.0),
          rotation=geometry.get("rotation", 0.0),
          icon_source=geometry.get("iconSource"),
          icon_rotation=geometry.get("iconRotation", 0.0),
          icon_flip=geometry.get("iconFlip", False),
          icon_fixed_aspect_ratio=geometry.get("iconFixedAspectRatio", False),
        )
        return {"status": "ok", "method": method}
      except Exception as e:
          return {"status": "failed", "method": method, "error": str(e)}

    # --------- setConnectorGeometry ----------
    if method == "setConnectorGeometry":
      cref = CRef(*args["cref"])
      connector = self.model.getConnector(cref)
      if connector is None:
        return {"status": "failed", "method": method, "error": f"Connector with cref {cref} not found"}
      geometry = args["geometry"]
      connector.connectorGeometry = ConnectorGeometry(
          x=geometry.get("x", 0.5),
          y=geometry.get("y", 0.5)
      )
      return {"status": "ok", "method": method}

    # --------- setConnectionGeometry ----------
    if method == "setConnectionGeometry":
        crefA = CRef(*args["crefA"])
        crefB = CRef(*args["crefB"])
        geometry = args["geometry"]
        pointsX = geometry.get("pointsX", [])
        pointsY = geometry.get("pointsY", [])
        connection = self.model.getConnection(crefA, crefB)
        if connection is not None:
            connection.connectionGeometry = ConnectionGeometry(pointsX, pointsY)
        return {"status": "ok", "method": method}

    # --------- addConnection ----------
    if method == "addConnection":
        crefA = CRef(*args["crefA"])
        crefB = CRef(*args["crefB"])
        self.model.addConnection(crefA, crefB)
        return {"status": "ok", "method": method}

    # ---------- solver ----------
    if method == "newSolver":
        self.model.newSolver(args)
        return {"status": "ok", "method": method}

    if method == "setSolver":
        cref = CRef(*args["cref"])
        self.model.setSolver(cref, args["solver"])
        return {"status": "ok", "method": method}
    # ---------- shutdown ----------
    if method == "shutdown":
      print("Shutdown command received, stopping server.", flush=True)
      return {"status": "shutdown"}

    return {"status": "failed", "method": method, "error": f"Unknown method: {method!r}"}

  ## helper functions to serialize model elements to JSON for getElements response
  def serializeElementGeometry(self, element):
    geometry = getattr(element, "elementgeometry", None)
    if geometry is None:
        return {
            "x1": -10.0,
            "y1": -10.0,
            "x2": 10.0,
            "y2": 10.0,
            "rotation": 0.0,
            "iconSource": None,
            "iconRotation": 0.0,
            "iconFlip": False,
            "iconFixedAspectRatio": False,
        }
    return {
        "x1": geometry.x1 if geometry.x1 is not None else -10.0,
        "y1": geometry.y1 if geometry.y1 is not None else -10.0,
        "x2": geometry.x2 if geometry.x2 is not None else 10.0,
        "y2": geometry.y2 if geometry.y2 is not None else 10.0,
        "rotation": geometry.rotation if geometry.rotation is not None else 0.0,
        "iconSource": geometry.icon_source,
        "iconRotation": geometry.icon_rotation if geometry.icon_rotation is not None else 0.0,
        "iconFlip": geometry.icon_flip if geometry.icon_flip is not None else False,
        "iconFixedAspectRatio": geometry.icon_fixed_aspect_ratio if geometry.icon_fixed_aspect_ratio is not None else False,
    }

  def serializeConnectors(self, element):
     raw_connectors = list(getattr(element, "connectors", []))
     inputs = [
         c for c in raw_connectors
         if c.causality and c.causality.name.lower() == "input"
     ]
     outputs = [
         c for c in raw_connectors
         if c.causality and c.causality.name.lower() == "output"
     ]

     def default_y(index, count):
         return float(index + 1) / float(count + 1)

     connector_json = []
     for connector in raw_connectors:
       causality = connector.causality.name.lower() if connector.causality else ""
       signal_type = connector.signal_type.name.lower() if connector.signal_type else ""
       geometry = getattr(connector, "connectorGeometry", None)
       if geometry:
         x = geometry.x if geometry.x is not None else 0.5
         y = geometry.y if geometry.y is not None else 0.5
       elif causality == "input":
         x = 0.0
         y = default_y(inputs.index(connector), len(inputs))
       elif causality == "output":
         x = 1.0
         y = default_y(outputs.index(connector), len(outputs))
       else:
         x = 0.5
         y = 0.5
       connector_json.append({
          "type": "connector",
          "name": str(connector.name),
          "causality": causality,
          "signalType": signal_type,
          "geometry": {
              "x": x,
              "y": y
          }
       })
     return connector_json

  def getFMUInfo(self, element):
      fmu_inst = self.model.resources.get(element.fmuPath)
      if not fmu_inst:
          return {}
      # Fields copied directly from FMU object
      fields = [
          "description",
          "fmiVersion",
          "generationTool",
          "guid",
          "generationDateAndTime",
          "modelName",
          # FMI capability flags
          "canBeInstantiatedOnlyOncePerProcess",
          "canGetAndSetFMUstate",
          "canNotUseMemoryManagementFunctions",
          "canSerializeFMUstate",
          "completedIntegratorStepNotNeeded",
          "needsExecutionTool",
          "providesDirectionalDerivative",
          "canInterpolateInputs",
          "maxOutputDerivativeOrder",
      ]
      info = {field: getattr(fmu_inst, field, None)for field in fields}
      # Custom / renamed fields
      info["fmiKind"] = getattr(fmu_inst, "fmuType", None)
      info["path"] = str(getattr(fmu_inst, "fmuPath", ""))
      return info

  def serializeConnectionGeometry(self, connection):
    geometry = getattr(connection, "connectionGeometry", None)

    if geometry is None:
       return {
            "pointsX": [],
            "pointsY": []
        }
    return {
        "pointsX": list(geometry.pointsX) if geometry.pointsX is not None else [],
        "pointsY": list(geometry.pointsY) if geometry.pointsY is not None else []
    }

  def serializeConnections(self, system):
    connection_json = []
    for connection in getattr(system, "connections", []):
        connection_json.append({
            "conA": ".".join([str(connection.startElement), str(connection.startConnector)]),
            "conB": ".".join([str(connection.endElement), str(connection.endConnector)]),
            "type": "connection",
            "geometry": self.serializeConnectionGeometry(connection)
        })
    return connection_json

  def serializeElement(self, element):
    node = {
        "name": str(element._name) if isinstance(element, System) else str(element.name),
        "type": "system" if isinstance(element, System) else "component",
        "elements": []
    }
    node["connectors"] = self.serializeConnectors(element)
    # Add geometry for both systems and components.
    if isinstance(element, (System, Component)):
        node["geometry"] = self.serializeElementGeometry(element)
    if isinstance(element, Component):
        node["fmuInfo"] = self.getFMUInfo(element)
    if isinstance(element, System):
        node["connections"] = self.serializeConnections(element)
        for key, child in element.elements.items():
            node["elements"].append(self.serializeElement(child))
    return node

  # close
  def close(self):
    if self._socket is not None and not self._socket.closed:
      print("OMS server shutting down", flush=True)
      self._socket.close()
      self._context.term()
      self._socket = None
      self._context = None

def _main():
  # parse command-line arguments
  parser = argparse.ArgumentParser(description='OMS-SERVER', allow_abbrev=False)
  parser.add_argument('--endpoint-rep', default=None, help='define the endpoint used for the interactive simulation communication')
  args = parser.parse_args()
  server = OMSGuiServer(args.endpoint_rep)
  server.run()


if __name__ == "__main__":
  _main()
