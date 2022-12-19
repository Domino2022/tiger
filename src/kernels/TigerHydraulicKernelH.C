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

#include "TigerHydraulicKernelH.h"

registerMooseObject("TigerApp", TigerHydraulicKernelH);


InputParameters
TigerHydraulicKernelH::validParams()
{
  InputParameters params = Kernel::validParams();

  params.addCoupledVar("temperature", 0 ,"temperature nonlinear variable");

  return params;
}

TigerHydraulicKernelH::TigerHydraulicKernelH(const InputParameters & parameters)
  : Kernel(parameters),
    _scale_factor(getMaterialProperty<Real>("scale_factor")),
    _k_vis(getMaterialProperty<RankTwoTensor>("permeability_by_viscosity")),
    _rho_f(getMaterialProperty<Real>("fluid_density")),
    _drho_dp_f(getMaterialProperty<Real>("fluid_drho_dp")),
    _drho_dT_f(getMaterialProperty<Real>("fluid_drho_dT")),
    _mu_f(getMaterialProperty<Real>("fluid_viscosity")),
    _dmu_dp_f(getMaterialProperty<Real>("fluid_dmu_dp")),
    _dmu_dT_f(getMaterialProperty<Real>("fluid_dmu_dT")),
    _g(getMaterialProperty<RealVectorValue>("gravity_vector")),
    _temperature_var(coupled("temperature"))
{
}

Real
TigerHydraulicKernelH::computeQpResidual()
{
  RealVectorValue r;
  r  = _k_vis[_qp] * ( _grad_u[_qp] - _rho_f[_qp] * _g[_qp] );

  return  r * _scale_factor[_qp] * _grad_test[_i][_qp];
}

Real
TigerHydraulicKernelH::computeQpJacobian()
{
  RealVectorValue j;
  j  = (-_dmu_dp_f[_qp] / _mu_f[_qp] * _k_vis[_qp] * _phi[_j][_qp])
        * (_grad_u[_qp] - _rho_f[_qp] * _g[_qp]);
  j += _k_vis[_qp] * (_grad_phi[_j][_qp] - _drho_dp_f[_qp] * _phi[_j][_qp] * _g[_qp]);

  return _scale_factor[_qp] * _grad_test[_i][_qp] * j;
}

Real
TigerHydraulicKernelH::computeQpOffDiagJacobian(unsigned int jvar)
{
  RealVectorValue j = 0.0;

  if (jvar == _temperature_var)
  {
    j  = (-_dmu_dT_f[_qp] / _mu_f[_qp] * _k_vis[_qp] * _phi[_j][_qp])
          * (_grad_u[_qp] - _rho_f[_qp] * _g[_qp]);

    j -= _k_vis[_qp] * _drho_dT_f[_qp] * _phi[_j][_qp] * _g[_qp];
  }

  return _scale_factor[_qp] * _grad_test[_i][_qp] * j;
}
