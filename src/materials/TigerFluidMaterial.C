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

#include "TigerFluidMaterial.h"

registerMooseObject("TigerApp", TigerFluidMaterial);

InputParameters
TigerFluidMaterial::validParams()
{
  InputParameters params = Material::validParams();

  params.addCoupledVar("pressure", 0.0,
        "Pore pressure nonlinear variable (Pa)");
  params.addCoupledVar("temperature", 273.15,
        "temperature nonlinear variable (K)");
  params.addRequiredParam<UserObjectName>("fp_uo",
        "The name of the userobject for fluid properties");

  return params;
}

TigerFluidMaterial::TigerFluidMaterial(const InputParameters & parameters)
  : Material(parameters),
    _P(coupledValue("pressure")),
    _T(coupledValue("temperature")),
    _fp_uo(getUserObject<SinglePhaseFluidProperties>("fp_uo")),
    _rho_f(declareProperty<Real>("fluid_density")),
    _drho_dp_f(declareProperty<Real>("fluid_drho_dp")),
    _drho_dT_f(declareProperty<Real>("fluid_drho_dT")),
    _mu_f(declareProperty<Real>("fluid_viscosity")),
    _dmu_dp_f(declareProperty<Real>("fluid_dmu_dp")),
    _dmu_dT_f(declareProperty<Real>("fluid_dmu_dT")),
    _beta_f(declareProperty<Real>("fluid_compressibility")),
    _cp_f(declareProperty<Real>("fluid_specific_heat")),
    _lambda_f(declareProperty<Real>("fluid_thermal_conductivity"))
{
}

void
TigerFluidMaterial::computeQpProperties()
{
  Real Pressure = _P[_qp];
  if (Pressure <0.0)
  {
    mooseWarning("The pressure field has a negative value; Zero is used to avoid simulation interuption.\n");
    Pressure = 0.0;
  }

  _fp_uo.rho_from_p_T(Pressure, _T[_qp], _rho_f[_qp], _drho_dp_f[_qp], _drho_dT_f[_qp]);
  _fp_uo.mu_from_p_T(Pressure, _T[_qp], _mu_f[_qp], _dmu_dp_f[_qp], _dmu_dT_f[_qp]);
  _beta_f[_qp] = _drho_dp_f[_qp] / _rho_f[_qp];
  _cp_f[_qp] = _fp_uo.cp_from_p_T(Pressure, _T[_qp]);
  _lambda_f[_qp] = _fp_uo.k_from_p_T(Pressure, _T[_qp]);
}
