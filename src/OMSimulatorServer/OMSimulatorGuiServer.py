#!/usr/bin/env python3

# This file is part of OpenModelica.
#
# Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
# c/o Linköpings universitet, Department of Computer and Information Science,
# SE-58183 Linköping, Sweden.
#
# All rights reserved.
#
# THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
# THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
# VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
#
# The OpenModelica software and the OSMC (Open Source Modelica Consortium)
# Public License (OSMC-PL) are obtained from OSMC, either from the above
# address, from the URLs:
# http://www.openmodelica.org or
# https://github.com/OpenModelica/ or
# http://www.ida.liu.se/projects/OpenModelica,
# and in the OpenModelica distribution.
#
# GNU AGPL version 3 is obtained from:
# https://www.gnu.org/licenses/licenses.html#GPL
#
# This program is distributed WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
# IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
#
# See the full OSMC Public License conditions for more details.

import sys
sys.path.insert(0, "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib")

import argparse
import traceback
import zmq
from OMSimulator import SSP, CRef, System, Component, ComponentTable, Connector, Causality, SignalType
from OMSimulator.ssd import SSD
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator.connector import ConnectorGeometry
from OMSimulator.connection import ConnectionGeometry

class OMSGuiServer:
  def __init__(self, endpoint):
    self._context = zmq.Context()
    self._socket = self._context.socket(zmq.REP)
    self._socket.connect(endpoint)
    self._poller = zmq.Poller()
    self._poller.register(self._socket, zmq.POLLIN)

    # Multiple SSP models can be open simultaneously.Keyed by model name (activeVariant.name)
    self.models = {}  # model_name -> SSP

    print("OMS ZMQ Server started at", endpoint, flush=True)

  # -----------------------------------
  # main loop
  # -----------------------------------
  def run(self):
    try:
      while True:
        socks = dict(self._poller.poll(500))
        if self._socket not in socks:
          continue
        msg = self._socket.recv_json()
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
          self.close()
          break
    except KeyboardInterrupt:
      print("Ctrl+C received, shutting down...", flush=True)
    finally:
      self.close()

  # -----------------------------------
  # dispatcher
  # -----------------------------------
  def handle(self, msg):
    method = msg.get("method")
    args = msg.get("args", {})
    model_name = msg.get("model")   # every command carries the target model name
    try:
      return self._dispatch(method, args, model_name)
    except Exception as e:
      traceback.print_exc()
      return {"status": "failed", "method": method or "unknown", "error": str(e)}

  def _get_model(self, model_name):
    """Return the SSP for model_name, or raise a descriptive error."""
    model = self.models.get(model_name)
    if model is None:
      raise ValueError(f"Model '{model_name}' not found. Available: {list(self.models.keys())}")
    return model

  def _dispatch(self, method, args, model_name=None):

    # ---------- new model ----------
    # These methods don't require an existing model — handle before _get_model().
    if method == "newModel":
      name = args.get("name", "default")
      ssp = SSP()
      # SSP() always creates a variant keyed as "default" — re-key it to the requested name.
      ssd = ssp.variants.pop("default")
      ssd.name = name
      ssp.variants[name] = ssd
      ssp.activeVariantName = name
      ssp.activeVariant.system.name = args.get("system_name", "default")
      self.models[name] = ssp
      print(f"New model created: {name}", flush=True)
      return {"status": "ok", "method": method}

    # ---------- import file ----------
    if method == "importFile":
      print("importing model from file", args, flush=True)
      ssp = SSP(args["file"])
      name = ssp.activeVariant.name
      self.models[name] = ssp
      return {"status": "ok", "method": method, "modelName": name}

    # ---------- delete model ----------
    if method == "deleteModel":
      if model_name in self.models:
        del self.models[model_name]
      print(f"Model deleted: {model_name}", flush=True)
      return {"status": "ok", "method": method}

    # ---------- shutdown ----------
    if method == "shutdown":
      print("Shutdown command received, stopping server.", flush=True)
      self.models.clear()
      return {"status": "shutdown"}

    # All methods below require an existing model — look it up once.
    model = self._get_model(model_name)

    # --------- add system ----------
    if method == "addSystem":
      cref_parts = list(args["cref"])
      root_name = model.activeVariant.system.name
      # OMEdit sends ["Root", "subsystem"]; Python SSP expects ["modelName", "subsystem"]
      if cref_parts and cref_parts[0] == root_name:
        cref_parts = [model_name] + cref_parts[1:]
      elif cref_parts and cref_parts[0] != model_name:
        cref_parts = [model_name] + cref_parts
      cref = CRef(*cref_parts)
      model.addSystem(cref)
      return {"status": "ok", "method": method}

    # ---------- export snapshot ----------
    if method == "exportSnapshot":
      xml = model.activeVariant.export(filename=None)
      return {"status": "ok", "method": method, "xml": xml}

    # ---------- import snapshot ----------
    if method == "importSnapshot":
      snapshot_xml = args.get("snapshot", "")
      # Parse the snapshot XML into a new SSD, reusing existing resources (FMUs).
      new_ssd = SSD.importFromString(snapshot_xml, model.resources)
      new_name = new_ssd.name
      # Root system name — mirrors OMS2's new_root_cref return so C++ can build
      # the correct newEditedCref when the root system is renamed in the text editor.
      new_root_cref = new_ssd.system.name if new_ssd.system is not None else ""
      old_name = model.activeVariantName
      # Replace the variant in the SSP variants dict.
      del model.variants[old_name]
      model.variants[new_name] = new_ssd
      model._activeVariantName = new_name
      # If the top-level model name changed, re-key self.models.
      if new_name != model_name:
        self.models[new_name] = self.models.pop(model_name)
      #print(f"importSnapshot: '{model_name}' -> '{new_name}' (root: '{new_root_cref}')", flush=True)
      return {"status": "ok", "method": method, "modelName": new_name, "rootCref": new_root_cref}

    # ---------- export to file ----------
    if method == "export":
      print("exporting model to file", args, flush=True)
      model.export(args["file"])
      return {"status": "ok", "method": method}

    # ---------- add resource ----------
    if method == "addResource":
      model.addResource(args["source"], new_name=args["new_name"])
      return {"status": "ok", "method": method}

    # ---------- delete ----------
    if method == "delete":
      cref_parts = list(args["cref"])
      # Empty cref means the top-level model itself should be deleted.
      if not cref_parts:
        del self.models[model_name]
      else:
        cref = CRef(*cref_parts)
        model.delete(cref)
      return {"status": "ok", "method": method}

    # ---------- setValue ----------
    if method == "setValue":
      cref = CRef(*args["cref"])
      model.setValue(cref, args["value"])
      return {"status": "ok", "method": method}

    # ---------- getValue ----------
    if method == "getValue":
      cref = CRef(*args["cref"])
      value = model.getValue(cref)
      return {"status": "ok", "method": method, "value": str(value)}

    # ---------- get/set start time ----------
    if method == "getStartTime":
      return {"status": "ok", "method": method, "value": str(model.activeVariant.startTime)}

    if method == "setStartTime":
      model.activeVariant.startTime = args["value"]
      return {"status": "ok", "method": method}

    # ---------- get/set stop time ----------
    if method == "getStopTime":
      return {"status": "ok", "method": method, "value": str(model.activeVariant.stopTime)}

    if method == "setStopTime":
      model.activeVariant.stopTime = args["value"]
      return {"status": "ok", "method": method}

    # ---------- get/set result file ----------
    if method == "getResultFile":
      return {"status": "ok", "method": method, "fileName": model.activeVariant.resultFile, "bufferSize": str(model.activeVariant.bufferSize)}

    if method == "setResultFile":
      model.activeVariant.resultFile = args.get("fileName", "model_res.mat")
      model.activeVariant.bufferSize = args.get("bufferSize", 10)
      return {"status": "ok", "method": method}

    # ---------- getTolerance----------
    if method == "getTolerance":
      return {"status": "ok", "method": method, "value": str(model.activeVariant.tolerance)}

    # ---------- setTolerance ----------
    if method == "setTolerance":
      model.activeVariant.tolerance = args["value"]
      return {"status": "ok", "method": method}

    ## ---------- get/set step size ----------
    if method == "getVariableStepSize":
      return {"status": "ok", "method": method, "initialStepSize": str(model.activeVariant.initialStepSize), "minimumStepSize": str(model.activeVariant.minimumStepSize), "maximumStepSize": str(model.activeVariant.maximumStepSize)}

    if method == "setVariableStepSize":
      model.activeVariant.initialStepSize = args["initialStepSize"]
      model.activeVariant.minimumStepSize = args["minimumStepSize"]
      model.activeVariant.maximumStepSize = args["maximumStepSize"]
      return {"status": "ok", "method": method}

    ## --------- get/set fixed step size ----------
    if method == "getFixedStepSize":
      return {"status": "ok", "method": method, "value": str(model.activeVariant.maximumStepSize)}

    ## --------- set fixed step size (sets variable step size with equal initial, min, max) ----------
    if method == "setFixedStepSize":
      step_size = args["value"]
      model.activeVariant.initialStepSize = step_size
      model.activeVariant.minimumStepSize = step_size
      model.activeVariant.maximumStepSize = step_size
      return {"status": "ok", "method": method}

    # ---------- add component ----------
    if method == "addComponent":
      model.addResource(args["source"], new_name=args["new_name"])
      cref = CRef(*args["cref"])
      model.addComponent(cref, args["new_name"])
      return {"status": "ok", "method": method}

    # ---------- add connector ----------
    if method == "addConnector":
      CAUSALITY_MAP = {
        "Parameter": Causality.parameter,
        "Input":     Causality.input,
        "Output":    Causality.output,
      }
      SIGNALTYPE_MAP = {
        "Real":    SignalType.Real,
        "Integer": SignalType.Integer,
        "Boolean": SignalType.Boolean,
        "String":  SignalType.String,
        "Enum":    SignalType.Enumeration,
      }
      causality   = CAUSALITY_MAP[args["causality"]]
      signal_type = SIGNALTYPE_MAP[args["type"]]
      cref = CRef(*args["cref"])
      model.addConnector(cref, Connector(args["name"], causality, signal_type))
      return {"status": "ok", "method": method}

    # ---------- get elements ----------
    if method == "getElements":
      json_elements = self.serializeElement(model.activeVariant.system, model)
      return {"status": "ok", "method": method, "elements": [json_elements]}

    # ---------- set element geometry ----------
    if method == "setElementGeometry":
      cref = CRef(*args["cref"])
      root_name = model.activeVariant.system.name
      if cref.is_root() and str(cref.first()) == root_name:
        element = model.activeVariant.system
      else:
        element = model.getElement(cref)
      g = args["geometry"]
      element.elementgeometry = ElementGeometry(
        x1=g.get("x1", -10.0), y1=g.get("y1", -10.0),
        x2=g.get("x2",  10.0), y2=g.get("y2",  10.0),
        rotation=g.get("rotation", 0.0),
        icon_source=g.get("iconSource"),
        icon_rotation=g.get("iconRotation", 0.0),
        icon_flip=g.get("iconFlip", False),
        icon_fixed_aspect_ratio=g.get("iconFixedAspectRatio", False),
      )
      return {"status": "ok", "method": method}

    # ---------- set connector geometry ----------
    if method == "setConnectorGeometry":
      cref = CRef(*args["cref"])
      connector = model.getConnector(cref)
      if connector is None:
        return {"status": "failed", "method": method, "error": f"Connector '{cref}' not found"}
      g = args["geometry"]
      connector.connectorGeometry = ConnectorGeometry(x=g.get("x", 0.5), y=g.get("y", 0.5))
      return {"status": "ok", "method": method}

    # ---------- set connection geometry ----------
    if method == "setConnectionGeometry":
      crefA = CRef(*args["crefA"])
      crefB = CRef(*args["crefB"])
      g = args["geometry"]
      connection = model.getConnection(crefA, crefB)
      if connection is not None:
        connection.connectionGeometry = ConnectionGeometry(g.get("pointsX", []), g.get("pointsY", []))
      return {"status": "ok", "method": method}

    # ---------- add connection ----------
    if method == "addConnection":
      crefA = CRef(*args["crefA"])
      crefB = CRef(*args["crefB"])
      model.addConnection(crefA, crefB)
      return {"status": "ok", "method": method}

    # ---------- delete connection ----------
    if method == "deleteConnection":
      crefA = CRef(*args["crefA"])
      crefB = CRef(*args["crefB"])
      model.deleteConnection(crefA, crefB)
      return {"status": "ok", "method": method}

    # ---------- solver settings ----------
    if method == "getSolverSettings":
      # Return all named solver configs and per-component assignments.
      solver_list = []
      for solver in model.activeVariant.system.solvers:
        solver_dict = {}
        for key, value in solver.items():
          solver_dict[str(key)] = str(value)
        solver_list.append(solver_dict)

      assignments = {}
      for elem_name, element in model.activeVariant.system.elements.items():
        if hasattr(element, 'solver') and element.solver:
          assignments[str(elem_name)] = str(element.solver)
        if isinstance(element, System):
          for comp_name, comp in element.elements.items():
            if hasattr(comp, 'solver') and comp.solver:
              assignments[str(comp_name)] = str(comp.solver)

      return {"status": "ok", "method": method, "solvers": solver_list, "assignments": assignments}

    if method == "setSolverSettings":
      # Replace all solver configs and re-apply component assignments.
      model.activeVariant.system.solvers = []
      for solver in args.get("solvers", []):
        model.newSolver(solver)
      for comp_name, solver_name in args.get("assignments", {}).items():
        comp_ref = comp_name.split(".")[1:]  # assuming comp_name is like "subsystem.component"
        cref = CRef(*comp_ref)
        model.setSolver(cref, solver_name)
      return {"status": "ok", "method": method}

    # ---------- rename ----------
    if method == "rename":
      cref_parts = list(args.get("cref", []))
      print(f"rename: model='{model_name}', cref_parts={cref_parts}, new_name='{args.get('newName')}'", flush=True)
      new_name   = args["newName"]
      if not cref_parts:
        # Top-level model rename: re-key self.models and update the SSD name.
        if new_name != model_name:
          model.activeVariant.name = new_name
          model._activeVariantName = new_name
          print(f"check variants: {model.variants.keys()}", flush=True)
          model.variants[new_name] = model.variants.pop(model_name)
          self.models[new_name] = self.models.pop(model_name)
      else:
        # Component/subsystem rename — delegate to the SSP API.
        model.rename(CRef(*cref_parts), CRef(new_name))
      return {"status": "ok", "method": method}

    # ---------- component-level solver assignment ----------
    if method == "setSolver":
      cref = CRef(*args["cref"])
      model.setSolver(cref, args["solver"])
      return {"status": "ok", "method": method}

    return {"status": "failed", "method": method, "error": f"Unknown method: {method!r}"}

  # -----------------------------------
  # serialization helpers
  # -----------------------------------
  def serializeElementGeometry(self, element):
    g = getattr(element, "elementgeometry", None)
    if g is None:
      return {"x1": -10.0, "y1": -10.0, "x2": 10.0, "y2": 10.0,
              "rotation": 0.0, "iconSource": None, "iconRotation": 0.0,
              "iconFlip": False, "iconFixedAspectRatio": False}
    return {
      "x1": g.x1 if g.x1 is not None else -10.0,
      "y1": g.y1 if g.y1 is not None else -10.0,
      "x2": g.x2 if g.x2 is not None else  10.0,
      "y2": g.y2 if g.y2 is not None else  10.0,
      "rotation":             g.rotation             if g.rotation             is not None else 0.0,
      "iconSource":           g.icon_source,
      "iconRotation":         g.icon_rotation         if g.icon_rotation         is not None else 0.0,
      "iconFlip":             g.icon_flip             if g.icon_flip             is not None else False,
      "iconFixedAspectRatio": g.icon_fixed_aspect_ratio if g.icon_fixed_aspect_ratio is not None else False,
    }

  def serializeConnectors(self, element):
    raw = list(getattr(element, "connectors", []))
    inputs  = [c for c in raw if c.causality and c.causality.name.lower() == "input"]
    outputs = [c for c in raw if c.causality and c.causality.name.lower() == "output"]

    def default_y(index, count):
      return float(index + 1) / float(count + 1)

    result = []
    for c in raw:
      causality   = c.causality.name.lower()   if c.causality   else ""
      signal_type = c.signal_type.name.lower() if c.signal_type else ""
      geom = getattr(c, "connectorGeometry", None)
      if geom:
        x, y = (geom.x if geom.x is not None else 0.5), (geom.y if geom.y is not None else 0.5)
      elif causality == "input":
        x, y = 0.0, default_y(inputs.index(c), len(inputs))
      elif causality == "output":
        x, y = 1.0, default_y(outputs.index(c), len(outputs))
      else:
        x, y = 0.5, 0.5
      result.append({
        "type": "connector", "name": str(c.name),
        "causality": causality, "signalType": signal_type,
        "geometry": {"x": x, "y": y}
      })
    return result

  def getFMUInfo(self, element, model):
    fmu_inst = model.resources.get(element.fmuPath)
    if not fmu_inst:
      return {}
    fields = [
      "description", "fmiVersion", "generationTool", "guid",
      "generationDateAndTime", "modelName",
      "canBeInstantiatedOnlyOncePerProcess", "canGetAndSetFMUstate",
      "canNotUseMemoryManagementFunctions", "canSerializeFMUstate",
      "completedIntegratorStepNotNeeded", "needsExecutionTool",
      "providesDirectionalDerivative", "canInterpolateInputs",
      "maxOutputDerivativeOrder",
    ]
    info = {f: getattr(fmu_inst, f, None) for f in fields}
    info["fmiKind"] = getattr(fmu_inst, "fmuType", None)
    info["path"]    = str(getattr(fmu_inst, "fmuPath", ""))
    return info

  def serializeConnectionGeometry(self, connection):
    g = getattr(connection, "connectionGeometry", None)
    if g is None:
      return {"pointsX": [], "pointsY": []}
    return {
      "pointsX": list(g.pointsX) if g.pointsX is not None else [],
      "pointsY": list(g.pointsY) if g.pointsY is not None else [],
    }

  def serializeConnections(self, system):
    result = []
    for conn in getattr(system, "connections", []):
      result.append({
        "conA": ".".join([str(conn.startElement), str(conn.startConnector)]),
        "conB": ".".join([str(conn.endElement),   str(conn.endConnector)]),
        "type": "connection",
        "geometry": self.serializeConnectionGeometry(conn)
      })
    return result

  def serializeElement(self, element, model):
    if element is None:
      return
    node = {
      "name": str(element._name) if isinstance(element, System) else str(element.name),
      "type": "system" if isinstance(element, System) else "component",
      "elements": [],
      "connectors": self.serializeConnectors(element),
    }
    if isinstance(element, (System, Component)):
      node["geometry"] = self.serializeElementGeometry(element)
    if isinstance(element, Component):
      node["fmuInfo"] = self.getFMUInfo(element, model)
    if isinstance(element, System):
      node["connections"] = self.serializeConnections(element)
      for child in element.elements.values():
        node["elements"].append(self.serializeElement(child, model))
    return node

  # -----------------------------------
  # close
  # -----------------------------------
  def close(self):
    if self._socket is not None and not self._socket.closed:
      print("OMS server shutting down", flush=True)
      self._socket.close()
      self._context.term()
      self._socket = None
      self._context = None

def _main():
  parser = argparse.ArgumentParser(description='OMS-GUI-SERVER', allow_abbrev=False)
  parser.add_argument('--endpoint-rep', default=None)
  args = parser.parse_args()
  server = OMSGuiServer(args.endpoint_rep)
  server.run()

if __name__ == "__main__":
  _main()
