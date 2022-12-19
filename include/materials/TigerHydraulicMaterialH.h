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


#include "Material.h"
#include "RankTwoTensor.h"
#include "TigerPermeability.h"

class TigerHydraulicMaterialH : public Material
{
public:
  static InputParameters validParams();

  TigerHydraulicMaterialH(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  // Gradient of pressure
  const VariableGradient & _grad_p;

  // Permeability tensor divided by viscosity (rotated in lowerD)
  MaterialProperty<RankTwoTensor> & _k_vis;
  // Hydraulic time derivative coefficient
  MaterialProperty<Real> & _H_Kernel_dt;
  // Tiger permeability calculater userobject
  const TigerPermeability & _kf_uo;
  // Permeability vector filled with functions
  std::vector<const Function *> _perm_vector;
  // Darcy velocity
  MaterialProperty<RealVectorValue> & _dv;
  // Derivative of Dracy velocity wrt temperature
  MaterialProperty<RealVectorValue> & _ddv_dT;
  // Derivative of Dracy velocity wrt pressure to multiply by _phi
  MaterialProperty<RealVectorValue> & _ddv_dp_phi;
  // Derivative of Dracy velocity wrt pressure to multiply by _grad_phi
  MaterialProperty<RankTwoTensor> & _ddv_dp_gradphi;

  // Imported props from TigerGeometryMaterial
  const MaterialProperty<Real> & _n;
  const MaterialProperty<Real> & _scale_factor;
  const MaterialProperty<RankTwoTensor> & _rot_mat;

  // imported props from TigerFluidMaterial
  const MaterialProperty<Real> & _rho_f;
  const MaterialProperty<Real> & _mu_f;
  const MaterialProperty<Real> & _beta_f;
  const MaterialProperty<Real> & _drho_dT_f;
  const MaterialProperty<Real> & _drho_dp_f;
  const MaterialProperty<Real> & _dmu_dT_f;
  const MaterialProperty<Real> & _dmu_dp_f;

  const MaterialProperty<RealVectorValue> & _gravity;

private:
  // Compressibility of the solid phase
  Real _beta_s;
  // Permeability vector filled from functional input
  std::vector<Real> _kinit;

};
