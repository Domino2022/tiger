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

#include "TigerSoluteAdvectionKernelS.h"
#include "MaterialPropertyInterface.h"

registerMooseObject("TigerApp", TigerSoluteAdvectionKernelS);


InputParameters
TigerSoluteAdvectionKernelS::validParams()
{
  InputParameters params = Kernel::validParams();

  params.addCoupledVar("pressure", 0 ,"Pore pressure nonlinear variable");

  return params;
}

TigerSoluteAdvectionKernelS::TigerSoluteAdvectionKernelS(const InputParameters & parameters)
  : Kernel(parameters),
  _scale_factor(getMaterialProperty<Real>("scale_factor")),
  _SUPG_p(getMaterialProperty<RealVectorValue>("solute_petrov_supg_p_function")),
  _SUPG_ind(getMaterialProperty<bool>("solute_supg_indicator")),
  _av_ind(getMaterialProperty<bool>("solute_av_dv_indicator")),
  _av(getMaterialProperty<RealVectorValue>("solute_advection_velocity")),
  _pressure_var(coupled("pressure"))
{
  if (parameters.isParamSetByUser("pressure"))
  {
    _dav_dT = &getMaterialProperty<RealVectorValue>("d_darcy_velocity_dT");
    _dav_dp_phi = &getMaterialProperty<RealVectorValue>("d_darcy_velocity_dp_phi");
    _dav_dp_gradphi = &getMaterialProperty<RankTwoTensor>("d_darcy_velocity_dp_gradphi");
  }
  else
  {
    _dav_dT = NULL;
    _dav_dp_phi = NULL;
    _dav_dp_gradphi = NULL;
  }
}

Real
TigerSoluteAdvectionKernelS::computeQpResidual()
{
  Real test = 0.0;

  if (_SUPG_ind[_qp])
    test = _test[_i][_qp] + _SUPG_p[_qp] * _grad_test[_i][_qp];
  else
    test = _test[_i][_qp];

  return _scale_factor[_qp] * test * _av[_qp] * _grad_u[_qp];
}

Real
TigerSoluteAdvectionKernelS::computeQpJacobian()
{
  Real test = 0.0 , j = 0.0;

  if (_SUPG_ind[_qp])
    test = _test[_i][_qp] + _SUPG_p[_qp] * _grad_test[_i][_qp];
  else
    test = _test[_i][_qp];

  j += _av[_qp] * _grad_phi[_j][_qp];
  j *= _scale_factor[_qp] * test;

  return j;
}

Real
TigerSoluteAdvectionKernelS::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real test = 0.0 , j = 0.0;

  if (jvar == _pressure_var)
  {
    if (_SUPG_ind[_qp])
      test = _test[_i][_qp] + _SUPG_p[_qp] * _grad_test[_i][_qp];
    else
      test = _test[_i][_qp];

    j  = (*_dav_dp_phi)[_qp] * _phi[_j][_qp] * _grad_u[_qp];
    j += (*_dav_dp_gradphi)[_qp] * _grad_phi[_j][_qp] * _grad_u[_qp];
    j *= _scale_factor[_qp] * test;
  }

  return j;
}
