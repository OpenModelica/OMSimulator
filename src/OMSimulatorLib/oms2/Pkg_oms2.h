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
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS2_PKG_OMS2_H_
#define _OMS2_PKG_OMS2_H_

/************************************************************
 Pkg_oms2 package header
 ************************************************************/

/* Package dependency header include                        */

namespace oms2
{
  /**
   * \brief Supported FMI co-simulation master algorithms.
   */
  enum class MasterAlgorithm
  {
    STANDARD, //!< Single-Core default algorithm
    PCTPL, //!< Parallel doStep(..) using CTPL task pool library (https://github.com/vit-vit/CTPL)
    PMRCHANNELA, //!< Experimental parallel (multi-rate) communication channel approach using ATOMIC variables and polling for synchronization
    PMRCHANNELCV, //!< Experimental parallel (multi-rate) communication channel approach using CONDITION variables for synchronization
    PMRCHANNELM, //!< Experimental parallel (multi-rate) communication channel approach using MUTEXES for synchronization
    // PMRTASKPOOLA, //!< Experimental parallel (multi-rate) approach using fixed number of tasks and ATOMIC variables and polling for synchronization
    // PMRTASKPOOLCV //!< Experimental parallel (multi-rate) approach using fixed number of tasks and CONDITION variables for synchronization
  };

  /**
   * \brief Type of rate transition in multi-rate communication.
   */
  enum class RateTransition
  {
    FASTTOSLOW, //!< The clock of output y is faster than that of input u
    SLOWTOFAST, //!< The clock of output y is slower than that of input u
    SAME        //!< The clocks of y and u run at the same rate.
  };

  /**
   * \brief Experimental class for playing with basic multi-rate stuff.
   *
   * A final implementation might approach this quite differently...
   */
  struct Experimental_Clock
  {
    int k = 1; //!< activation ratio (default 1).
  };

} // Types defined within the package of namespace oms2

#endif /* _OMS2_PKG_OMS2_H_ */
