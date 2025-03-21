ns = {
    "ssc": "http://ssp-standard.org/SSP1/SystemStructureCommon",
    "ssd": "http://ssp-standard.org/SSP1/SystemStructureDescription",
    "ssv": "http://ssp-standard.org/SSP1/SystemStructureParameterValues",
    "ssm": "http://ssp-standard.org/SSP1/SystemStructureParameterMapping",
    "ssb": "http://ssp-standard.org/SSP1/SystemStructureSignalDictionary",
    "oms": "https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
}

def tag(prefix, name):
  '''Helper method to generate a properly namespaced tag.'''
  return f"{{{ns[prefix]}}}{name}"
