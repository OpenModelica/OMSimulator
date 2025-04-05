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

#pragma once

#include "OMSimulator/Types.h"

/* define OMSimulatorLib_EXPORTS *only* when building the DLL */
#if defined(OMS_STATIC)
  #define OMSAPI
  #define OMSCALL
#else
  #if defined(_MSC_VER) || defined(__MINGW32__)
    #ifdef OMSimulatorLib_EXPORTS
      #define OMSAPI __declspec(dllexport)
      #define OMSCALL __cdecl
    #else
      #define OMSAPI __declspec(dllimport)
      #define OMSCALL __cdecl
    #endif
  #else
    #define OMSAPI
    #define OMSCALL
  #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

OMSAPI void* OMSCALL oms3_modelJson(char* model_json_desc);
OMSAPI oms_status_enu_t OMSCALL oms3_instantiate(void* model);
OMSAPI oms_status_enu_t OMSCALL oms3_initialize(void* model);
OMSAPI oms_status_enu_t OMSCALL oms3_simulate(void* model);
OMSAPI oms_status_enu_t OMSCALL oms3_stepUntil(void* model, double stopTime);
OMSAPI oms_status_enu_t OMSCALL oms3_terminate(void* model);

#ifdef __cplusplus
}
#endif

#endif
