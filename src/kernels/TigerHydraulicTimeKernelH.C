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

#include "TigerHydraulicTimeKernelH.h"

registerMooseObject("TigerApp", TigerHydraulicTimeKernelH);

InputParameters
TigerHydraulicTimeKernelH::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  return params;
}

TigerHydraulicTimeKernelH::TigerHydraulicTimeKernelH(const InputParameters & parameters)
  : TimeDerivative(parameters),
    _scale_factor(getMaterialProperty<Real>("scale_factor")),
    _H_Kernel_dt(getMaterialProperty<Real>("H_Kernel_dt_coefficient"))
{
}

Real
TigerHydraulicTimeKernelH::computeQpResidual()
{
  return _scale_factor[_qp] * _H_Kernel_dt[_qp] * TimeDerivative::computeQpResidual();
}

Real
TigerHydraulicTimeKernelH::computeQpJacobian()
{
  return _scale_factor[_qp] * _H_Kernel_dt[_qp] * TimeDerivative::computeQpJacobian();
}
