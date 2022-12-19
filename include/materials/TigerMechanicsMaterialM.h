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

class TigerMechanicsMaterialM : public Material
{
public:
  static InputParameters validParams();
  TigerMechanicsMaterialM(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  // biot coefficient for poromechanics
  MaterialProperty<Real> & _biot;
  // Solid bulk modulus
  MaterialProperty<Real> & _solid_bulk;
  /// Number of displacements supplied (1 in 1D, 2 in 2D, 3 in 3D)
  const unsigned int _ndisp;
  /// Gradient of the displacements
  std::vector<const VariableGradient *> _grad_disp;
  /// Old value of gradient of the displacements
  std::vector<const VariableGradient *> _grad_disp_old;
  /// The volumetric strain rate at the quadpoints
  MaterialProperty<Real> & _vol_strain_rate;
  /// The total volumetric strain at the quadpoints
  MaterialProperty<Real> & _vol_total_strain;
  // The strain and strain rate from Tensormechanics action
  const std::string _base_name;
  const MaterialProperty<RankTwoTensor> & _TenMech_total_strain;
  const MaterialProperty<RankTwoTensor> * _TenMech_strain_rate;
  // Extra stresses added to TensorMechanics action
   std::vector<const Function *> _extra_stress;
   MaterialProperty<RankTwoTensor> & _TenMech_extra_stress;

private:
  const Real _b;
  const Real _bu;
  bool _incremental;
};
