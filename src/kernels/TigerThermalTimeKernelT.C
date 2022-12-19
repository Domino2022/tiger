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

#include "TigerThermalTimeKernelT.h"

registerMooseObject("TigerApp", TigerThermalTimeKernelT);

InputParameters
TigerThermalTimeKernelT::validParams()
{
  InputParameters params = TimeDerivative::validParams();

  params.addCoupledVar("pressure", 0 ,"Pore pressure nonlinear variable");

  return params;
}

TigerThermalTimeKernelT::TigerThermalTimeKernelT(const InputParameters & parameters)
  : TimeDerivative(parameters),
  _scale_factor(getMaterialProperty<Real>("scale_factor")),
  _TimeKernelT(getMaterialProperty<Real>("TimeKernel_T")),
  _dTimeKernelT_dT(getMaterialProperty<Real>("dTimeKernelT_dT")),
  _dTimeKernelT_dp(getMaterialProperty<Real>("dTimeKernelT_dp")),
  _SUPG_p(getMaterialProperty<RealVectorValue>("thermal_petrov_supg_p_function")),
  _SUPG_ind(getMaterialProperty<bool>("thermal_supg_indicator")),
  _pressure_var(coupled("pressure"))
{
}

Real
TigerThermalTimeKernelT::computeQpResidual()
{
  Real test = 0.0;

  if (_SUPG_ind[_qp])
    test = _test[_i][_qp] + _SUPG_p[_qp] * _grad_test[_i][_qp];
  else
    test = _test[_i][_qp];

  return _scale_factor[_qp] * _TimeKernelT[_qp] * test * _u_dot[_qp];
}

Real
TigerThermalTimeKernelT::computeQpJacobian()
{
  Real test = 0.0,j = 0.0;

  if (_SUPG_ind[_qp])
    test = _test[_i][_qp] + _SUPG_p[_qp] * _grad_test[_i][_qp];
  else
    test = _test[_i][_qp];

  j  = _TimeKernelT[_qp]  * _phi[_j][_qp] * _du_dot_du[_qp];
  j += _dTimeKernelT_dT[_qp] * _phi[_j][_qp] * _u_dot[_qp];
  j *= _scale_factor[_qp] * test;

  return j;
}

Real
TigerThermalTimeKernelT::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real test = 0.0 , j = 0.0;

  if (jvar == _pressure_var)
  {
    if (_SUPG_ind[_qp])
      test = _test[_i][_qp] + _SUPG_p[_qp] * _grad_test[_i][_qp];
    else
      test = _test[_i][_qp];

      j  = _dTimeKernelT_dp[_qp] * _phi[_j][_qp] * _u_dot[_qp];
      j *= _scale_factor[_qp] * test;
  }

  return j;
}
