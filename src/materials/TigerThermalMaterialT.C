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

#include "TigerThermalMaterialT.h"
#include "MooseMesh.h"
#include "libmesh/quadrature.h"

registerMooseObject("TigerApp", TigerThermalMaterialT);

InputParameters
TigerThermalMaterialT::validParams()
{
  InputParameters params = Material::validParams();

  params.addRequiredParam<Real>("specific_heat",
        "Specific heat of rock matrix (J/(kg K))");
  params.addRequiredParam<std::vector<Real>>("lambda",
        "Initial thermal conductivity of rock matrix (W/(m K))");
  MooseEnum Advection
        ("pure_diffusion darcy_velocity user_velocity darcy_user_velocities",
        "darcy_velocity");
  params.addParam<MooseEnum>("advection_type", Advection,
        "Type of the velocity to simulate advection [pure_diffusion "
        "darcy_velocity user_velocity darcy_user_velocities]");
  MooseEnum CT("isotropic orthotropic anisotropic");
  params.addRequiredParam<MooseEnum>("conductivity_type", CT,
        "Thermal conductivity distribution type [isotropic, orthotropic, anisotropic]");
  MooseEnum Mean("arithmetic geometric", "arithmetic");
  params.addParam<MooseEnum>("mean_calculation_type", Mean,
        "Solid-liquid mixture thermal conductivity calculation method "
        "[arithmetic, geometric]");
  params.addParam<bool>("output_Pe_Cr_numbers", false ,
        "calcuate Peclet and Courant numbers");
  params.addParam<bool>("has_supg", false ,
        "Is Streameline Upwinding / Petrov Galerkin (SU/PG) activated?");
  params.addParam<Real>("supg_coeficient_scale", 1.0 ,
        "The user defined factor to scale SU/PG coefficent (tau)");
  params.addParam<FunctionName>("user_velocity", 0.0,
        "a vector function to define the velocity field");
  params.addParam<UserObjectName>("supg_uo", "",
        "The name of the userobject for SU/PG");
  params.addClassDescription("Thermal material for thermal kernels");

  return params;
}

TigerThermalMaterialT::TigerThermalMaterialT(const InputParameters & parameters)
  : Material(parameters),
    _at(getParam<MooseEnum>("advection_type")),
    _ct(getParam<MooseEnum>("conductivity_type")),
    _mean(getParam<MooseEnum>("mean_calculation_type")),
    _lambda0(getParam<std::vector<Real>>("lambda")),
    _cp0(getParam<Real>("specific_heat")),
    _has_PeCr(getParam<bool>("output_Pe_Cr_numbers")),
    _has_supg(getParam<bool>("has_supg")),
    _supg_scale(getParam<Real>("supg_coeficient_scale")),
    _lambda_sf(declareProperty<RankTwoTensor>("thermal_conductivity_mixture")),
    _TimeKernelT(declareProperty<Real>("TimeKernel_T")),
    _dTimeKernelT_dT(declareProperty<Real>("dTimeKernelT_dT")),
    _dTimeKernelT_dp(declareProperty<Real>("dTimeKernelT_dp")),
    _SUPG_ind(declareProperty<bool>("thermal_supg_indicator")),
    _av_ind(declareProperty<bool>("thermal_av_dv_indicator")),
    _av(declareProperty<RealVectorValue>("thermal_advection_velocity")),
    _SUPG_p(declareProperty<RealVectorValue>("thermal_petrov_supg_p_function")),
    _rot_mat(getMaterialProperty<RankTwoTensor>("lowerD_rotation_matrix")),
    _n(getMaterialProperty<Real>("porosity")),
    _rho_m(getMaterialProperty<Real>("mixture_density")),
    _mass_frac(getMaterialProperty<Real>("void_mass_fraction")),
    _cp_f(getMaterialProperty<Real>("fluid_specific_heat")),
    _lambda_f(getMaterialProperty<Real>("fluid_thermal_conductivity")),
    _drho_dT_f(getMaterialProperty<Real>("fluid_drho_dT")),
    _drho_dp_f(getMaterialProperty<Real>("fluid_drho_dp"))
{
  _Pe = (_has_PeCr || _has_supg) ?
              &declareProperty<Real>("thermal_peclet_number") : NULL;
  _Cr = (_has_PeCr || _has_supg) ?
              &declareProperty<Real>("thermal_courant_number") : NULL;
  _vel_func = (_at == AT::user_velocity || _at == AT::darcy_user_velocities) ?
              &getFunction("user_velocity") : NULL;
  _supg_uo = (parameters.isParamSetByUser("supg_uo")) ?
              &getUserObject<TigerSUPG>("supg_uo") : NULL;
  _dv = (_at == AT::darcy_velocity || _at == AT::darcy_user_velocities) ?
              &getMaterialProperty<RealVectorValue>("darcy_velocity") : NULL;
}

void
TigerThermalMaterialT::computeQpProperties()
{
  Real c_p_m = _mass_frac[_qp] * _cp_f[_qp] + (1.0 - _mass_frac[_qp]) * _cp0;

  _TimeKernelT[_qp] = _rho_m[_qp] * c_p_m;
  _dTimeKernelT_dT[_qp] = _n[_qp] * _drho_dT_f[_qp] * c_p_m;
  _dTimeKernelT_dp[_qp] = _n[_qp] * _drho_dp_f[_qp] * c_p_m;

  switch (_mean)
  {
    case M::arithmetic:
      _lambda_sf[_qp] = Ari_Cond_Calc(_n[_qp], _lambda_f[_qp], _lambda0, _current_elem->dim());
      break;
    case M::geometric:
      _lambda_sf[_qp] = Geo_Cond_Calc(_n[_qp], _lambda_f[_qp], _lambda0, _current_elem->dim());
      break;
  }

  if (_current_elem->dim() < _mesh.dimension())
    _lambda_sf[_qp].rotate(_rot_mat[_qp]);

  switch (_at)
  {
    case AT::pure_diffusion:
      _av[_qp].zero();
      _av_ind[_qp] = false;
    break;
    case AT::darcy_velocity:
      _av[_qp] = (*_dv)[_qp];
      _av_ind[_qp] = true;
      break;
    case AT::user_velocity:
      _av[_qp] = _vel_func->vectorValue(_t, _q_point[_qp]);
      _av_ind[_qp] = false;
      break;
    case AT::darcy_user_velocities:
      _av[_qp] = (*_dv)[_qp] + _vel_func->vectorValue(_t, _q_point[_qp]);
      _av_ind[_qp] = true;
      break;
  }

  Real lambda = _lambda_sf[_qp].trace() / (_current_elem->dim() * _TimeKernelT[_qp]);

  if (_has_PeCr && !_has_supg)
    _supg_uo->PeCrNrsCalculator(lambda, _dt, _current_elem, _av[_qp], (*_Pe)[_qp], (*_Cr)[_qp]);

  if (_has_supg)
  {
    // should be multiplied by the gradient of the test function to build the Petrov Galerkin P function
    _supg_uo->SUPGCalculator(lambda, _dt, _current_elem, _av[_qp], _SUPG_p[_qp], (*_Pe)[_qp], (*_Cr)[_qp]);
    _SUPG_p[_qp] *= _supg_scale;

    if (_SUPG_p[_qp].norm() != 0.0)
      _SUPG_ind[_qp] = true;
    else
      _SUPG_ind[_qp] = false;
  }
  else
    _SUPG_ind[_qp] = false;
}

RankTwoTensor
TigerThermalMaterialT::Ari_Cond_Calc(Real const & n, Real const & lambda_f, const std::vector<Real> & lambda_s, const int & dim)
{
  RankTwoTensor lambda = RankTwoTensor();
  RealVectorValue lambda_x;
  RealVectorValue lambda_y;
  RealVectorValue lambda_z;
  lambda_x.zero();
  lambda_y.zero();
  lambda_z.zero();


  if (dim == 1)
    switch (_ct)
    {
      case CT::isotropic:
        if (lambda_s.size() != 1)
          mooseError("One input value is needed for isotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda = ((1.0 - n) * lambda_s[0] + n * lambda_f) * RankTwoTensor(1., 0., 0., 0., 0., 0.);
        break;
      case CT::orthotropic ... CT::anisotropic:
        mooseError("One dimensional elements cannot have non-isotropic thermal conductivity values.\n");
        break;
    }
  else if (dim == 2)
    switch (_ct)
    {
      case CT::isotropic:
        if (lambda_s.size() != 1)
          mooseError("One input value is needed for isotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda = ((1.0 - n) * lambda_s[0] + n * lambda_f) * RankTwoTensor(1., 1., 0., 0., 0., 0.);
        break;
      case CT::orthotropic:
        if (lambda_s.size() != 2)
          mooseError("Two input values are needed for orthotropic distribution of thermal conductivity in two dimensional elements! You provided ", lambda_s.size(), " values.\n");
        lambda  = (1.0 - n)    * RankTwoTensor(lambda_s[0], lambda_s[1], 0., 0., 0., 0.);
        lambda += n * lambda_f * RankTwoTensor(1.         , 1.         , 0., 0., 0., 0.);
        break;
      case CT::anisotropic:
        if (lambda_s.size() != 4)
          mooseError("Four input values are needed for anisotropic distribution of thermal conductivity in two dimensional elements! You provided ", lambda_s.size(), " values.\n");
        lambda_x = RealVectorValue(lambda_s[0], lambda_s[1], 0.0);
        lambda_y = RealVectorValue(lambda_s[2], lambda_s[3], 0.0);
        lambda  = (1.0 - n)    * RankTwoTensor(lambda_x, lambda_y, lambda_z);
        lambda += n * lambda_f * RankTwoTensor(1., 1., 0., 0., 0., 0.);
        break;
    }
  else if (dim == 3)
    switch (_ct)
    {
      case CT::isotropic:
        if (lambda_s.size() != 1)
          mooseError("One input value is needed for isotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda = ((1.0 - n) * lambda_s[0] + n * lambda_f) * RankTwoTensor::Identity();
        break;
      case CT::orthotropic:
        if (lambda_s.size() != 3)
          mooseError("Three input values are needed for orthotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda  = (1.0 - n) * RankTwoTensor(lambda_s[0], lambda_s[1], lambda_s[2], 0., 0., 0.);
        lambda += n * lambda_f * RankTwoTensor::Identity();
        break;
      case CT::anisotropic:
        if (lambda_s.size() != 9)
          mooseError("Nine input values are needed for anisotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda_x = RealVectorValue(lambda_s[0], lambda_s[1], lambda_s[2]);
        lambda_y = RealVectorValue(lambda_s[3], lambda_s[4], lambda_s[5]);
        lambda_z = RealVectorValue(lambda_s[6], lambda_s[7], lambda_s[8]);
        lambda  = (1.0 - n) * RankTwoTensor(lambda_x, lambda_y, lambda_z);
        lambda += n * lambda_f * RankTwoTensor::Identity();
        break;
    }

  return lambda;
}

RankTwoTensor
TigerThermalMaterialT::Geo_Cond_Calc(Real const & n, Real const & lambda_f, const std::vector<Real> & lambda_s, const int & dim)
{
  RankTwoTensor lambda = RankTwoTensor();
  RealVectorValue lambda_x;
  RealVectorValue lambda_y;
  RealVectorValue lambda_z;
  lambda_x.zero();
  lambda_y.zero();
  lambda_z.zero();

  if (dim == 1)
    switch (_ct)
    {
      case CT::isotropic:
        if (lambda_s.size() != 1)
          mooseError("One input value is needed for isotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda = RankTwoTensor(1., 0., 0., 0., 0., 0.) * std::pow(lambda_f,n) * std::pow(lambda_s[0],(1.-n));
      break;
      case CT::orthotropic ... CT::anisotropic:
        mooseError("One dimensional elements cannot have non-isotropic thermal conductivity values.\n");
        break;
    }
  else if (dim == 2)
    switch (_ct)
    {
      case CT::isotropic:
        if (lambda_s.size() != 1)
          mooseError("One input value is needed for isotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda = RankTwoTensor(1., 1., 0., 0., 0., 0.) * std::pow(lambda_f,n) * std::pow(lambda_s[0],(1.-n));
        break;
      case CT::orthotropic:
        if (lambda_s.size() != 2)
          mooseError("Two input values are needed for orthotropic distribution of thermal conductivity in two dimensional elements! You provided ", lambda_s.size(), " values.\n");
        lambda  = RankTwoTensor(std::pow(lambda_s[0],(1.-n)), std::pow(lambda_s[1],(1.-n)), 0., 0., 0., 0.);
        lambda *= std::pow(lambda_f,n);
        break;
      case CT::anisotropic:
        mooseError("Geometric mean for thermal conductivity of mixture is not available in anisotropic distribution.\n");
        break;
    }
  else if (dim == 3)
    switch (_ct)
    {
      case CT::isotropic:
        if (lambda_s.size() != 1)
          mooseError("One input value is needed for isotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda = RankTwoTensor::Identity() * std::pow(lambda_f,n) * std::pow(lambda_s[0],(1.-n));
        break;
      case CT::orthotropic:
        if (lambda_s.size() != 3)
          mooseError("Three input values are needed for orthotropic distribution of thermal conductivity! You provided ", lambda_s.size(), " values.\n");
        lambda  = RankTwoTensor(std::pow(lambda_s[0],(1.-n)), std::pow(lambda_s[1],(1.-n)), std::pow(lambda_s[2],(1.-n)), 0., 0., 0.);
        lambda *= std::pow(lambda_f,n);
        break;
      case CT::anisotropic:
        mooseError("Geometric mean for thermal conductivity of mixture is not available in anisotropic distribution.\n");
        break;
    }

  return lambda;
}
