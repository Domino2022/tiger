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

#ifndef TIGERFLUIDMATERIAL_H
#define TIGERFLUIDMATERIAL_H

#include "Material.h"
#include "SinglePhaseFluidProperties.h"

 

class TigerFluidMaterial : public Material
{
public:
  static InputParameters validParams();
  TigerFluidMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  // Pore pressure nonlinear variable
  const VariableValue & _P;
  // Temperature nonlinear variable
  const VariableValue & _T;
  // Userobject from fluid_properties_module for calculating fluid properties
  const SinglePhaseFluidProperties & _fp_uo;

  // Density of the fluid
  MaterialProperty<Real> & _rho_f;
  // Density derivative wrt pressure for the fluid
  MaterialProperty<Real> & _drho_dp_f;
  // Density derivative wrt pressure for the fluid
  MaterialProperty<Real> & _drho_dT_f;
  // Viscosity of the fluid
  MaterialProperty<Real> & _mu_f;
  // Viscosity derivative wrt pressure for the fluid
  MaterialProperty<Real> & _dmu_dp_f;
  // Viscosity derivative wrt temperature for the fluid
  MaterialProperty<Real> & _dmu_dT_f;
  // Compressibility of the fluid
  MaterialProperty<Real> & _beta_f;
  // Specific heat at constant pressure for the fluid
  MaterialProperty<Real> & _cp_f;
  // Thermal conductivity of the fluid
  MaterialProperty<Real> & _lambda_f;

};

#endif /* TIGERFLUIDMATERIAL_H */
