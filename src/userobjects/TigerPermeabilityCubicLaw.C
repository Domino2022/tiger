/**************************************************************************/
/*  TIGER - THMC sImulator for GEoscience Research                        */
/*                                                                        */
/*  Copyright (C) 2017 by Maziar Gholami Korzani                          */
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

#include "TigerPermeabilityCubicLaw.h"
#include "MooseError.h"

registerMooseObject("TigerApp", TigerPermeabilityCubicLaw);


InputParameters
TigerPermeabilityCubicLaw::validParams()
{
  InputParameters params = TigerPermeability::validParams();
  params.addParam<Real>("aperture", 0, "Aperture of the fracture (m),"
        " Otherwise the given scaling factor will be used");
  params.addParam<Real>("reservor_thickness", 1, "Reservoir thickness for "
        " correcting scaling factor of 1D fractures (one for 2D fractures)");
  params.set<ExecFlagEnum>("execute_on", true) = EXEC_TIMESTEP_BEGIN;
  params.addClassDescription("Permeability tensor for fractures"
        " based on the Cubic law valid only on lower dimensional 2D and 1D. "
        " It is not allowed to use this object if mesh is 3D and the lower"
        " dimensionl element is 1D (wells)");
  return params;
}

TigerPermeabilityCubicLaw::TigerPermeabilityCubicLaw(const InputParameters & parameters)
  : TigerPermeability(parameters),
  _aperture(getParam<Real>("aperture")),
  _rt(getParam<Real>("reservor_thickness"))
{
}

RankTwoTensor
TigerPermeabilityCubicLaw::Permeability(const int & dim, const Real & porosity, const Real & scale_factor, const std::vector<Real> kmat) const
{
  if (dim == 3)
    mooseError(name(),": This permeability userobject cannot be used for 3D elements.");

  Real effAperture = 0;
  effAperture = _aperture == 0 ? (scale_factor/_rt) : _aperture;

  std::vector<Real> k0;
  k0.push_back(std::pow(scale_factor,2.0) / 12.0);

  MooseEnum pt("isotropic","isotropic");

  return PermeabilityTensorCalculator(dim, k0, pt);
}
