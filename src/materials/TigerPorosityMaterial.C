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

#include "TigerPorosityMaterial.h"
#include "MooseMesh.h"
#include <cfloat>
#include "Function.h"

registerMooseObject("TigerApp", TigerPorosityMaterial);

InputParameters
TigerPorosityMaterial::validParams()
{
  InputParameters params = Material::validParams();

  params.addCoupledVar("temperature",0, "temperature nonlinear variable");
  params.addCoupledVar("pressure", 0,"pressure nonlinear variable");
  params.addRequiredParam<Real>("specific_density",
        "specific density of rock for calculating bulk density (kg/m^3)");
  params.addRequiredCoupledVar("porosity", "porosity (temporal and spatial function)");
  params.addParam<bool>("porosity_evolution", false,"if it evoloves by "
        "deformation, true");
  MooseEnum ET("exponential fractional", "exponential");
  params.addParam<MooseEnum>("evolution_type", ET,
        "Two approaches to calculate porosity evolution based on total strain"
        "[exponential, fractional]");
  params.addParam<Real>("thermal_expansion_coeff",0,
        "The linear thermal expansion coefficient of the mixture");
  params.addCoupledVar("stress_free_temperature",0,
                                    "Reference temperature for which the porosity change due to thermal expansion"
                                    "is calculated. Values or AuxVariables are possible.");
  params.addCoupledVar("reference_pressure",0,
                                    "Reference pressure for which the porosity change due to fluid pressure"
                                    "is calculated. Values or AuxVariables are possible.");
  params.addClassDescription("Material for defining porosity and its evolution");

  return params;
}

TigerPorosityMaterial::TigerPorosityMaterial(const InputParameters & parameters)
  : Material(parameters),
    _temp(coupledValue("temperature")),
    _pressure(coupledValue("pressure")),
    _rho_b(declareProperty<Real>("bulk_density")),
    _rho_m(declareProperty<Real>("mixture_density")),
    _mass_frac(declareProperty<Real>("void_mass_fraction")),
    _n(declareProperty<Real>("porosity")),
    _n0(coupledValue("porosity")),
    _thermal_expansion_coeff(declareProperty<Real>("thermal_expansion_coeff")),
    _stress_free_temperature(coupledValue("stress_free_temperature")),
    _reference_pressure(coupledValue("reference_pressure")),
    _rho_f(getMaterialProperty<Real>("fluid_density")),
    _p_e(getParam<bool>("porosity_evolution")),
    _rho_r(getParam<Real>("specific_density")),
    _alpha_t(getParam<Real>("thermal_expansion_coeff"))
{
  MooseEnum ET("exponential", "exponential");
  if(getParam<MooseEnum>("evolution_type")==ET)
    _ev_type = true;
  else
    _ev_type = false;

  if (_p_e)
  {
    _biot = &getMaterialProperty<Real>("biot_coefficient");
    _vol_total_strain = &getMaterialProperty<Real>("total_volumetric_strain_HM");
    _solid_bulk = &getMaterialProperty<Real>("solid_bulk");
  }
  else
  {
    _biot = NULL;
    _vol_total_strain = NULL;
  }
}

void
TigerPorosityMaterial::computeQpProperties()
{
  if (!_p_e)
  {
    _n[_qp] = _n0[_qp];
    _thermal_expansion_coeff[_qp] = 0;
    Real fluid_coeff = 0;
  }
  else
  {
    //Make thermal expansion volumetric from linear input and getting the coefficients
    _thermal_expansion_coeff[_qp] = 3*_alpha_t;
    Real fluid_coeff = (((*_biot)[_qp] - 1.0) / (*_solid_bulk)[_qp]);
    // Get the mechanical strain component from TigerMechanicsMaterialM
    Real mech_component = -(*_vol_total_strain)[_qp];
    // Volumetric change due to temperature changes
    Real thermal_component = (_thermal_expansion_coeff)[_qp] * ( (_temp)[_qp] - _stress_free_temperature[_qp] );
    // Volumetric change due to pore pressure change
    Real fluid_component = (fluid_coeff) * ( (_pressure)[_qp] - _reference_pressure[_qp] );
    // Get final exponent
    Real strain_components = fluid_component + thermal_component + mech_component;
    // More components for the exponential porosity evolution
    Real c = log((*_biot)[_qp] / ((*_biot)[_qp] - _n0[_qp]));
    Real expo = exp(-strain_components / c);

    if (_ev_type)
    {
      // Exponential porosity evolution
    /*  For further details see. Chen, Zhou & Jing - Modeling coupled THM processes of geological
      porous media with multiphase flow - Computers and Geotechnics 36 (2009) 1308-1329 -
      DOI: 10.1016/j.compgeo.2009.06.001 */
      _n[_qp] = (*_biot)[_qp] + (_n0[_qp] - (*_biot)[_qp]) * exp(c * (1.0 - expo));
    }
    else
    {
      // Linear porosity evolution
  //    Old formulation
  //    _n[_qp] = (_n0[_qp] + (*_vol_total_strain)[_qp]) / (1.0 + (*_vol_total_strain)[_qp]);
  // Make it consistent with Porous flow, but looks physically not correct to me.
    _n[_qp] = (_n0[_qp] + fluid_component + thermal_component - mech_component);
      if (_n[_qp]<0.0) mooseError("negative porosity due to very low volumetric strain");
    }
  }

  _rho_b[_qp] = (1.0 - _n[_qp]) * _rho_r;
  _rho_m[_qp] = _n[_qp] * _rho_f[_qp] + _rho_b[_qp];

  if (_n[_qp] ==0.0 || _n[_qp] == 1.0)
    _mass_frac[_qp] =  _n[_qp];
  else
  {
    if ((_rho_r - _rho_f[_qp]) == 0.0 || _rho_m[_qp] == 0.0)
      mooseError("Rock density and fluid density are either equal or zero");
    else
      _mass_frac[_qp] = (_rho_r - _rho_m[_qp]) * _rho_f[_qp] / _rho_m[_qp] / (_rho_r - _rho_f[_qp]);
  }
}
