#!/usr/bin/env python3
#from platform import node
import sys

sys.path.insert(0, "C:/OPENMODELICAGIT/OpenModelica/OMSimulator/install/lib")  # add the path to the OMSimulatorPython package

import argparse
import zmq
from OMSimulator import SSP, CRef, System, Component, ComponentTable, Connector, Causality, SignalType
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator.connector import ConnectorGeometry

class OMSServer:
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

    # -----------------------------------
    # main loop
    # -----------------------------------
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
                    reply = self.handle(msg)

                self._socket.send_json(reply)

                if reply.get("status") == "shutdown":
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

        # ---------- new model ----------
        if method == "newModel":
            self.model = SSP()
            ##print("New model created ARRRRRRRRRRR", flush=True)
            # print(args, flush=True)
            # print(args.get("name", "default"), flush=True)
            name = args.get("name", "default")
            ## set the model name and the system name to the same value
            self.model.activeVariant.name = args.get("name", "default")
            self.model.activeVariant.system.name = args.get("system_name", "default")
            ##print(self.model.list(), flush=True)
            return {"status": "ok", "method": method}

        if method == "list":
            return {"status": "ok", "method": method}

        if method == "exportSnapshot":
            xml = self.model.activeVariant.export(filename = None)
            return {"status": "ok", "method": method, "xml": xml}

        # ---------- export ----------
        elif method == "export":
            print("exporting model to file", args, flush=True)
            self.model.export(args["file"])
            return {"status": "ok", "method": method}

        # ---------- add resource ----------
        elif method == "addResource":
            self.model.addResource(
                args["source"],
                new_name=args["new_name"]
            )
            return {"status": "ok", "method": method}

        # ---------- stop time ----------
        elif method == "setStopTime":
            self.model.activeVariant.stopTime = args["value"]
            return {"status": "ok", "method": method}


        # ---------- add component ----------
        elif method == "addComponent":
            # print(f"addComponent args: {args}", flush=True)
            # print(f"add component cref: {args.get('cref', 'default')}", flush=True)
            self.model.addResource(args["source"], new_name=args["new_name"])
            cref = CRef(*args["cref"])
            # print(f"cref: {cref}", flush=True)
            self.model.addComponent(cref, args["new_name"])
            # print(f"model after addComponent: {self.model.list()}", flush=True)
            return {"status": "ok", "method": method}

        # ---------- add component ----------
        elif method == "addConnector":
            print(f"addConnector args: {args}", flush=True)
            print(f"addConnector cref: {args.get('cref', 'default')}", flush=True)
            cref = CRef(*args["cref"])
            print(f"cref: {cref}", flush=True)
            self.model.addConnector(cref, Connector(args["name"], Causality.input, SignalType.Real))
            # print(f"model after addConnector: {self.model.list()}", flush=True)
            return {"status": "ok", "method": method}

        elif method == "getElements":
            # print(f"getElements Python", flush=True)
            # print(f"get element args Python : {args}", flush=True)
            #system = self.model.activeVariant.system

            # for key, element in elements.items():
            #     print(f"element items Python : {key}", flush=True)
            json_elements = self.serializeElement(self.model.activeVariant.system)

            # print(f"json_elements_python: {json_elements}", flush=True)

            return {"status": "ok", "method": method, "elements": [json_elements]}

        elif method == "setElementGeometry":
            cref = CRef(*args["cref"])
            print(f"setElementGeometry Novak for cref: {cref} with geometry: {args['geometry']}", flush=True)

            element = self.model.getElement(cref)
            if element is None:
              return {"status": "failed", "method": method, "error": f"Element with cref {cref} not found"}

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

        elif method == "setConnectorGeometry":
            cref = CRef(*args["cref"])
            print(f"setConnectorGeometry Ayana for cref: {cref} with geometry: {args['geometry']}", flush=True)
            connector = self.model.getConnector(cref)
            print(f"Connector found: {connector} {connector.name}", flush=True)
            if connector is None:
              return {"status": "failed", "method": method, "error": f"Connector with cref {cref} not found"}
            geometry = args["geometry"]
            connector.connectorGeometry = ConnectorGeometry(
                x=geometry.get("x", 0.5),
                y=geometry.get("y", 0.5)
            )
            return {"status": "ok", "method": method}

        # ---------- solver ----------
        elif method == "newSolver":
            self.model.newSolver(args)
            return {"status": "ok", "method": method}

        elif method == "setSolver":
            cref = CRef(*args["cref"])
            self.model.setSolver(cref, args["solver"])
            return {"status": "ok", "method": method}


        # ---------- instantiate ----------
        elif method == "instantiate":
            self.inst = self.model.instantiate()
            return {"status": "ok", "method": method}

        # ---------- result file ----------
        elif method == "setResultFile":
            self.inst.setResultFile(args["file"])
            return {"status": "ok", "method": method}

        # ---------- simulation ----------
        elif method == "initialize":
            self.inst.initialize()
            return {"status": "ok", "method": method}

        elif method == "simulate":
            self.inst = self.model.instantiate()  # ensure the model is instantiated before simulation
            self.inst.initialize()   # ensure the model is initialized before simulation
            self.inst.simulate()
            self.inst.terminate()
            self.inst.delete()
            return {"status": "ok", "method": method}

        elif method == "terminate":
            self.inst.terminate()
            return {"status": "ok", "method": method}

        elif method == "delete":
            self.inst.delete()
            return {"status": "ok", "method": method}

        # ---------- shutdown ----------
        elif method == "shutdown":
            return {"status": "shutdown"}

        return {"status": "failed", "method": method}

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

        print(
            f"Retrieving FMU info for "
            f"{element.name}, {element.fmuPath}, {fmu_inst}",
            flush=True
        )

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

        info = {
            field: getattr(fmu_inst, field, None)
            for field in fields
        }

        # Custom / renamed fields
        info["fmiKind"] = getattr(fmu_inst, "fmuType", None)
        info["path"] = str(getattr(fmu_inst, "fmuPath", ""))

        return info

    def serializeElement(self, element):
      node = {
          "name": str(element._name) if isinstance(element, System) else str(element.name),
          "type": "system" if isinstance(element, System) else "component",
          "elements": []
      }

      node["connectors"] = self.serializeConnectors(element)

      if isinstance(element, Component):
        node["geometry"] = self.serializeElementGeometry(element)
        print(f"Serialized component {element.name} with geometry: {self.model.resources} , {element.name} , {element.fmuPath}", flush=True)
        #self.getFMUInfo(element)
        node["fmuInfo"] = self.getFMUInfo(element)

      # SYSTEM → recurse into dict
      if isinstance(element, System):
        for key, child in element.elements.items():
          node["elements"].append(self.serializeElement(child))

      return node

    # -----------------------------------
    # close
    # -----------------------------------
    def close(self):
        print("OMS server shutting down", flush=True)
        self._socket.close()
        self._context.term()

def _main():
  # parse command-line arguments
  parser = argparse.ArgumentParser(description='OMS-SERVER', allow_abbrev=False)
  parser.add_argument('--endpoint-rep', default=None, help='define the endpoint used for the interactive simulation communication')
  args = parser.parse_args()
  server = OMSServer(args.endpoint_rep)
  server.run()


if __name__ == "__main__":
    _main()

# OMEdit OMS ZMQ JSON migration status:
# - getElementsJson via ZMQ works.
# - Python getElements returns JSON:
#   Root system with child components.
# - LibraryTreeItem stores QJsonObject in mpOMSElementJson.
# - createLibraryTreeItemArun / createOMSLibraryTreeItemImpl(QJsonObject) creates tree items from JSON.
# - reLoadOMSimulatorModel was changed to use createLibraryTreeItemArun to avoid old oms_element_t overload.
# - ModelWidget::drawOMSModelDiagramElements was changed to draw JSON components without requiring getOMSElement()->geometry.
# - Python serializeElementGeometry now always returns default geometry.
# - LibraryTreeItem::getOMSElementGeometry reads geometry from mpOMSElementJson["geometry"].
# - FMU now draws from JSON geometry.
# - Current issue: moving FMU resets position because moved geometry is not written back to Python/ZMQ model yet.

# Next steps:
# 1. Update Element::updatePlacementAnnotation() to call setElementGeometry for JSON OMS elements.
# 2. Update OMSProxy::setElementGeometry(...) to send ZMQ method "setElementGeometry".
# 3. Add Python OMSimulatorCommand.py handler for "setElementGeometry".
# 4. Handler should update element.elementgeometry.
