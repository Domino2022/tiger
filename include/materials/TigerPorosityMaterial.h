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

#pragma once

#include "Material.h"

 

class TigerPorosityMaterial : public Material
{
public:
  static InputParameters validParams();
  TigerPorosityMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  // Coupled temperature
  const VariableValue & _temp;
  // Coupled presysure
  const VariableValue & _pressure;
  // bulk density
  MaterialProperty<Real> & _rho_b;
  // mixture density
  MaterialProperty<Real> & _rho_m;
  // void mass fraction
  MaterialProperty<Real> & _mass_frac;
  // calculated porosity
  MaterialProperty<Real> & _n;
  // Initial porosity
  const VariableValue & _n0;
  // Volumetric thermal expansion
  MaterialProperty<Real> & _thermal_expansion_coeff;
  //Stress free reference temperature for porosity evolution
  const VariableValue & _stress_free_temperature;
  //Stress free reference pressure for porosity evolution
  const VariableValue & _reference_pressure;

  // taken from TigerFluidMaterial
  const MaterialProperty<Real> & _rho_f;

  // taken from TigerMechanicsMaterialM
  const MaterialProperty<Real> * _biot;
  const MaterialProperty<Real> * _vol_total_strain;
//taken from TigerHydraulicMaterialH
  const MaterialProperty<Real> * _solid_bulk;


private:
  const bool _p_e;
  const Real _rho_r;
  Real _alpha_t;
  bool _ev_type;
};
