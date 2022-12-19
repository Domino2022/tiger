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

#include "TigerPermeabilityConst.h"
#include "MooseError.h"

registerMooseObject("TigerApp", TigerPermeabilityConst);

InputParameters
TigerPermeabilityConst::validParams()
{
  InputParameters params = TigerPermeability::validParams();
  params.addRequiredParam<std::vector<Real>>("k0", "Initial permeability (m^2)");
  MooseEnum PT("isotropic orthotropic anisotropic");
  params.addRequiredParam<MooseEnum>("permeability_type", PT,
        "The permeability distribution type [isotropic, orthotropic, anisotropic].");
  params.set<ExecFlagEnum>("execute_on", true) = EXEC_INITIAL;
  params.addClassDescription("Permeability tensor based on provided "
        "constant permeability value(s)");
  return params;
}

TigerPermeabilityConst::TigerPermeabilityConst(const InputParameters & parameters)
  : TigerPermeability(parameters),
    _kinit(getParam<std::vector<Real>>("k0")),
    _permeability_type(getParam<MooseEnum>("permeability_type"))
{
}

RankTwoTensor
TigerPermeabilityConst::Permeability(const int & dim, const Real & porosity, const Real & scale_factor, const std::vector<Real> kmat) const
{

  // Accept permeability either from userobject or TigerHydraulicMaterialH
  // This enables permeabilities as a function
  if (size(kmat) > 0)
    {
      return  PermeabilityTensorCalculator(dim, kmat, _permeability_type);
    }
    else
    {
      return  PermeabilityTensorCalculator(dim, _kinit, _permeability_type);
    }
}
