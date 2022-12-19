/**************************************************************************/
/*  TIGER - THMC sImulator for GEoscience Research                        */
/*                                                                        */
/*  Copyright (C) 2017 by Maziar Gholami Korzani, Robert Egert            */
/*  Karlsruhe Institute of Technology, Institute of Applied Geosciences   */
/*  Division of Geothermal Research                                       */
/*                                                                        */
/*  This file is part of TIGER App                                        */
/*                                                                        */
/*  This program is free software: you can redistribute it and/or modify  */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  This program is distributed in the hope that it will be useful,       */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>  */
/**************************************************************************/

#include "TigerPermeabilityVar.h"
#include "MooseError.h"
#include <algorithm>

registerMooseObject("TigerApp", TigerPermeabilityVar);

InputParameters
TigerPermeabilityVar::validParams()
{
  InputParameters params = TigerPermeability::validParams();
  params.addRequiredParam<std::vector<Real>>("k0", "Initial permeability (m^2)");
  params.addRequiredParam<Real>("n0", "Initial porosity");
  params.addParam<Real>("m", 2, "The power in the denominator of the Kozeny-Carman Eq");
  params.addParam<Real>("n", 3, "The power in the numerator of the Kozeny-Carman Eq");
  MooseEnum PT("isotropic orthotropic anisotropic");
  params.addRequiredParam<MooseEnum>("permeability_type", PT,
        "The permeability distribution type [isotropic, orthotropic, anisotropic].");
  params.set<ExecFlagEnum>("execute_on", true) = EXEC_TIMESTEP_BEGIN;
  params.addClassDescription("Permeability tensor which evoloves by porosity using "
        "Kozeny-Carman equation based on provided initial permeability and porosity");
  return params;
}

TigerPermeabilityVar::TigerPermeabilityVar(const InputParameters & parameters)
  : TigerPermeability(parameters),
    _kinit(getParam<std::vector<Real>>("k0")),
    _ninit(getParam<Real>("n0")),
    _n(getParam<Real>("n")),
    _m(getParam<Real>("m")),
    _permeability_type(getParam<MooseEnum>("permeability_type"))
{
  Real c = std::pow(1.0 - _ninit, _m) / std::pow(_ninit, _n);
  std::transform(_kinit.begin(), _kinit.end(), _kinit.begin(), [c](Real k){ return c * k;});
}

RankTwoTensor
TigerPermeabilityVar::Permeability(const int & dim, const Real & porosity, const Real & scale_factor, const std::vector<Real> kmat) const
{
  if (dim != 3)
    mooseError(name(),": This permeability userobject can be only used for 3D elements.");

    // Accept permeability either from userobject or TigerHydraulicMaterialH
    // This enables permeabilities as a function
    if (size(kmat) > 0)
      {
        std::vector<Real> kn(kmat);
        Real c = std::pow(porosity, _n) / std::pow(1.0 - porosity, _m);
        std::transform(kn.begin(), kn.end(), kn.begin(), [c](Real k){ return c * k;});

        return  PermeabilityTensorCalculator(dim, kn, _permeability_type);
      }
      else
      {
        std::vector<Real> kn(_kinit);
        Real c = std::pow(porosity, _n) / std::pow(1.0 - porosity, _m);
        std::transform(kn.begin(), kn.end(), kn.begin(), [c](Real k){ return c * k;});

        return  PermeabilityTensorCalculator(dim, kn, _permeability_type);
      }

}
