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

#include "TigerMechanicsGravityM.h"

registerMooseObject("TigerApp", TigerMechanicsGravityM);

InputParameters
TigerMechanicsGravityM::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("Apply the gravity for porous materials in Mechanics");
  params.addParam<bool>("use_displaced_mesh", true, "Displaced mesh defaults to true");
  params.addRequiredParam<signed int>(
      "component", "the gravity direction (0 for x, 1 for y, 2 for z)");
  return params;
}

TigerMechanicsGravityM::TigerMechanicsGravityM(const InputParameters & parameters)
  : Kernel(parameters),
    _density(getMaterialProperty<Real>("bulk_density")),
    _g(getMaterialProperty<RealVectorValue>("gravity_vector")),
    _component(getParam<signed int>("component"))
{
}

Real
TigerMechanicsGravityM::computeQpResidual()
{
  return _density[_qp] * _test[_i][_qp] * -_g[_qp](_component);
}
