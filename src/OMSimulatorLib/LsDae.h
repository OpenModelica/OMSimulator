/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS_LSDAE_H_
#define _OMS_LSDAE_H_

#include <cstdint>
#include <string>
#include <vector>

namespace oms
{
  /**
   * \brief fmi-ls-dae: what an FMU's layered-standard manifest says about its
   *        DAE formulation.
   *
   * A Model Exchange FMU that carries
   * `extra/org.fmi-standard.fmi-ls-dae/fmi-ls-manifest.xml` is an ordinary ODE
   * FMU until the importer sets the structural parameter the manifest names,
   * and a DAE FMU after: it then exposes the residuals of
   * `F(t, x, x', z) = 0` and lets the importer solve for the algebraic
   * variables `z` beside the states, instead of iterating on them internally.
   *
   * The manifest names the switch, the algebraic variables and the residuals;
   * the state derivatives are the model description's own
   * `<ContinuousStateDerivative>` list, which orders them with the states.
   */
  class LsDaeManifest
  {
  public:
    /// The layered standard this reader understands.
    static const char* name() { return "org.fmi-standard.fmi-ls-dae"; }
    /// Where the manifest sits inside an FMU.
    static const char* path() { return "extra/org.fmi-standard.fmi-ls-dae/fmi-ls-manifest.xml"; }

    /**
     * \brief Read the manifest out of an unzipped FMU.
     *
     * \param unzippedFmuPath  Directory the FMU was unzipped into.
     * \return true if a manifest is there and could be read. A missing file is
     *         not an error: the FMU simply has no DAE formulation.
     */
    bool load(const std::string& unzippedFmuPath);

    bool isValid() const { return valid; }
    /// The version the manifest declares, e.g. "1.0.0-alpha.1".
    const std::string& getVersion() const { return version; }
    /// The structural parameter that switches the FMU into DAE mode.
    uint32_t getEnableValueReference() const { return enableVr; }
    /// The algebraic variables the importer solves for, beside the states.
    const std::vector<uint32_t>& getAlgebraicVariables() const { return algebraicVrs; }
    /// One value reference per residual of F.
    const std::vector<uint32_t>& getResiduals() const { return residualVrs; }
    /**
     * \brief Per residual, the value references it declares a dependency on;
     *        empty for a residual that named none, which the standard reads as
     *        a dependency on every known.
     */
    const std::vector<std::vector<uint32_t>>& getResidualDependencies() const { return residualDependencies; }
    /**
     * \brief The state derivatives the manifest lists.
     *
     * Empty for the fully implicit form `F(t, x, x', z) = 0`, where the
     * residuals cover the state rows too. A non-empty list is the semi-explicit
     * form: the FMU answers with `der(x)` and the residuals constrain the
     * algebraic variables alone.
     */
    const std::vector<uint32_t>& getContinuousStateDerivatives() const { return continuousStateDerivatives; }
    bool isFullyImplicit() const { return continuousStateDerivatives.empty(); }

  private:
    bool valid = false;
    std::string version;
    uint32_t enableVr = 0;
    std::vector<uint32_t> algebraicVrs;
    std::vector<uint32_t> residualVrs;
    std::vector<std::vector<uint32_t>> residualDependencies;
    std::vector<uint32_t> continuousStateDerivatives;
  };
}

#endif
