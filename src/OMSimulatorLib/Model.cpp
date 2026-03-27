/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "Model.h"

#include "Component.h"
#include "CSVWriter.h"
#include "Flags.h"
#include "MATWriter.h"
#include "OMSFileSystem.h"
#include "OMSString.h"
#include "Scope.h"
#include "ssd/Tags.h"
#include "System.h"

#include "minizip.h"
#include <thread>
#include <algorithm> /* std::unique and std::find are defined here */

oms::Model::Model(const oms::ComRef& cref, const std::string& tempDir)
  : cref(cref), tempDir(tempDir), resultFilename(std::string(cref) + "_res.mat")
{
  if (!Flags::SuppressPath())
    logInfo("New model \"" + std::string(cref) + "\" with corresponding temp directory \"" + tempDir + "\"");

  elements.push_back(NULL);
  elements.push_back(NULL);
}

oms::Model::~Model()
{
  terminate();

  if (system)
    delete system;

  for (auto const & variant : ssdVariants)
    free(variant.second);

  // delete temp directory
  if (Flags::DeleteTempFiles())
  {
    if (!tempDir.empty() && filesystem::is_directory(tempDir))
    {
      try
      {
        filesystem::remove_all(tempDir);
        logDebug("removed temp directory: \"" + tempDir + "\"");
      }
      catch (const std::exception& e)
      {
        logWarning("temp directory \"" + tempDir + "\" couldn't be removed\n" + e.what());
      }
    }
  }
}

oms::Model* oms::Model::NewModel(const oms::ComRef& cref)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  filesystem::path tempDir = (filesystem::path(Scope::GetInstance().getTempDirectory().c_str()) / oms_unique_path(std::string(cref))).string();
  if (filesystem::is_directory(tempDir))
  {
    logError("Unique temp directory does already exist. Clean up the temp directory \"" + Scope::GetInstance().getTempDirectory() + "\" and try again.");
    return NULL;
  }
  if (!filesystem::create_directory(tempDir))
  {
    logError("Failed to create unique temp directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }
  if (!filesystem::create_directory(tempDir / "temp"))
  {
    logError("Failed to create temp directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }
  if (!filesystem::create_directory(tempDir / "resources"))
  {
    logError("Failed to create resources directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }

  oms::Model* model = new oms::Model(cref, tempDir.string());
  return model;
}

oms_status_enu_t oms::Model::rename(const oms::ComRef& cref)
{
  if (!cref.isValidIdent())
    return logError(std::string(cref) + " is not a valid ident");

  this->cref = cref;

  if (system)
  {
    system->renameConnectors();
    for (const auto & it:system->getSubSystems())
      it.second->renameConnectors();
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Model::rename(const ComRef& cref, const ComRef& newCref)
{
  if (!newCref.isValidIdent())
    return logError(std::string(newCref) + " is not a valid ident");

  if (!system)
    return logError("Model \"" + std::string(getCref()) + "\" does not contain any system");

  // renaming the model
  if (cref.isEmpty())
    return this->rename(newCref);

  // renaming any subcomponent
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (system->getCref() == front)
    return system->rename(tail, newCref);

  return logError("Model \"" + std::string(getCref()) + "\" does not contain system \"" + std::string(front) + "\"");
}

oms_status_enu_t oms::Model::loadSnapshot(const pugi::xml_node& node)
{
  // This method will not change the name of the model.
  // If a renaming is requested then it will happen in Scope::loadSnapshot.

  if (!validState(oms_modelState_virgin))
    return logError_ModelInWrongState(getCref());

  System* old_root_system = system;
  system = NULL;

  Snapshot snapshot; // this is a temporary workaroud, loadSnapshot will be removed later
  snapshot.importResourceNode(this->variantName, node);
  //snapshot.debugPrintAll();

  bool old_copyResources = copyResources();
  copyResources(false);
  oms_status_enu_t status = importFromSnapshot(snapshot);
  copyResources(old_copyResources);

  if (oms_status_ok != status)
  {
    system = old_root_system;
    return logError("loading snapshot failed");
  }

  if (old_root_system)
  {
    delete old_root_system;
    old_root_system = NULL;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Model::duplicateVariant(const ComRef& crefA, const ComRef& crefB)
{
  if (!crefA.isEmpty())
    return logError("only top level model is allowed");

  // copy the current snapshot
  char* fullsnapshot = NULL;
  exportSnapshot("", &fullsnapshot);

  Snapshot snapshot;
  snapshot.import(fullsnapshot);

  {
    auto it = ssdVariants.find(snapshot.getRootCref());
    if (it != ssdVariants.end())
    {
      free(ssdVariants[snapshot.getRootCref()]);
    }
  }
  ssdVariants[snapshot.getRootCref()] = fullsnapshot;

  // rename the model and all it components to new variant name
  oms::Scope::GetInstance().renameModel(getCref(), crefB);

  // set the current variantName and signalFilter name

  // check the variant name already exist in scope which means activating variant from memory
  auto it = ssdVariants.find(crefB);
  if (it != ssdVariants.end())
  {
    Snapshot variant;
    variant.import(it->second);
    // variant.debugPrintAll();
    this->variantName = variant.getSSDFilename();
    this->signalFilterFilename = variant.getSignalFilterFilename();
  }
  else
  {
    // variant does not exist, create a new variant
    this->variantName = std::string(crefB) + ".ssd";
    this->signalFilterFilename = "resources/signalFilter_" + std::string(crefB) + ".xml";
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Model::activateVariant(const ComRef& crefA, const ComRef& crefB)
{
  if (!crefA.isEmpty())
    return logError("only top level model is allowed");

  // copy the current snapshot for any updates done by user after activating the variant
  duplicateVariant(crefA, crefB);

  for (auto const & variant : ssdVariants)
  {
    Snapshot variants;
    variants.import(variant.second);
    if (variants.getRootCref() == crefB)
    {
      /* copy the resources to variants, as snapshot contains only .ssd file,
        at this stage all the imported ssv and ssm and .xml resources are copied to
        variants, regardless of whether they are referenced in the ssd file or not, otherwise
        we have to iterate the whole ssd and find the referenced resouces which will be a overhead
      */
      for (const auto & file : importedResources)
        variants.importResourceFile("resources/"+ file, getTempDirectory());

      //variants.debugPrintAll();
      char *modifiedVariant = NULL;
      variants.writeDocument(&modifiedVariant);
      char * newCref = NULL;
      importSnapshot(modifiedVariant, &newCref);
      free(modifiedVariant);
      return oms_status_ok;
    }
  }

  return logError("ssdVariant \"" + std::string(crefB) + "\"" + " does not exist, use oms_listVariant() to see the available ssd variants");
}

oms_status_enu_t oms::Model::listVariants(const oms::ComRef& cref, char** contents)
{

  Snapshot allVariantSnapshot;

  // get the current variant
  char* fullsnapshot = NULL;
  exportSnapshot("", &fullsnapshot);
  Snapshot currentSnapshot;
  currentSnapshot.import(fullsnapshot);
  free(fullsnapshot);

  pugi::xml_node oms_Variants = allVariantSnapshot.getTemplateResourceNodeSSDVariants();

  pugi::xml_node oms_variant = oms_Variants.append_child("oms:variant");
  oms_variant.append_attribute("name") = currentSnapshot.getRootCref().c_str();

  //Snapshot variants;
  for (auto const & variant : ssdVariants)
  {
    //variants.import(variant.second);
    pugi::xml_node oms_variant = oms_Variants.append_child("oms:variant");
    oms_variant.append_attribute("name") = variant.first.c_str();
  }

  allVariantSnapshot.writeDocument(contents);

  return oms_status_ok;
}

oms_status_enu_t oms::Model::importSnapshot(const char* snapshot_, char** newCref)
{
  if (!validState(oms_modelState_virgin))
    return logError_ModelInWrongState(getCref());

  Snapshot snapshot;
  snapshot.import(snapshot_);
  //snapshot.debugPrintAll();

  // get the new root cref from the snapshot, this should be done here
  // at the top before importing the full snapshot, as the new cref
  // will be overwritten
  new_root_cref = snapshot.getRootCref();

  if (snapshot.isPartialSnapshot())
  {
    char* fullsnapshot;
    // get full snapshot
    exportSnapshot("", &fullsnapshot);
    snapshot.importPartialSnapshot(fullsnapshot);
    free(fullsnapshot);
  }

  // get ssd:SystemStructureDescription
  pugi::xml_node ssdNode = snapshot.getResourceNode(this->variantName);

  ComRef new_cref = ComRef(ssdNode.attribute("name").as_string());
  std::string ssdVersion = ssdNode.attribute("version").as_string();

  // check new_cref exists in scope
  if (new_cref != getCref() && Scope::GetInstance().getModel(new_cref))
    return logError("Renaming the model \"" + std::string(getCref()) + "\" to \"" + std::string(new_cref) + "\" failed because another model with the same name already exists in the scope.");

  if (ssdVersion != "Draft20180219" && ssdVersion != "1.0" && ssdVersion != "2.0")
    logWarning("Unknown SSD version: " + ssdVersion);

  System* old_root_system = system;
  system = NULL;

  bool old_copyResources = copyResources();
  copyResources(false);
  oms_status_enu_t status = importFromSnapshot(snapshot);
  copyResources(old_copyResources);

  if (oms_status_ok != status)
  {
    system = old_root_system;
    return logError("loading snapshot failed");
  }

  if (old_root_system)
  {
    delete old_root_system;
    old_root_system = NULL;
  }

  if (newCref)
    *newCref = (char*)new_root_cref.c_str();

  // rename model
  if (new_cref != getCref())
    Scope::GetInstance().renameModel(getCref(), new_cref);

  return oms_status_ok;
}

oms::System* oms::Model::getSystem(const oms::ComRef& cref)
{
  if (!system)
    return NULL;

  if (cref.isEmpty())
    return system;

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (this->system->getCref() == front)
    return system->getSystem(tail);

  return NULL;
}

oms::Component* oms::Model::getComponent(const oms::ComRef& cref)
{
  if (!system)
    return NULL;

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (this->system->getCref() == front)
    return system->getComponent(tail);

  return NULL;
}

oms_status_enu_t oms::Model::delete_(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (!system || front != system->getCref())
    return oms_status_error;

  if (tail.isEmpty())
  {
    delete system;
    system = NULL;
    return oms_status_ok;
  }
  else
    return system->delete_(tail);
}

oms_status_enu_t oms::Model::list(const oms::ComRef& cref, char** contents)
{
  struct xmlStringWriter : pugi::xml_writer
  {
    std::string result;
    virtual void write(const void* data, size_t size)
    {
      result += std::string(static_cast<const char*>(data), size);
    }
  };

  xmlStringWriter writer;
  pugi::xml_document doc;

  Snapshot snapshot;
  // check for toplevelSystem or Model to update parameterbindings in ssd
  bool isTopSystemOrModel = false;

  // list model
  if (cref.isEmpty())
  {
    isTopSystemOrModel = true;
    exportToSSD(snapshot);
    doc.append_copy(snapshot.getResourceNode(this->variantName));
  }
  else
  {
    // list system
    if (!system)
      return logError("Model \"" + std::string(getCref()) + "\" does not contain any system");
    System* subsystem = getSystem(cref);

    // check for topLevel System, to update parameterBindings in ssd
    if (cref.isValidIdent())
      isTopSystemOrModel = true;

    if (subsystem)
    {
      pugi::xml_node ssdNode = snapshot.getTemplateResourceNodeSSD(this->variantName, this->getCref());
      pugi::xml_node system_node = ssdNode.append_child(oms::ssp::Draft20180219::ssd::system);

      subsystem->exportToSSD(system_node, snapshot, this->variantName);
      doc.append_copy(snapshot.getResourceNode(this->variantName).first_child());
    }
    else
    {
      // list component
      Component* component = getComponent(cref);
      if (!component)
        return logError("error");

      pugi::xml_node ssdNode = snapshot.getTemplateResourceNodeSSD(this->variantName, this->getCref());
      pugi::xml_node system_node = ssdNode.append_child(oms::ssp::Draft20180219::ssd::system);

      component->exportToSSD(system_node, snapshot, this->variantName);
      doc.append_copy(snapshot.getResourceNode(this->variantName).first_child());
    }
  }

  doc.save(writer);
  *contents = mallocAndCopyString(writer.result);
  if (!*contents)
    return oms_status_fatal;

  return oms_status_ok;
}

oms_status_enu_t oms::Model::exportSnapshot(const oms::ComRef& cref, char** contents)
{
  Snapshot snapshot;

  exportToSSD(snapshot);
  exportSignalFilter(snapshot);

  if (cref.isEmpty())
    return snapshot.writeDocument(contents);

  // query for partial snapshot
  Snapshot partialSnapshot(true);
  snapshot.exportPartialSnapshot(cref, partialSnapshot);
  return partialSnapshot.writeDocument(contents);
}

oms_status_enu_t oms::Model::exportSSVTemplate(const oms::ComRef& cref, const std::string& filename)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  pugi::xml_document ssvdoc;
  // generate XML declaration for ssm file
  pugi::xml_node ssvDeclarationNode = ssvdoc.append_child(pugi::node_declaration);
  ssvDeclarationNode.append_attribute("version") = "1.0";
  ssvDeclarationNode.append_attribute("encoding") = "UTF-8";

  std::string extension = "";
  if (filename.length() > 4)
    extension = filename.substr(filename.length() - 4);

  if (extension != ".ssv")
    return logError("filename extension must be \".ssv\"; no other formats are supported");

  Snapshot snapshot;
  pugi::xml_node ssvNode = snapshot.getTemplateResourceNodeSSV("template.ssv", "modelDescriptionStartValues");

  for (const auto& component : system->getComponents())
  {
    // skip csv files for top level model and export the remaining fmus in the model
    if (cref.isEmpty())
    {
      if (component.second->getType() == oms_component_table)
        continue;
    }
    if(component.first == tail || cref.isEmpty() || cref.isValidIdent()) // allow querying single component or whole model
    {
      if (oms_status_ok != component.second->exportToSSVTemplate(ssvNode, snapshot))
      {
        return logError("export of ssv template failed for component " + std::string(system->getFullCref()+std::string(component.first)));
      }
    }
  }

  // check in subsystems
  ComRef tailA(tail);
  ComRef frontA = tailA.pop_front();

  //std::cout <<"\n subsystem: " << tailA.c_str() << "==>" << frontA.c_str() << "==>" << cref.c_str();
  for (const auto &subsytem : system->getSubSystems())
  {
    for (const auto &component : subsytem.second->getComponents())
    {
      // skip csv files for top level model and export the remaining fmus in the model
      if (cref.isEmpty())
      {
        if (component.second->getType() == oms_component_table)
          continue;
      }
      if (component.first == tailA || cref.isEmpty() || (system->getSystem(frontA) && tailA.isEmpty()))
      {
        if (oms_status_ok != component.second->exportToSSVTemplate(ssvNode, snapshot))
        {
          return logError("export of ssv template failed for component " + std::string(system->getFullCref() + std::string(component.first)));
        }
      }
    }
  }

  //ssvdoc.save(std::cout);
  ssvdoc.append_copy(snapshot.getResourceNode("template.ssv"));

  if (!ssvdoc.save_file(filename.c_str(), "  ", pugi::format_indent|pugi::format_indent_attributes, pugi::encoding_utf8))
    return logError("failed to export  \"" + filename + "\" (for model \"" + std::string(this->getCref()) + "\")");

  return oms_status_ok;
}

oms_status_enu_t oms::Model::exportSSMTemplate(const oms::ComRef& cref, const std::string& filename)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  pugi::xml_document ssmdoc;
  // generate XML declaration for ssm file
  pugi::xml_node ssmDeclarationNode = ssmdoc.append_child(pugi::node_declaration);
  ssmDeclarationNode.append_attribute("version") = "1.0";
  ssmDeclarationNode.append_attribute("encoding") = "UTF-8";

  std::string extension = "";
  if (filename.length() > 4)
    extension = filename.substr(filename.length() - 4);

  if (extension != ".ssm")
    return logError("filename extension must be \".ssm\"; no other formats are supported");

  Snapshot snapshot;
  pugi::xml_node ssmNode = snapshot.getTemplateResourceNodeSSM("template.ssm");

  for (const auto& component : system->getComponents())
  {
    if(component.first == tail || cref.isEmpty()) // allow querying single component or whole model
    {
      if (oms_status_ok != component.second->exportToSSMTemplate(ssmNode))
      {
        return logError("export of ssm template failed for component " + std::string(system->getFullCref()+std::string(component.first)));
      }
    }
  }

  //ssmdoc.save(std::cout);
  ssmdoc.append_copy(snapshot.getResourceNode("template.ssm"));

  if (!ssmdoc.save_file(filename.c_str(), "  ", pugi::format_indent|pugi::format_indent_attributes, pugi::encoding_utf8))
    return logError("failed to export  \"" + filename + "\" (for model \"" + std::string(this->getCref()) + "\")");

  return oms_status_ok;
}

/*
 * This function update the ParameterBindings in SSD, to link with a SSV file, (e.g)
 * <ssd:ParameterBindings>
 *     <ssd:ParameterBinding source="resources/import_export_parameters.ssv" />
 * </ssd:ParameterBindings>
 */
oms_status_enu_t oms::Model::updateParameterBindingsToSSD(pugi::xml_node& node, bool isTopSystemOrModel) const
{
  // check parameter bindings exist and export to ssv file and also update the ssd file with parameterBindings at the top level
  if (isTopSystemOrModel)
  {
    // update the ssd with the top level parameterBindings (e.g)  <ParameterBinding source="resources/ControlledTemperature.ssv">
    for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
    {
      if (std::string(it->name()) == oms::ssp::Draft20180219::ssd::elements) // insert the parameter bindings after top-level connectors node
      {
        pugi::xml_node node_parameters_bindings = node.insert_child_before(oms::ssp::Version1_0::ssd::parameter_bindings, *it);
        pugi::xml_node node_parameter_binding  = node_parameters_bindings.append_child(oms::ssp::Version1_0::ssd::parameter_binding);
        std::string ssvFileName = "resources/" + std::string(this->getCref()) + ".ssv";
        node_parameter_binding.append_attribute("source") = ssvFileName.c_str();
        break;
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Model::newResources(const oms::ComRef& cref)
{
  ComRef subCref(cref);
  const std::string fileName = subCref.pop_suffix();

  if (fileName.empty())
    return logError("resource file not provided for \"" + std::string(getCref()+cref) + "\", Provide a valid reference file eg: \"model.root:test1.ssv\"");

  std::string extension = "";
  if (fileName.length() > 4)
    extension = fileName.substr(fileName.length() - 4);

  if (extension != ".ssv")
    return logError("filename extension for \"" + std::string(getCref() + cref) + "\" must be \".ssv\", no other formats are supported");

  if (system)
    return system->newResources(subCref, fileName);

  return oms_status_ok;
}

oms_status_enu_t oms::Model::addResources(const oms::ComRef& cref, const std::string& path)
{
  filesystem::path path_ = oms_canonical(path);
  if (!filesystem::exists(path_))
    return logError("file does not exist: \"" + path + "\"");

  ComRef subCref(cref);
  std::string fileName = subCref.pop_suffix();

  // get the filename from path, if no name provided
  if (fileName.empty())
  {
    filesystem::path path_(path);
    fileName = path_.filename().generic_string();
  }

  // copy the file to temp directory
  filesystem::path temp_root(getTempDirectory());
  filesystem::path temp_temp = temp_root / "temp";
  filesystem::path relFMUPath = filesystem::path("resources/" + fileName);
  filesystem::path absFMUPath = temp_root / relFMUPath;
  oms_copy_file(filesystem::path(path), absFMUPath);
  // push to external resources
  externalResources.push_back("resources/"+ fileName);

  return oms_status_ok;
}

oms_status_enu_t oms::Model::referenceResources(const oms::ComRef& cref, const std::string& ssmFile)
{
  ComRef subCref(cref);
  std::string fileName = subCref.pop_suffix();

  if (fileName.empty())
    return logError("reference file not provided for \"" + std::string(getCref() + cref) + "\", hence switching reference file to a new \".ssv\" or \".ssm\" cannot be done.");

  std::string ssvExtension = "";
  if (fileName.length() > 4)
    ssvExtension = fileName.substr(fileName.length() - 4);

  if (ssvExtension != ".ssv")
    return logError("filename extension for \"" + std::string(getCref() + cref) + "\" must be \".ssv\", no other formats are supported");

  if (!ssmFile.empty())
  {
    std::string ssmExtension = "";
    if (ssmFile.length() > 4)
      ssmExtension = ssmFile.substr(ssmFile.length() - 4);
    if (ssmExtension != ".ssm")
      return logError("filename extension for \"" + ssmFile + "\" must be \".ssm\", no other formats are supported");
  }

  if (system)
    return system->newResources(subCref, fileName, ssmFile, true);

  return oms_status_ok;
}

oms_status_enu_t oms::Model::reduceSSV(const std::string& ssvfile, const std::string& ssmfile, const std::string& filepath)
{
  filesystem::path ssvpath_ = oms_canonical(ssvfile);
  if (!filesystem::exists(ssvfile))
    return logError("ssvfile does not exist: \"" + ssvfile + "\"");

  filesystem::path ssmpath_ = oms_canonical(ssmfile);
  if (!filesystem::exists(ssmfile))
    return logError("ssmfile does not exist: \"" + ssmfile + "\"");

  std::string ssvFilename = ssvpath_.filename().generic_string();
  std::string ssmFilename = ssmpath_.filename().generic_string();

  Snapshot snapshot;
  snapshot.importResourceFile(ssvFilename, ssvpath_.remove_filename());
  snapshot.importResourceFile(ssmFilename, ssmpath_.remove_filename());

  values.importFromSnapshot(snapshot, ssvFilename, ssmFilename);

  //snapshot.debugPrintAll();

  pugi::xml_document ssvdoc;
  // generate XML declaration for ssm file
  pugi::xml_node ssvDeclarationNode = ssvdoc.append_child(pugi::node_declaration);
  ssvDeclarationNode.append_attribute("version") = "1.0";
  ssvDeclarationNode.append_attribute("encoding") = "UTF-8";

  std::string savePath = "";
  if (filepath.empty())
    savePath = "reduced.ssv";
  else
    savePath = filepath;

  pugi::xml_node ssvNode = snapshot.getTemplateResourceNodeSSV(savePath, "reducedSSV");
  values.exportReduceSSV(ssvNode, this->getCref());
  ssvdoc.append_copy(snapshot.getResourceNode(savePath));

  if (!ssvdoc.save_file(savePath.c_str(), "  ", pugi::format_indent|pugi::format_indent_attributes, pugi::encoding_utf8))
    return logError("failed to export  \"" + savePath + "\" (for model \"" + std::string(this->getCref()) + "\")");

  return oms_status_ok;
}

oms_status_enu_t oms::Model::deleteReferencesInSSD(const oms::ComRef& cref)
{
  ComRef subCref(cref);
  std::string fileName = subCref.pop_suffix();

  if (fileName.empty())
    return logError("reference file not provided for \"" + std::string(getCref()+cref) + "\", hence deleting reference file cannot be done. Provide a valid reference file eg: \"model.root:test1.ssv\"");

  std::string extension = "";
  if (fileName.length() > 4)
    extension = fileName.substr(fileName.length() - 4);

  if (extension != ".ssv" && extension != ".ssm")
    return logError("filename extension for \"" + std::string(getCref() + cref) + "\" must be \".ssv\" or \".ssm\", no other formats are supported");

  if (system)
    return system->deleteReferencesInSSD(subCref, fileName);

  return oms_status_ok;
}

oms_status_enu_t oms::Model::deleteResourcesInSSP(const std::string & fileName)
{
  if (fileName.empty())
    return logError("reference file not provided for \"" + std::string(getCref()) + "\", hence deleting resource file cannot be done. Provide a valid reference file eg: \"model:test1.ssv\"");

  std::string extension = "";
  if (fileName.length() > 4)
    extension = fileName.substr(fileName.length() - 4);

  if (extension != ".ssv" && extension != ".ssm")
    return logError("filename extension for \"" + std::string(getCref()) + ":" + fileName + "\" must be \".ssv\" or \".ssm\", no other formats are supported");

  if (system)
    return system->deleteResourcesInSSP(fileName);

  return oms_status_ok;
}

oms_status_enu_t oms::Model::addSystem(const oms::ComRef& cref, oms_system_enu_t type)
{
  if (cref.isValidIdent() && !system)
  {
    system = System::NewSystem(cref, type, this, NULL);

    if (system)
    {
      elements[0] = system->getElement();
      return oms_status_ok;
    }
    return oms_status_error;
  }

  if (!system)
    return logError("Model \"" + std::string(getCref()) + "\" does not contain any system");

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (system->getCref() == front)
    return system->addSubSystem(tail, type);

  return logError("wrong input \"" + std::string(front) + "\" != \"" + std::string(system->getCref()) + "\"");
}

oms_status_enu_t oms::Model::exportToSSD(Snapshot& snapshot) const
{
  pugi::xml_node ssdNode;
  // check for variants
  if (this->variantName == "SystemStructure.ssd")
    ssdNode = snapshot.getTemplateResourceNodeSSD("SystemStructure.ssd", this->getCref());
  else
    ssdNode = snapshot.getTemplateResourceNodeSSD(this->variantName, ComRef(this->variantName).pop_front());

  if (system)
  {
    pugi::xml_node system_node = ssdNode.append_child(oms::ssp::Draft20180219::ssd::system);
    if (oms_status_ok != system->exportToSSD(system_node, snapshot, this->variantName))
      return logError("export of system failed");
  }
  exportEnumerationDefinitionsToSSD(ssdNode);
  exportUnitDefinitionsToSSD(ssdNode);

  pugi::xml_node default_experiment = ssdNode.append_child(oms::ssp::Draft20180219::ssd::default_experiment);
  default_experiment.append_attribute("startTime") = std::to_string(startTime).c_str();
  default_experiment.append_attribute("stopTime") = std::to_string(stopTime).c_str();

  // export openmodelica_default_experiment as vendor annotations
  pugi::xml_node node_annotations = default_experiment.append_child(oms::ssp::Draft20180219::ssd::annotations);
  pugi::xml_node node_annotation = node_annotations.append_child(oms::ssp::Version1_0::ssc::annotation);
  node_annotation.append_attribute("type") =  oms::ssp::Draft20180219::annotation_type;
  pugi::xml_node oms_annotation_node = node_annotation.append_child(oms::ssp::Version1_0::oms_annotations);
  pugi::xml_node oms_simulation_information = oms_annotation_node.append_child(oms::ssp::Version1_0::simulation_information);
  //pugi::xml_node oms_default_experiment = oms_simulation_information.append_child("DefaultExperiment");

  oms_simulation_information.append_attribute("resultFile") = resultFilename.c_str();
  oms_simulation_information.append_attribute("loggingInterval") = std::to_string(loggingInterval).c_str();
  oms_simulation_information.append_attribute("bufferSize") = std::to_string(bufferSize).c_str();
  // check for root system exist and export signalFilter
  if (system)
    oms_simulation_information.append_attribute("signalFilter") = signalFilterFilename.c_str();

  return oms_status_ok;
}

oms_status_enu_t oms::Model::importFromSnapshot(const Snapshot& snapshot)
{
  pugi::xml_node ssdNode = snapshot.getResourceNode(this->variantName);
  if (!ssdNode)
    return logError("loading <oms:file>\"" + this->variantName  + "\"" + "from <oms:snapshot> failed");

  std::string sspVersion = ssdNode.attribute("version").as_string();

  if(sspVersion == "Draft20180219")
  {
    logWarning_deprecated;
  }

  for(pugi::xml_node_iterator it = ssdNode.begin(); it != ssdNode.end(); ++it)
  {
    std::string name = it->name();
    if (name == oms::ssp::Draft20180219::ssd::system)
    {
      ComRef systemCref = ComRef(it->attribute("name").as_string());

      // lochel: I guess that can somehow be improved
      oms_system_enu_t systemType = getSystemType(*it, sspVersion);

      if (oms_status_ok != addSystem(systemCref, systemType))
        return logError("Unable to add System \"" + std::string(systemCref.c_str()) + "\"");

      System* system = getSystem(systemCref);
      if (!system)
        return oms_status_error;

      if (oms_status_ok != system->importFromSnapshot(*it, sspVersion, snapshot, this->variantName))
        return oms_status_error;
    }
    else if (name == oms::ssp::Draft20180219::ssd::units)
    {
      // allow importing unitDefinitions, the unitDefinitions are handled in Values.cpp importFromSnapshot
    }
    else if (name == oms::ssp::Draft20180219::ssd::enumerations)
    {
      // allow importing enumerations, the enumerationDefinitions are handled in Values.cpp importFromSnapshot
    }
    else if (name == oms::ssp::Draft20180219::ssd::default_experiment)
    {
      startTime = it->attribute("startTime").as_double(0.0);
      stopTime = it->attribute("stopTime").as_double(1.0);

      // import oms::DefaultExperiment from oms:simulationInformation
      pugi::xml_node annotations = it->child(oms::ssp::Draft20180219::ssd::annotations);
      pugi::xml_node annotation_node;
      annotation_node = annotations.child(oms::ssp::Version1_0::ssc::annotation);

      // check for ssd:annotation to support older version, which is a bug
      if(!annotation_node)
      {
        annotation_node = annotations.child(oms::ssp::Draft20180219::ssd::annotation);
        logWarning_deprecated;
      }

      if (annotation_node && std::string(annotation_node.attribute("type").as_string()) == oms::ssp::Draft20180219::annotation_type)
      {
        pugi::xml_node oms_annotation_node = annotation_node.child(oms::ssp::Version1_0::oms_annotations);
        // support older <ssc:annotation>
        if (!oms_annotation_node)
        {
          oms_annotation_node = annotation_node;
          logWarning_deprecated;
        }
        for(pugi::xml_node_iterator itAnnotations = oms_annotation_node.begin(); itAnnotations != oms_annotation_node.end(); ++itAnnotations)
        {
          name = itAnnotations->name();
          // check for oms_default_experiment from version 1.0
          if (std::string(name) == oms::ssp::Version1_0::simulation_information  && (sspVersion == "1.0" || sspVersion == "2.0"))
          {
            resultFilename = itAnnotations->attribute("resultFile").as_string();
            loggingInterval = itAnnotations->attribute("loggingInterval").as_double();
            bufferSize = itAnnotations->attribute("bufferSize").as_int();

            if (oms_status_ok == importSignalFilter(itAnnotations->attribute("signalFilter").as_string(), snapshot))
              this->signalFilterFilename = itAnnotations->attribute("signalFilter").as_string();
          }
        }
      }
    }
    else
      return logError("wrong xml schema detected");
  }

  return oms_status_ok;
}

oms_system_enu_t oms::Model::getSystemType(const pugi::xml_node& node, const std::string& sspVersion)
{
  oms_system_enu_t systemType = oms_system_none;
  for(pugi::xml_node_iterator itElements = node.begin(); itElements != node.end(); ++itElements)
  {
    std::string name = itElements->name();
    /*  To handle version = "Draft20180219"*/
    if (name ==  oms::ssp::Draft20180219::ssd::simulation_information && sspVersion == "Draft20180219")
    {
      systemType = getSystemTypeHelper(*itElements, sspVersion);
    }

    /* from Version "1.0" simulationInformation is handled in vendor annotation */
    if (name == oms::ssp::Draft20180219::ssd::annotations  && (sspVersion == "1.0" || sspVersion == "2.0"))
    {
      pugi::xml_node annotation_node;
      annotation_node = itElements->child(oms::ssp::Version1_0::ssc::annotation);

      // check for ssd:annotation to support older version, which is a bug
      if (!annotation_node)
      {
        annotation_node = itElements->child(oms::ssp::Draft20180219::ssd::annotation);
        logWarning_deprecated;
      }

      if (annotation_node && std::string(annotation_node.attribute("type").as_string()) == oms::ssp::Draft20180219::annotation_type)
      {
        pugi::xml_node oms_annotation_node;
        oms_annotation_node = annotation_node.child(oms::ssp::Version1_0::oms_annotations);
        // support older <ssc:annotation>
        if (!oms_annotation_node)
        {
          oms_annotation_node = annotation_node;
          logWarning_deprecated;
        }

        for(pugi::xml_node_iterator itAnnotations = oms_annotation_node.begin(); itAnnotations != oms_annotation_node.end(); ++itAnnotations)
        {
          std::string annotationName = itAnnotations->name();
          if (std::string(annotationName) == oms::ssp::Version1_0::simulation_information) // check for oms:simulationInformation from version 1.0
          {
            systemType = getSystemTypeHelper(*itAnnotations, sspVersion);
          }
        }
      }
    }
  }

  return systemType;
}

oms_system_enu_t oms::Model::getSystemTypeHelper(const pugi::xml_node& node, const std::string& sspVersion)
{
  oms_system_enu_t systemType = oms_system_wc;
  if (std::string(node.child(oms::ssp::Version1_0::VariableStepSolver).attribute("description").as_string()) != "" || std::string(node.child("VariableStepSolver").attribute("description").as_string()) !="")
  {
    systemType = oms_system_sc;
  }
  else if (std::string(node.child("oms:FixedStepSolver").attribute("description").as_string()) != "" || std::string(node.child("FixedStepSolver").attribute("description").as_string()) != "")
  {
    systemType = oms_system_sc;
  }
  else if (std::string(node.child(oms::ssp::Version1_0::VariableStepMaster).attribute("description").as_string()) != "" || std::string(node.child("VariableStepMaster").attribute("description").as_string()) != "")
  {
    systemType = oms_system_wc;
  }
  else if (std::string(node.child(oms::ssp::Version1_0::FixedStepMaster).attribute("description").as_string()) != "" || std::string(node.child("FixedStepMaster").attribute("description").as_string()) != "")
  {
    systemType = oms_system_wc;
  }

  return systemType;
}

oms_status_enu_t oms::Model::exportToFile(const std::string& filename) const
{

  Snapshot snapshot;

  std::string extension = "";
  if (filename.length() > 4)
    extension = filename.substr(filename.length() - 4);

  if (extension != ".ssp")
    return logError("filename extension must be \".ssp\"; no other formats are supported");

  exportToSSD(snapshot);
  exportSignalFilter(snapshot);

  // Usage: minizip [-o] [-a] [-0 to -9] [-p password] [-j] file.zip [files_to_add]
  //        -o  Overwrite existing file.zip
  //        -a  Append to existing file.zip
  //        -0  Store only
  //        -1  Compress faster
  //        -9  Compress better
  //        -j  exclude path. store only the file name

  std::vector<std::string> resources;
  writeAllResourcesToFilesystem(resources, snapshot);

  /*
   * remove duplicates resources which is basically a fmu with multiple instances
   * eg. tank1 => resources/001_tank1.fmu
   *     tank2 => resources/001_tank1.fmu
  */
  resources.erase(std::unique(resources.begin(), resources.end()), resources.end());

  std::string cd = Scope::GetInstance().getWorkingDirectory();
  Scope::GetInstance().setWorkingDirectory(tempDir);
  int argc = 4 + resources.size();
  char **argv = new char*[argc];
  int i=0;
  argv[i++] = (char*)"minizip";
  argv[i++] = (char*)"-o";
  argv[i++] = (char*)"-1";
  argv[i++] = (char*)"temp/model.ssp";
  for (const auto& file : resources)
    argv[i++]=(char*)file.c_str();
  minizip(argc, argv);
  delete[] argv;
  Scope::GetInstance().setWorkingDirectory(cd);

  filesystem::path full_path = filesystem::path(tempDir) / "temp/model.ssp";
  oms_copy_file(full_path, filesystem::path(filename));

  return oms_status_ok;
}

void oms::Model::writeAllResourcesToFilesystem(std::vector<std::string>& resources, Snapshot& snapshot) const
{
  // get all files from snapshot and save the document (eg.) ssd, ssv and signalFilter
  snapshot.getResources(resources);
  for (auto const &filename : resources)
  {
    if (oms_status_ok != snapshot.writeResourceNode(filename, filesystem::path(tempDir)))
      logError("failed to export \"" + filename + " to directory " + tempDir);
  }

  // get all the variants and its resources
  // TODO how to handle mutiple resouces with same file name (e.g) resources/signalFilter.xml and other ssv resouces
  for (auto const & variant : ssdVariants)
  {
    std::vector<std::string> variantResources;
    Snapshot snapshot_;
    snapshot_.import(variant.second);
    snapshot_.getResources(variantResources);
    //snapshot_.debugPrintAll();
    for (auto const &variantfilename : variantResources)
    {
      //std::cout << "\n variant resources : " << variantfilename;
      auto file = std::find(resources.begin(), resources.end(), variantfilename);
      if (file == resources.end())
      {
        resources.push_back(variantfilename);
        snapshot_.writeResourceNode(variantfilename, filesystem::path(tempDir));
      }
    }
  }

  // export the unreferenced ssv and ssm files to ssp, this must be extended to export all unreferenced resources
  for (auto & it : importedResources)
  {
    auto file = std::find(resources.begin(), resources.end(), "resources/"+ it);
    if (file == resources.end())
      resources.push_back("resources/"+ it); // export unreferenced resources
  }

  if (system)
    system->getAllResources(resources);

  // check for external resources added at model level
  if (!externalResources.empty())
  {
    for (const auto &file : externalResources)
    {
      resources.push_back(file);
    }
  }
}

oms_status_enu_t oms::Model::setStartTime(double value)
{
  if (!validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated))
    return logError_ModelInWrongState(getCref());

  startTime = value;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::setStopTime(double value)
{
  stopTime = value;
  return oms_status_ok;
}

double oms::Model::getTime() const
{
  if (system && validState(oms_modelState_simulation))
    return system->getTime();
  return getStartTime();
}

oms_status_enu_t oms::Model::instantiate()
{
  if (!validState(oms_modelState_virgin|oms_modelState_enterInstantiation))
    return logError_ModelInWrongState(getCref());

  if (!system)
    return logError("Model doesn't contain a system");

  // initialize thread pool
  int numThreads = Flags::NumProcs();
  if (numThreads != 1)
  {
    if (numThreads > std::thread::hardware_concurrency() || 0 == numThreads)
      numThreads = std::thread::hardware_concurrency();
    pool = new ctpl::thread_pool(numThreads);
    logInfo("Initialized thread pool for model " + std::string(getCref()) + " with " + std::to_string(numThreads) + " threads");
  }

  modelState = oms_modelState_enterInstantiation;
  if (oms_status_ok != system->instantiate())
    return oms_status_error;

  modelState = oms_modelState_instantiated;

  return oms_status_ok;
}

oms_status_enu_t oms::Model::initialize()
{
  if (!validState(oms_modelState_instantiated))
    return logError_ModelInWrongState(getCref());

  if (!system)
    return logError("Model doesn't contain a system");

  modelState = oms_modelState_initialization;
  clock.reset();
  clock.tic();

  lastEmit = startTime - 1.0;

  if (!resultFilename.empty())
  {
    std::string resulttype;
    if (resultFilename.length() > 4)
      resulttype = resultFilename.substr(resultFilename.length() - 4);

    if (".csv" == resulttype)
      resultFile = new CSVWriter(bufferSize);
    else if (".mat" == resulttype)
      resultFile = new MATWriter(bufferSize);
    else
    {
      modelState = oms_modelState_instantiated;
      clock.toc();
      return logError("Unsupported format of the result file: " + resultFilename);
    }
  }

  if (oms_status_ok != system->initialize())
  {
    terminate();
    clock.toc();
    return logError_Initialization(system->getFullCref());
  }

  if (resultFile)
  {
    logInfo("Result file: " + resultFilename + " (bufferSize=" + std::to_string(bufferSize) + ")");

    // add all signals
    if (oms_status_ok != registerSignalsForResultFile())
    {
      terminate();
      clock.toc();
      return logError_Initialization(system->getFullCref());
    }

    // create result file
    if (!resultFile->create(resultFilename, startTime, stopTime))
    {
      delete resultFile;
      resultFile = NULL;
      logError("Creating result file failed");
      terminate();
      clock.toc();
      return logError_Initialization(system->getFullCref());
    }

    // dump results
    emit(startTime, true);
  }
  else
    logInfo("No result file will be created");

  clock.toc();
  modelState = oms_modelState_simulation;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::simulate()
{
  clock.tic();
  if (!validState(oms_modelState_simulation))
  {
    clock.toc();
    return logError_ModelInWrongState(getCref());
  }

  if (!system)
  {
    clock.toc();
    return logError("Model doesn't contain a system");
  }

  oms_status_enu_t status = system->stepUntil(stopTime);
  emit(stopTime, true);
  clock.toc();
  return status;
}

oms_status_enu_t oms::Model::doStep()
{
  clock.tic();
  if (!validState(oms_modelState_simulation))
  {
    clock.toc();
    return logError_ModelInWrongState(getCref());
  }

  if (!system)
  {
    clock.toc();
    return logError("Model doesn't contain a system");
  }

  oms_status_enu_t status = system->doStep();
  clock.toc();
  return status;
}

oms_status_enu_t oms::Model::stepUntil(double stopTime)
{
  clock.tic();
  if (!validState(oms_modelState_simulation))
  {
    clock.toc();
    return logError_ModelInWrongState(getCref());
  }

  if (!system)
  {
    clock.toc();
    return logError("Model doesn't contain a system");
  }

  if (stopTime > getStopTime())
    setStopTime(stopTime);

  oms_status_enu_t status = system->stepUntil(stopTime);
  emit(stopTime, true);
  clock.toc();
  return status;
}

oms_status_enu_t oms::Model::terminate()
{
  if (validState(oms_modelState_virgin))
    return oms_status_ok;

  if (validState(oms_modelState_enterInstantiation))
    return logError_ModelInWrongState(getCref());

  if (!system)
    return logError("Model doesn't contain a system");

  if (oms_status_ok != system->terminate())
    return logError_Termination(system->getFullCref());

  if (useThreadPool())
  {
    delete pool;
    pool = nullptr;
  }

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }

  modelState = oms_modelState_virgin;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::reset()
{
  if (!validState(oms_modelState_simulation))
    return logError_ModelInWrongState(getCref());

  if (!system)
    return logError("Model doesn't contain a system");

  if (oms_status_ok != system->reset())
    return logError_ResetFailed(system->getFullCref());

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }

  modelState = oms_modelState_instantiated;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::setLoggingInterval(double loggingInterval)
{
  if (loggingInterval < 0.0)
    this->loggingInterval = 0.0;
  this->loggingInterval = loggingInterval;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::registerSignalsForResultFile()
{
  if (!resultFile)
    return oms_status_ok;

  if (system)
    if (oms_status_ok != system->registerSignalsForResultFile(*resultFile))
      return oms_status_error;

  for (auto &it : aliases)
  {
    const ComRef& alias = it.first;
    ComRef rhs;
    oms_signal_type_enu_t varType = oms_signal_type_real;
    int result_file_id = -1;
    std::tie(rhs, varType, result_file_id) = it.second;

    std::string description = std::string("Alias target: ") + std::string(rhs);
    oms::SignalType_t sigType;
    switch (std::get<1>(it.second))
    {
      case oms_signal_type_real:
        sigType = oms::SignalType_t::SignalType_REAL;
        break;
      case oms_signal_type_integer:
        sigType = oms::SignalType_t::SignalType_INT;
        break;
      case oms_signal_type_boolean:
        sigType = oms::SignalType_t::SignalType_BOOL;
        break;
      default:
        return logError("Unsupported signal type for alias \"" + std::string(alias) + "\" in model \"" + std::string(getCref()) + "\"");
    }

    result_file_id = resultFile->addSignal(alias, description, sigType);
    it.second = std::make_tuple(rhs, varType, result_file_id);
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Model::addAlias(const ComRef& alias, const ComRef& rhs, oms_signal_type_enu_t type)
{
  // check that alias does not already exist
  if (aliases.find(alias) != aliases.end())
    return logError("Alias \"" + std::string(alias) + "\" already exists in model \"" + std::string(getCref()) + "\"");

  // insert tuple (rhs, type, result_file_id) with result_file_id = -1 (not yet registered)
  aliases[alias] = std::make_tuple(rhs, type, -1);
  return oms_status_ok;
}

oms_status_enu_t oms::Model::emit(double time, bool force, bool* emitted)
{
  if (emitted)
    *emitted = false;

  if (!resultFile)
    return oms_status_ok;
  if (!force && time < lastEmit + loggingInterval)
    return oms_status_ok;
  if (!force && time <= lastEmit)
    return oms_status_ok;

  // Skip rhs of events if --emitEvents=true
  if (!Flags::EmitEvents() && lastEmit == time)
    return oms_status_ok;

  if (system)
    if (oms_status_ok != system->updateSignals(*resultFile))
      return oms_status_error;

  // emit alias variables
  for (const auto &it : aliases)
  {
    const ComRef& alias = it.first;
    ComRef rhs;
    oms_signal_type_enu_t varType;
    int result_file_id = -1;
    std::tie(rhs, varType, result_file_id) = it.second;

    if (result_file_id < 0)
      continue; // not registered in result file

    SignalValue_t value;
    oms_status_enu_t status = oms_status_ok;
    if (varType == oms_signal_type_real)
    {
      double v;
      status = system->getReal(rhs, v);
      if (status == oms_status_ok)
        value.realValue = v;
    }
    else if (varType == oms_signal_type_integer || varType == oms_signal_type_enum)
    {
      int v;
      status = system->getInteger(rhs, v);
      if (status == oms_status_ok)
        value.intValue = v;
    }
    else if (varType == oms_signal_type_boolean)
    {
      bool v;
      status = system->getBoolean(rhs, v);
      if (status == oms_status_ok)
        value.boolValue = v;
    }

    if (status == oms_status_ok)
      resultFile->updateSignal(static_cast<unsigned int>(result_file_id), value);
  }

  resultFile->emit(time);
  lastEmit = time;
  if (emitted)
    *emitted = true;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::setResultFile(const std::string& filename, int bufferSize)
{
  this->resultFilename = filename;
  this->bufferSize = bufferSize;

  bool createNewResultfile = (resultFile != NULL);

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }

  if (createNewResultfile && !resultFilename.empty())
  {
    std::string resulttype;
    if (resultFilename.length() > 4)
      resulttype = resultFilename.substr(resultFilename.length() - 4);

    if (".csv" == resulttype)
      resultFile = new CSVWriter(bufferSize);
    else if (".mat" == resulttype)
      resultFile = new MATWriter(bufferSize);
    else
      return logError("Unsupported format of the result file: " + resultFilename);

    logInfo("Result file: " + resultFilename + " (bufferSize=" + std::to_string(bufferSize) + ")");

    // add all signals
    registerSignalsForResultFile();

    // create result file
    if (!resultFile->create(resultFilename, startTime, stopTime))
    {
      delete resultFile;
      resultFile = NULL;
      return logError("Creating result file failed");
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Model::getResultFile(char** filename, int* bufferSize)
{
  *filename = (char*)this->resultFilename.c_str();
  *bufferSize = this->bufferSize;

  return oms_status_ok;
}

oms_status_enu_t oms::Model::addSignalsToResults(const char* regex)
{
  if (system)
    if (oms_status_ok != system->addSignalsToResults(regex))
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::removeSignalsFromResults(const char* regex)
{
  if (system)
    if (oms_status_ok != system->removeSignalsFromResults(regex))
      return oms_status_error;
  return oms_status_ok;
}

void oms::Model::exportSignalFilter(Snapshot& snapshot) const
{
  if (!system)
    return;

  pugi::xml_node oms_signalFilter = snapshot.getTemplateResourceNodeSignalFilter(signalFilterFilename);

  std::vector<oms::Connector> filteredSignals;
  system->getFilteredSignals(filteredSignals);

  for (auto const& signal : filteredSignals)
  {
    pugi::xml_node oms_variable = oms_signalFilter.append_child(oms::ssp::Version1_0::oms_Variable);
    oms_variable.append_attribute("name") = signal.getFullName().c_str();
    oms_variable.append_attribute("type") = signal.getTypeString().c_str();
    oms_variable.append_attribute("kind") = signal.getCausalityString().c_str();
  }
}

void oms::Model::exportUnitDefinitionsToSSD(pugi::xml_node& node) const
{
  if (!system)
    return;

  std::map<std::string, std::map<std::string, std::string>> unitDefinitions;
  for (const auto& component : system->getComponents())
    component.second->getFilteredUnitDefinitionsToSSD(unitDefinitions);

  if (unitDefinitions.empty())
    return;

  pugi::xml_node node_units = node.append_child(oms::ssp::Draft20180219::ssd::units);

  std::vector<std::string> unitList;
  for (const auto &it : unitDefinitions)
  {
    if (std::find(unitList.begin(), unitList.end(), it.first) == unitList.end()) // check for duplicates just in case
    {
      pugi::xml_node ssc_unit = node_units.append_child(oms::ssp::Version1_0::ssc::unit);
      ssc_unit.append_attribute("name") = it.first.c_str();
      pugi::xml_node ssc_base_unit = ssc_unit.append_child(oms::ssp::Version1_0::ssc::base_unit);
      for (const auto &baseunit : it.second)
      {
        ssc_base_unit.append_attribute(baseunit.first.c_str()) = baseunit.second.c_str();
      }
      unitList.push_back(it.first);
    }
  }
}

void oms::Model::exportEnumerationDefinitionsToSSD(pugi::xml_node& node) const
{
  if (!system)
    return;

  std::map<std::string, std::map<std::string, std::string>> enumerationDefinitions;
  for (const auto& component : system->getComponents())
    component.second->getFilteredEnumerationDefinitionsToSSD(enumerationDefinitions);

  if (enumerationDefinitions.empty())
    return;

  pugi::xml_node node_enumeration = node.append_child(oms::ssp::Draft20180219::ssd::enumerations);

  for (const auto &it : enumerationDefinitions)
  {
    pugi::xml_node ssc_enumeration = node_enumeration.append_child(oms::ssp::Version1_0::ssc::enumeration_type);
    ssc_enumeration.append_attribute("name") = it.first.c_str();
    for (const auto & item: it.second)
    {
      pugi::xml_node enumItem = ssc_enumeration.append_child(oms::ssp::Version1_0::ssc::enum_item);
      enumItem.append_attribute("name") = item.first.c_str();
      enumItem.append_attribute("value") = item.second.c_str();
    }
  }

}

std::string oms::Model::escapeSpecialCharacters(const std::string& regex)
{
  std::string escapedRegex;
  for (char c : regex)
  {
    /* https://github.com/OpenModelica/OMSimulator/issues/1320
       If the character is a special regex character, add a backslash before it
      (e.g) model.root.A.a[1] => model\.root\.A\.a\[1\]
            model.root.testArray.der(x)=>model\.root\.testArray\.der\(x\)
    */
    if (c == '.' || c == '[' || c == ']' || c == '(' || c == ')' ||
        c == '{' || c == '}' || c == '*' || c == '+' || c == '?' ||
        c == '^' || c == '$' || c == '|')
    {
      escapedRegex += '\\'; // Add escape character
    }
    escapedRegex += c; // Add the original character
  }
  return escapedRegex;
}

oms_status_enu_t oms::Model::importSignalFilter(const std::string& filename, const Snapshot& snapshot)
{

  if (filename.empty())
    return oms_status_ok;

  // check for system and do not import signalFilter if system == NULL
  if (!system)
    return oms_status_ok;

  if (".*" == filename) // avoid error messages for older ssp files
  {
    addSignalsToResults(".*");
    return oms_status_warning;
  }

  pugi::xml_node oms_signalfilter = snapshot.getResourceNode(filename);

  if (!oms_signalfilter)
    return oms_status_error;

  removeSignalsFromResults(".*"); // disable all signals
  for (pugi::xml_node_iterator it = oms_signalfilter.begin(); it != oms_signalfilter.end(); ++it)
  {
    if (std::string(it->name()) == oms::ssp::Version1_0::oms_Variable)
      addSignalsToResults(escapeSpecialCharacters(it->attribute("name").as_string()).c_str());
  }

  return oms_status_ok;
}
