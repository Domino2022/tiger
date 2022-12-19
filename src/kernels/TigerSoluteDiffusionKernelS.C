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

#include "TigerSoluteDiffusionKernelS.h"

registerMooseObject("TigerApp", TigerSoluteDiffusionKernelS);

InputParameters
TigerSoluteDiffusionKernelS::validParams()
{
  InputParameters params = Kernel::validParams();
  return params;
}

TigerSoluteDiffusionKernelS::TigerSoluteDiffusionKernelS(const InputParameters & parameters)
  : Kernel(parameters),
    _scale_factor(getMaterialProperty<Real>("scale_factor")),
    _diffdisp(getMaterialProperty<RankTwoTensor>("diffusion_dispersion"))
{
}

Real
TigerSoluteDiffusionKernelS::computeQpResidual()
{
  return _grad_test[_i][_qp] * ( _scale_factor[_qp] * _diffdisp[_qp] * _grad_u[_qp]);
}

Real
TigerSoluteDiffusionKernelS::computeQpJacobian()
{
  return _grad_test[_i][_qp] * ( _scale_factor[_qp] * _diffdisp[_qp] * _grad_phi[_j][_qp]);
}
