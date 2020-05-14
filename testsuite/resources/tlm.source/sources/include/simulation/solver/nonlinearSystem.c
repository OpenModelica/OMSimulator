/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THE BSD NEW LICENSE OR THE
 * GPL VERSION 3 LICENSE OR THE OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs: http://www.openmodelica.org or
 * http://www.ida.liu.se/projects/OpenModelica, and in the OpenModelica
 * distribution. GNU version 3 is obtained from:
 * http://www.gnu.org/copyleft/gpl.html. The New BSD License is obtained from:
 * http://www.opensource.org/licenses/BSD-3-Clause.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, EXCEPT AS
 * EXPRESSLY SET FORTH IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE
 * CONDITIONS OF OSMC-PL.
 *
 */

/*! \file nonlinearSystem.c
 */

#include <math.h>
#include <string.h>

#include "util/omc_error.h"
#include "nonlinearSystem.h"
#include "nonlinearValuesList.h"
#if !defined(OMC_MINIMAL_RUNTIME)
#include "kinsolSolver.h"
#include "nonlinearSolverHybrd.h"
#include "nonlinearSolverNewton.h"
#include "newtonIteration.h"
#endif
#include "nonlinearSolverHomotopy.h"
#include "simulation/simulation_info_json.h"
#include "simulation/simulation_runtime.h"
#include "simulation/solver/model_help.h"

/* for try and catch simulationJumpBuffer */
#include "meta/meta_modelica.h"

int check_nonlinear_solution(DATA *data, int printFailingSystems, int sysNumber);

int init_lambda_steps = 1;

struct dataNewtonAndHybrid
{
  void* newtonData;
  void* hybridData;
};

#if !defined(OMC_MINIMAL_RUNTIME)
#include "util/write_csv.h"
/*! \fn int initializeNLScsvData(DATA* data, NONLINEAR_SYSTEM_DATA* systemData)
 *
 *  This function initializes csv files for analysis propose.
 *
 *  \param [ref] [data]
 *  \param [ref] [systemData]
 */
int initializeNLScsvData(DATA* data, NONLINEAR_SYSTEM_DATA* systemData)
{
  struct csvStats* stats = (struct csvStats*) malloc(sizeof(struct csvStats));
  char buffer[100];
  sprintf(buffer, "%s_NLS%dStatsCall.csv", data->modelData->modelFilePrefix, (int)systemData->equationIndex);
  stats->callStats = omc_write_csv_init(buffer, ',', '"');

  sprintf(buffer, "%s_NLS%dStatsIter.csv", data->modelData->modelFilePrefix, (int)systemData->equationIndex);
  stats->iterStats = omc_write_csv_init(buffer, ',', '"');

  systemData->csvData = stats;

  return 0;
}
#else
int initializeNLScsvData(DATA* data, NONLINEAR_SYSTEM_DATA* systemData)
{
  fprintf(stderr, "initializeNLScsvData not implemented for OMC_MINIMAL_RUNTIME");
  abort();
}
#endif

#if !defined(OMC_MINIMAL_RUNTIME)
/*! \fn int print_csvLineCallStatsHeader(OMC_WRITE_CSV* csvData)
 *
 *  This function initializes csv files for analysis propose.
 *
 *  \param [ref] [data]
 *  \param [ref] [systemData]
 */
int print_csvLineCallStatsHeader(OMC_WRITE_CSV* csvData)
{
  char buffer[1024];
  buffer[0] = 0;

  /* number of call */
  sprintf(buffer,"numberOfCall");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* simulation time */
  sprintf(buffer,"simulationTime");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solving iterations */
  sprintf(buffer,"iterations");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solving fCalls */
  sprintf(buffer,"numberOfFunctionCall");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solving Time */
  sprintf(buffer,"solvingTime");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solving Time */
  sprintf(buffer,"solvedSystem");
  omc_write_csv(csvData, buffer);

  /* finish line */
  fputc('\n',csvData->handle);

  return 0;
}

/*! \fn int print_csvLineCallStats(OMC_WRITE_CSV* csvData)
 *
 *  This function initializes csv files for analysis propose.
 *
 *  \param [ref] [csvData]
 *  \param [in] [number of calls]
 *  \param [in] [simulation time]
 *  \param [in] [iterations]
 *  \param [in] [number of function call]
 *  \param [in] [solving time]
 *  \param [in] [solved system]
 */
int print_csvLineCallStats(OMC_WRITE_CSV* csvData, int num, double time,
                           int iterations, int fCalls, double solvingTime,
                           int solved)
{
  char buffer[1024];

  /* number of call */
  sprintf(buffer, "%d", num);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* simulation time */
  sprintf(buffer, "%g", time);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solving iterations */
  sprintf(buffer, "%d", iterations);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solving fCalls */
  sprintf(buffer, "%d", fCalls);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solving Time */
  sprintf(buffer, "%f", solvingTime);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solved system */
  sprintf(buffer, "%s", solved?"TRUE":"FALSE");
  omc_write_csv(csvData, buffer);

  /* finish line */
  fputc('\n',csvData->handle);

  return 0;
}

/*! \fn int print_csvLineIterStatsHeader(OMC_WRITE_CSV* csvData)
 *
 *  This function initializes csv files for analysis propose.
 *
 *  \param [ref] [data]
 *  \param [ref] [systemData]
 */
int print_csvLineIterStatsHeader(DATA* data, NONLINEAR_SYSTEM_DATA* systemData, OMC_WRITE_CSV* csvData)
{
  char buffer[1024];
  int j;
  int size = modelInfoGetEquation(&data->modelData->modelDataXml, systemData->equationIndex).numVar;

  /* number of call */
  sprintf(buffer,"numberOfCall");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* solving iterations */
  sprintf(buffer,"iteration");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* variables x */
  for(j=0; j<size; ++j) {
    sprintf(buffer, "%s", modelInfoGetEquation(&data->modelData->modelDataXml, systemData->equationIndex).vars[j]);
    omc_write_csv(csvData, buffer);
    fputc(csvData->seperator,csvData->handle);
  }

  /* residuals */
  for(j=0; j<size; ++j) {
    sprintf(buffer, "r%d", j+1);
    omc_write_csv(csvData, buffer);
    fputc(csvData->seperator,csvData->handle);
  }

  /* delta x */
  sprintf(buffer,"delta_x");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* delta x scaled */
  sprintf(buffer,"delta_x_scaled");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* error in f */
  sprintf(buffer,"error_f");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* error in f scaled */
  sprintf(buffer,"error_f_scaled");
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* damping lambda */
  sprintf(buffer,"lambda");
  omc_write_csv(csvData, buffer);

  /* finish line */
  fputc('\n',csvData->handle);

  return 0;
}

/*! \fn int print_csvLineIterStatsHeader(OMC_WRITE_CSV* csvData)
 *
 *  This function initializes csv files for analysis propose.
 *
 *  \param [ref] [csvData]
 *  \param [in] [size, num, ...]
 */
int print_csvLineIterStats(void* voidCsvData, int size, int num,
                           int iteration, double* x, double* f, double error_f,
                           double error_fs, double delta_x, double delta_xs,
                           double lambda)
{
  OMC_WRITE_CSV* csvData = voidCsvData;
  char buffer[1024];
  int j;

  /* number of call */
  sprintf(buffer, "%d", num);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* simulation time */
  sprintf(buffer, "%d", iteration);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* x */
  for(j=0; j<size; ++j) {
    sprintf(buffer, "%g", x[j]);
    omc_write_csv(csvData, buffer);
    fputc(csvData->seperator,csvData->handle);
  }

  /* r */
  for(j=0; j<size; ++j) {
    sprintf(buffer, "%g", f[j]);
    omc_write_csv(csvData, buffer);
    fputc(csvData->seperator,csvData->handle);
  }

  /* error_f */
  sprintf(buffer, "%g", error_f);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* error_f */
  sprintf(buffer, "%g", error_fs);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* delta_x */
  sprintf(buffer, "%g", delta_x);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* delta_xs */
  sprintf(buffer, "%g", delta_xs);
  omc_write_csv(csvData, buffer);
  fputc(csvData->seperator,csvData->handle);

  /* lambda */
  sprintf(buffer, "%g", lambda);
  omc_write_csv(csvData, buffer);

  /* finish line */
  fputc('\n',csvData->handle);

  return 0;
}
#endif

/*! \fn int initializeNonlinearSystems(DATA *data)
 *
 *  This function allocates memory for all nonlinear systems.
 *
 *  \param [ref] [data]
 */
int initializeNonlinearSystems(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  int i,j;
  int size, nnz;
  NONLINEAR_SYSTEM_DATA *nonlinsys = data->simulationInfo->nonlinearSystemData;
  struct dataNewtonAndHybrid *mixedSolverData;

  infoStreamPrint(LOG_NLS, 1, "initialize non-linear system solvers");
  infoStreamPrint(LOG_NLS, 0, "%ld non-linear systems", data->modelData->nNonLinearSystems);

  for(i=0; i<data->modelData->nNonLinearSystems; ++i)
  {
    size = nonlinsys[i].size;
    nonlinsys[i].numberOfFEval = 0;
    nonlinsys[i].numberOfIterations = 0;

    /* check if residual function pointer are valid */
    assertStreamPrint(threadData, ((0 != nonlinsys[i].residualFunc)) || ((nonlinsys[i].strictTearingFunctionCall != NULL) ? (0 != nonlinsys[i].strictTearingFunctionCall) : 0), "residual function pointer is invalid" );

    /* check if analytical jacobian is created */
    if(nonlinsys[i].jacobianIndex != -1)
    {
      assertStreamPrint(threadData, 0 != nonlinsys[i].analyticalJacobianColumn, "jacobian function pointer is invalid" );
      if(nonlinsys[i].initialAnalyticalJacobian(data, threadData))
      {
        nonlinsys[i].jacobianIndex = -1;
      }
    }

#if !defined(OMC_MINIMAL_RUNTIME)
    if (nonlinsys[i].isPatternAvailable)
    {
      nnz = nonlinsys[i].sparsePattern.numberOfNoneZeros;

      if(nnz/(double)(size*size)<=nonlinearSparseSolverMaxDensity && size >= nonlinearSparseSolverMinSize)
      {
        data->simulationInfo->nlsMethod = NLS_KINSOL;
        infoStreamPrint(LOG_STDOUT, 0, "Using sparse solver kinsol for nonlinear system %d,\nbecause density of %.2f remains under threshold of %.2f and size of %d exceeds threshold of %d.\nThe maximum density and the minimal system size for using sparse solvers can be specified\nusing the runtime flags '<-nlsMaxDensity=value>' and '<-nlsMinSize=value>'.", i, nnz/(double)(size*size), nonlinearSparseSolverMaxDensity, size, nonlinearSparseSolverMinSize);
      }
    }
#endif

    /* allocate system data */
    nonlinsys[i].nlsx = (double*) malloc(size*sizeof(double));
    nonlinsys[i].nlsxExtrapolation = (double*) malloc(size*sizeof(double));
    nonlinsys[i].nlsxOld = (double*) malloc(size*sizeof(double));
    nonlinsys[i].resValues = (double*) malloc(size*sizeof(double));

    /* allocate value list*/
    nonlinsys[i].oldValueList = (void*) allocValueList(1);

    nonlinsys[i].lastTimeSolved = 0.0;

    nonlinsys[i].nominal = (double*) malloc(size*sizeof(double));
    nonlinsys[i].min = (double*) malloc(size*sizeof(double));
    nonlinsys[i].max = (double*) malloc(size*sizeof(double));
    nonlinsys[i].initializeStaticNLSData(data, threadData, &nonlinsys[i]);

#if !defined(OMC_MINIMAL_RUNTIME)
    /* csv data call stats*/
    if (data->simulationInfo->nlsCsvInfomation)
    {
      if (initializeNLScsvData(data, &nonlinsys[i]))
      {
        throwStreamPrint(threadData, "csvData initialization failed");
      }
      else
      {
        print_csvLineCallStatsHeader(((struct csvStats*) nonlinsys[i].csvData)->callStats);
        print_csvLineIterStatsHeader(data, &nonlinsys[i], ((struct csvStats*) nonlinsys[i].csvData)->iterStats);
      }
    }
#endif

    /* allocate solver data */
    switch(data->simulationInfo->nlsMethod)
    {
#if !defined(OMC_MINIMAL_RUNTIME)
    case NLS_HYBRID:
      allocateHybrdData(size, &nonlinsys[i].solverData);
      break;
    case NLS_KINSOL:
      nlsKinsolAllocate(size, &nonlinsys[i], data->simulationInfo->nlsLinearSolver);
      break;
    case NLS_NEWTON:
      allocateNewtonData(size, &nonlinsys[i].solverData);
      break;
#endif
    case NLS_HOMOTOPY:
      allocateHomotopyData(size, &nonlinsys[i].solverData);
      break;
#if !defined(OMC_MINIMAL_RUNTIME)
    case NLS_MIXED:
      mixedSolverData = (struct dataNewtonAndHybrid*) malloc(sizeof(struct dataNewtonAndHybrid));
      allocateHomotopyData(size, &(mixedSolverData->newtonData));

      allocateHybrdData(size, &(mixedSolverData->hybridData));

      nonlinsys[i].solverData = (void*) mixedSolverData;

      break;
#endif
    default:
      throwStreamPrint(threadData, "unrecognized nonlinear solver");
    }
  }

  messageClose(LOG_NLS);

  TRACE_POP
  return 0;
}

/*! \fn int updateStaticDataOfNonlinearSystems(DATA *data)
 *
 *  This function allocates memory for all nonlinear systems.
 *
 *  \param [ref] [data]
 */
int updateStaticDataOfNonlinearSystems(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  int i;
  int size;
  NONLINEAR_SYSTEM_DATA *nonlinsys = data->simulationInfo->nonlinearSystemData;
  struct dataNewtonAndHybrid *mixedSolverData;

  infoStreamPrint(LOG_NLS, 1, "update static data of non-linear system solvers");

  for(i=0; i<data->modelData->nNonLinearSystems; ++i)
  {
    nonlinsys[i].initializeStaticNLSData(data, threadData, &nonlinsys[i]);
  }

  messageClose(LOG_NLS);

  TRACE_POP
  return 0;
}

/*! \fn freeNonlinearSystems
 *
 *  This function frees memory of nonlinear systems.
 *
 *  \param [ref] [data]
 */
int freeNonlinearSystems(DATA *data, threadData_t *threadData)
{
  TRACE_PUSH
  int i;
  NONLINEAR_SYSTEM_DATA* nonlinsys = data->simulationInfo->nonlinearSystemData;
  struct csvStats* stats;

  infoStreamPrint(LOG_NLS, 1, "free non-linear system solvers");

  for(i=0; i<data->modelData->nNonLinearSystems; ++i)
  {
    free(nonlinsys[i].nlsx);
    free(nonlinsys[i].nlsxExtrapolation);
    free(nonlinsys[i].nlsxOld);
    free(nonlinsys[i].resValues);
    free(nonlinsys[i].nominal);
    free(nonlinsys[i].min);
    free(nonlinsys[i].max);
    freeValueList(nonlinsys[i].oldValueList, 1);

#if !defined(OMC_MINIMAL_RUNTIME)
    if (data->simulationInfo->nlsCsvInfomation)
    {
      stats = nonlinsys[i].csvData;
      omc_write_csv_free(stats->callStats);
      omc_write_csv_free(stats->iterStats);
    }
#endif
    /* free solver data */
    switch(data->simulationInfo->nlsMethod)
    {
#if !defined(OMC_MINIMAL_RUNTIME)
    case NLS_HYBRID:
      freeHybrdData(&nonlinsys[i].solverData);
      free(nonlinsys[i].solverData);
      break;
    case NLS_KINSOL:
      nlsKinsolFree(&nonlinsys[i].solverData);
      break;
    case NLS_NEWTON:
      freeNewtonData(&nonlinsys[i].solverData);
      free(nonlinsys[i].solverData);
      break;
#endif
    case NLS_HOMOTOPY:
      freeHomotopyData(&nonlinsys[i].solverData);
      break;
#if !defined(OMC_MINIMAL_RUNTIME)
    case NLS_MIXED:
      freeHomotopyData(&((struct dataNewtonAndHybrid*) nonlinsys[i].solverData)->newtonData);
      freeHybrdData(&((struct dataNewtonAndHybrid*) nonlinsys[i].solverData)->hybridData);
      free(nonlinsys[i].solverData);
      break;
#endif
    default:
      throwStreamPrint(threadData, "unrecognized nonlinear solver");
    }
  }

  messageClose(LOG_NLS);

  TRACE_POP
  return 0;
}

/*! \fn printNonLinearSystemSolvingStatistics
 *
 *  This function prints memory for all non-linear systems.
 *
 *  \param [ref] [data]
 *         [in]  [sysNumber] index of corresponding non-linear system
 */
void printNonLinearSystemSolvingStatistics(DATA *data, int sysNumber, int logLevel)
{
  NONLINEAR_SYSTEM_DATA* nonlinsys = data->simulationInfo->nonlinearSystemData;
  infoStreamPrint(logLevel, 1, "Non-linear system %d of size %d solver statistics:", (int)nonlinsys[sysNumber].equationIndex, (int)nonlinsys[sysNumber].size);
  infoStreamPrint(logLevel, 0, " number of calls                : %ld", nonlinsys[sysNumber].numberOfCall);
  infoStreamPrint(logLevel, 0, " number of iterations           : %ld", nonlinsys[sysNumber].numberOfIterations);
  infoStreamPrint(logLevel, 0, " number of function evaluations : %ld", nonlinsys[sysNumber].numberOfFEval);
  infoStreamPrint(logLevel, 0, " number of jacobian evaluations : %ld", nonlinsys[sysNumber].numberOfJEval);
  infoStreamPrint(logLevel, 0, " time of jacobian evaluations   : %f", nonlinsys[sysNumber].jacobianTime);
  infoStreamPrint(logLevel, 0, " average time per call          : %f", nonlinsys[sysNumber].totalTime/nonlinsys[sysNumber].numberOfCall);
  infoStreamPrint(logLevel, 0, " total time                     : %f", nonlinsys[sysNumber].totalTime);
  messageClose(logLevel);
}

/*! \fn getInitialGuess
 *
 *  This function writes initial guess to nonlinsys->nlsx and nonlinsys->nlsOld.
 *
 *  \param [in]  [nonlinsys]
 *  \param [in]  [time] time for extrapolation
 *
 */
int getInitialGuess(NONLINEAR_SYSTEM_DATA *nonlinsys, double time)
{
  /* value extrapolation */
  printValuesListTimes((VALUES_LIST*)nonlinsys->oldValueList);
  /* if list is empty use current start values */
  if (listLen(((VALUES_LIST*)nonlinsys->oldValueList)->valueList)==0)
  {
    /* use old value if no values are stored in the list */
    memcpy(nonlinsys->nlsx, nonlinsys->nlsxOld, nonlinsys->size*(sizeof(double)));
  }
  else
  {
    /* get extrapolated values */
    getValues((VALUES_LIST*)nonlinsys->oldValueList, time, nonlinsys->nlsxExtrapolation, nonlinsys->nlsxOld);
    memcpy(nonlinsys->nlsx, nonlinsys->nlsxOld, nonlinsys->size*(sizeof(double)));
  }

  return 0;
}

/*! \fn updateInitialGuessDB
 *
 *  This function writes new values to solution list.
 *
 *  \param [in]  [nonlinsys]
 *  \param [in]  [time] time for extrapolation
 *  \param [in]  [context] current context of evaluation
 *
 */
int updateInitialGuessDB(NONLINEAR_SYSTEM_DATA *nonlinsys, double time, int context)
{
  /* write solution to oldValue list for extrapolation */
  if (nonlinsys->solved == 1)
  {
    /* do not use solution of jacobian for next extrapolation */
    if (context < 4)
    {
      addListElement((VALUES_LIST*)nonlinsys->oldValueList,
              createValueElement(nonlinsys->size, time, nonlinsys->nlsx));
    }
  }
  else if (nonlinsys->solved == 2)
  {
    if (listLen(((VALUES_LIST*)nonlinsys->oldValueList)->valueList)>0)
    {
      cleanValueList((VALUES_LIST*)nonlinsys->oldValueList, NULL);
    }
    /* do not use solution of jacobian for next extrapolation */
    if (context < 4)
    {
      addListElement((VALUES_LIST*)nonlinsys->oldValueList,
              createValueElement(nonlinsys->size, time, nonlinsys->nlsx));
    }
  }
  messageClose(LOG_NLS_EXTRAPOLATE);
  return 0;
}

/*! \fn updateInnerEquation
 *
 *  This function updates inner equation with the current x.
 *
 *  \param [ref] [data]
 *         [in]  [sysNumber] index of corresponding non-linear system
 */
int updateInnerEquation(void **dataIn, int sysNumber, int discrete)
{
  DATA *data = (DATA*) ((void**)dataIn[0]);
  threadData_t *threadData = (threadData_t*) ((void**)dataIn[1]);

  NONLINEAR_SYSTEM_DATA* nonlinsys = &(data->simulationInfo->nonlinearSystemData[sysNumber]);
  int success = 0;
  int constraintViolated = 0;

  /* solve non continuous at discrete points*/
  if(discrete)
  {
    data->simulationInfo->solveContinuous = 0;
  }

  /* try */
#ifndef OMC_EMCC
  MMC_TRY_INTERNAL(simulationJumpBuffer)
#endif

  /* call residual function */
  if (nonlinsys->strictTearingFunctionCall != NULL)
    constraintViolated = nonlinsys->residualFuncConstraints((void*) dataIn, nonlinsys->nlsx, nonlinsys->resValues, (int*)&nonlinsys->size);
  else
    nonlinsys->residualFunc((void*) dataIn, nonlinsys->nlsx, nonlinsys->resValues, (int*)&nonlinsys->size);

  /* replace extrapolated values by current x for discrete step */
  memcpy(nonlinsys->nlsxExtrapolation, nonlinsys->nlsx, nonlinsys->size*(sizeof(double)));

  if (!constraintViolated)
    success = 1;
  /*catch */
#ifndef OMC_EMCC
  MMC_CATCH_INTERNAL(simulationJumpBuffer)
#endif

  if (!success && !constraintViolated)
  {
    warningStreamPrint(LOG_STDOUT, 0, "Non-Linear Solver try to handle a problem with a called assert.");
  }

  if(discrete)
  {
    data->simulationInfo->solveContinuous = 1;
  }

  return success;
}


int solveNLS(DATA *data, threadData_t *threadData, int sysNumber)
{
  int success = 0, constraintsSatisfied = 1;
  NONLINEAR_SYSTEM_DATA* nonlinsys = &(data->simulationInfo->nonlinearSystemData[sysNumber]);
  int casualTearingSet = nonlinsys->strictTearingFunctionCall != NULL;
  struct dataNewtonAndHybrid *mixedSolverData;

  /* use the selected solver for solving nonlinear system */
  switch(data->simulationInfo->nlsMethod)
  {
#if !defined(OMC_MINIMAL_RUNTIME)
  case NLS_HYBRID:
    success = solveHybrd(data, threadData, sysNumber);
    break;
  case NLS_KINSOL:
    success = nlsKinsolSolve(data, threadData, sysNumber);
    break;
  case NLS_NEWTON:
    success = solveNewton(data, threadData, sysNumber);
    /* check if solution process was successful, if not use alternative tearing set if available (dynamic tearing)*/
    if (!success && casualTearingSet){
      debugString(LOG_DT, "Solving the casual tearing set failed! Now the strict tearing set is used.");
      success = nonlinsys->strictTearingFunctionCall(data, threadData);
      if (success) success=2;
    }
    break;
#endif
  case NLS_HOMOTOPY:
    success = solveHomotopy(data, threadData, sysNumber);
    break;
#if !defined(OMC_MINIMAL_RUNTIME)
  case NLS_MIXED:
    mixedSolverData = nonlinsys->solverData;
    nonlinsys->solverData = mixedSolverData->newtonData;


    /* try to solve the system if it is the strict set, only try to solve the casual set if the constraints are satisfied */
    if ((!casualTearingSet) || constraintsSatisfied)
      success = solveHomotopy(data, threadData, sysNumber);


    /* check if solution process was successful, if not use alternative tearing set if available (dynamic tearing)*/
    if (!success && casualTearingSet){
      debugString(LOG_DT, "Solving the casual tearing set failed! Now the strict tearing set is used.");
      success = nonlinsys->strictTearingFunctionCall(data, threadData);
      if (success){
        success=2;
        /* update iteration variables of the casual set*/
        nonlinsys->getIterationVars(data, nonlinsys->nlsx);
      }
    }

    if (!success) {
      nonlinsys->solverData = mixedSolverData->hybridData;
      success = solveHybrd(data, threadData, sysNumber);
    }
    nonlinsys->solverData = mixedSolverData;
    break;
#endif
  default:
    throwStreamPrint(threadData, "unrecognized nonlinear solver");
  }

  return success;
}

/*! \fn solve non-linear systems
 *
 *  \param [in]  [data]
 *  \param [in]  [threadData]
 *  \param [in]  [sysNumber] index of corresponding non-linear system
 */
int solve_nonlinear_system(DATA *data, threadData_t *threadData, int sysNumber)
{
  void *dataAndThreadData[2] = {data, threadData};
  int success = 0, saveJumpState, constraintsSatisfied = 1;
  NONLINEAR_SYSTEM_DATA* nonlinsys = &(data->simulationInfo->nonlinearSystemData[sysNumber]);
  int casualTearingSet = nonlinsys->strictTearingFunctionCall != NULL;
  int step;
  int lambda_steps = 1;
  int j;
  char buffer[4096];
  FILE *pFile = NULL;

  data->simulationInfo->currentNonlinearSystemIndex = sysNumber;

  /* enable to avoid division by zero */
  data->simulationInfo->noThrowDivZero = 1;
  ((DATA*)data)->simulationInfo->solveContinuous = 1;

  /* performance measurement */
  rt_ext_tp_tick(&nonlinsys->totalTimeClock);

  /* grab the initial guess */
  infoStreamPrint(LOG_NLS_EXTRAPOLATE, 1, "############ Start new iteration for system %ld at time %g ############", nonlinsys->equationIndex, data->localData[0]->timeValue);
  /* if last solving is too long ago use just old values  */
  if (fabs(data->localData[0]->timeValue - nonlinsys->lastTimeSolved) < 5*data->simulationInfo->stepSize || casualTearingSet)
  {
    getInitialGuess(nonlinsys, data->localData[0]->timeValue);
  }
  else
  {
    nonlinsys->getIterationVars(data, nonlinsys->nlsx);
    memcpy(nonlinsys->nlsx, nonlinsys->nlsxOld, nonlinsys->size*(sizeof(double)));
  }
  /* update non continuous */
  if (data->simulationInfo->discreteCall)
  {
    constraintsSatisfied = updateInnerEquation(dataAndThreadData, sysNumber, 1);
  }

  /* try */
#ifndef OMC_EMCC
  MMC_TRY_INTERNAL(simulationJumpBuffer)
#endif

  /* handle asserts */
  saveJumpState = threadData->currentErrorStage;
  threadData->currentErrorStage = ERROR_NONLINEARSOLVER;

  if(data->simulationInfo->initial && nonlinsys->homotopySupport && init_lambda_steps > 1)
    lambda_steps = init_lambda_steps;

#if !defined(OMC_NO_FILESYSTEM)
  if(lambda_steps > 1 && ACTIVE_STREAM(LOG_INIT))
  {
    sprintf(buffer, "%s_homotopy_nls_%d.csv", data->modelData->modelFilePrefix, sysNumber);
    infoStreamPrint(LOG_INIT, 0, "The homotopy path of system %d will be exported to %s.", sysNumber, buffer);
    pFile = fopen(buffer, "wt");
    fprintf(pFile, "%s,", "lambda");
    for(j=0; j<nonlinsys->size; ++j)
      fprintf(pFile, "%s,", modelInfoGetEquation(&data->modelData->modelDataXml, nonlinsys->equationIndex).vars[j]);
    fprintf(pFile, "\n");
  }
#endif

  for(step=0; step<lambda_steps-1; ++step)
  {
    data->simulationInfo->lambda = ((double)step)/(lambda_steps-1);
    infoStreamPrint(LOG_INIT, 0, "[system %d] homotopy parameter lambda = %g", sysNumber, data->simulationInfo->lambda);
    solveNLS(data, threadData, sysNumber);

#if !defined(OMC_NO_FILESYSTEM)
    if(ACTIVE_STREAM(LOG_INIT))
    {
      infoStreamPrint(LOG_INIT, 0, "[system %d] homotopy parameter lambda = %g done\n---------------------------", sysNumber, data->simulationInfo->lambda);
      fprintf(pFile, "%.16g,", data->simulationInfo->lambda);
      for(j=0; j<nonlinsys->size; ++j)
        fprintf(pFile, "%.16g,", nonlinsys->nlsx[j]);
      fprintf(pFile, "\n");
    }
#endif
  }

  data->simulationInfo->lambda = 1.0;
  /* SOLVE! */
  nonlinsys->solved = solveNLS(data, threadData, sysNumber);

#if !defined(OMC_NO_FILESYSTEM)
  if(lambda_steps > 1 && ACTIVE_STREAM(LOG_INIT))
  {
    infoStreamPrint(LOG_INIT, 0, "[system %d] homotopy parameter lambda = %g done\n---------------------------", sysNumber, data->simulationInfo->lambda);
    fprintf(pFile, "%.16g,", data->simulationInfo->lambda);
    for(j=0; j<nonlinsys->size; ++j)
      fprintf(pFile, "%.16g,", nonlinsys->nlsx[j]);
    fprintf(pFile, "\n");
    fclose(pFile);
  }
#endif

  /* handle asserts */
  threadData->currentErrorStage = saveJumpState;

  /*catch */
#ifndef OMC_EMCC
  MMC_CATCH_INTERNAL(simulationJumpBuffer)
#endif

  /* update value list database */
  updateInitialGuessDB(nonlinsys, data->localData[0]->timeValue, data->simulationInfo->currentContext);
  if (nonlinsys->solved == 1)
  {
    nonlinsys->lastTimeSolved = data->localData[0]->timeValue;
  }


  /* enable to avoid division by zero */
  data->simulationInfo->noThrowDivZero = 0;
  ((DATA*)data)->simulationInfo->solveContinuous = 0;

  /* performance measurement and statistics */
  nonlinsys->totalTime += rt_ext_tp_tock(&(nonlinsys->totalTimeClock));
  nonlinsys->numberOfCall++;

  /* write csv file for debugging */
#if !defined(OMC_MINIMAL_RUNTIME)
  if (data->simulationInfo->nlsCsvInfomation)
  {
    print_csvLineCallStats(((struct csvStats*) nonlinsys->csvData)->callStats,
                           nonlinsys->numberOfCall,
                           data->localData[0]->timeValue,
                           nonlinsys->numberOfIterations,
                           nonlinsys->numberOfFEval,
                           nonlinsys->totalTime,
                           nonlinsys->solved
    );
  }
#endif
  return check_nonlinear_solution(data, 1, sysNumber);
}

/*! \fn check_nonlinear_solutions
 *
 *   This function check whether some of non-linear systems
 *   are failed to solve. If one is failed it returns 1 otherwise 0.
 *
 *  \param [in]  [data]
 *  \param [in]  [printFailingSystems]
 *  \param [out] [returnValue] It returns >0 if fail otherwise 0.
 *
 *  \author wbraun
 */
int check_nonlinear_solutions(DATA *data, int printFailingSystems)
{
  long i;

  for(i=0; i<data->modelData->nNonLinearSystems; ++i) {
     if(check_nonlinear_solution(data, printFailingSystems, i))
       return 1;
  }

  return 0;
}

/*! \fn check_nonlinear_solution
 *
 *   This function checks if a non-linear system
 *   is solved. Returns a warning and 1 in case it's not
 *   solved otherwise 0.
 *
 *  \param [in]  [data]
 *  \param [in]  [printFailingSystems]
 *  \param [in]  [sysNumber] index of corresponding non-linear System
 *  \param [out] [returnValue] Returns 1 if fail otherwise 0.
 *
 *  \author wbraun
 */
int check_nonlinear_solution(DATA *data, int printFailingSystems, int sysNumber)
{
  NONLINEAR_SYSTEM_DATA* nonlinsys = data->simulationInfo->nonlinearSystemData;
  long j;
  int i = sysNumber;

  if(nonlinsys[i].solved == 0)
  {
    int index = nonlinsys[i].equationIndex, indexes[2] = {1,index};
    if (!printFailingSystems) return 1;
    warningStreamPrintWithEquationIndexes(LOG_NLS, 1, indexes, "nonlinear system %d fails: at t=%g", index, data->localData[0]->timeValue);
    if(data->simulationInfo->initial)
    {
      warningStreamPrint(LOG_NLS_V, 0, "proper start-values for some of the following iteration variables might help");
    }
    for(j=0; j<modelInfoGetEquation(&data->modelData->modelDataXml, (nonlinsys[i]).equationIndex).numVar; ++j) {
      int done=0;
      long k;
      const MODEL_DATA *mData = data->modelData;
      for(k=0; k<mData->nVariablesReal && !done; ++k)
      {
        if (!strcmp(mData->realVarsData[k].info.name, modelInfoGetEquation(&data->modelData->modelDataXml, (nonlinsys[i]).equationIndex).vars[j]))
        {
        done = 1;
        warningStreamPrint(LOG_NLS_V, 0, "[%ld] Real %s(start=%g, nominal=%g)", j+1,
                                     mData->realVarsData[k].info.name,
                                     mData->realVarsData[k].attribute.start,
                                     mData->realVarsData[k].attribute.nominal);
        }
      }
      if (!done)
      {
        warningStreamPrint(LOG_NLS_V, 0, "[%ld] Real %s(start=?, nominal=?)", j+1, modelInfoGetEquation(&data->modelData->modelDataXml, (nonlinsys[i]).equationIndex).vars[j]);
      }
    }
    messageCloseWarning(LOG_NLS);
    return 1;
  }
  if(nonlinsys[i].solved == 2)
  {
    nonlinsys[i].solved = 1;
    return 2;
  }


  return 0;
}

/*! \fn cleanUpOldValueListAfterEvent
 *
 *   This function clean old value list up to parameter time for all
 *   non-linear systems.
 *
 *  \param [in]  [data]
 *  \param [in]  [time]
 *
 *  \author wbraun
 */
void cleanUpOldValueListAfterEvent(DATA *data, double time)
{
  long i;
  NONLINEAR_SYSTEM_DATA* nonlinsys = data->simulationInfo->nonlinearSystemData;

  for(i=0; i<data->modelData->nNonLinearSystems; ++i) {
    cleanValueListbyTime(nonlinsys[i].oldValueList, time);
  }
}

