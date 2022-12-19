/**************************************************************************/
/*  TIGER - Hydro-thermal sImulator GEothermal Reservoirs                 */
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

#ifndef TIGERSOLUTEMATERIALS_H
#define TIGERSOLUTEMATERIALS_H

#include "RankTwoTensor.h"
#include "Material.h"
#include "TigerSUPG.h"
#include "Function.h"

 

class TigerSoluteMaterialS : public Material
{
public:
  static InputParameters validParams();
  TigerSoluteMaterialS(const InputParameters & parameters);

private:
  // enum to select type of advection velocity
  enum AT {pure_diffusion, darcy_velocity, user_velocity, darcy_user_velocities};
  MooseEnum _at;

  // boolean selecting mode for upwinding and critical numbers output
  bool _has_PeCr;
  bool _has_supg;
  // userdefined factor to manually modify upwinding coefficient
  Real _supg_scale;
  // userdefined velocity vector function for advection
  const Function * _vel_func;

protected:
  virtual void computeQpProperties() override;
  RankTwoTensor DispersionTensorCalculator(const RealVectorValue & darcy_v, Real const & dispersion_l, Real const & dispersion_tr, int dim, int dimMesh, Real diffusion_factor);

  // Peclet number upon request
  MaterialProperty<Real> * _Pe;
  // Courant number upon request
  MaterialProperty<Real> * _Cr;
  // coefficient for solute time kernel
  MaterialProperty<Real> & _TimeKernelS;
  // indicator to inform kernels for considering upwinding
  MaterialProperty<bool> & _SUPG_ind;
  // indicator to inform kernels for considering derivative of darcy velocity
  MaterialProperty<bool> & _av_ind;
  // advection velocity
  MaterialProperty<RealVectorValue> & _av;
  // upwinding coefficient
  MaterialProperty<RealVectorValue> & _SUPG_p;

  // imported props from TigerGeometryMaterial
  const MaterialProperty<Real> & _n;
  const MaterialProperty<RankTwoTensor> & _rot_mat;

  // imported darcy velocity from TigerHydraulicMaterial
  const MaterialProperty<RealVectorValue> * _dv;

  // userobject to calculate upwinding
  const TigerSUPG * _supg_uo;

  // molecular diffusion as input parameter
  Real _diffusion_molecular;
  // initial molecular dispersivity
  Real _disp_l;
  Real _disp_t;
  // Formation factor
  Real _formation_factor;
  // Tensors for internal calculation, will be transferred
  RankTwoTensor _dispersion_ten = RankTwoTensor();
  RankTwoTensor _diffusion_ten = RankTwoTensor();
  // Tensor for Handover to the Kernels and output as AuxVariables
  MaterialProperty<RankTwoTensor> & _dispersion_tensor;
  // Relativ Diffusion depending on porosity
  MaterialProperty<Real> & _diffusion_factor;

  // Tensor for Handover of combined diffusion and dispersion to Kernels
  MaterialProperty<RankTwoTensor> & _diffdisp;

  // Neumann number Fo
  MaterialProperty<Real> & _Fo;
  // Peclet number
  MaterialProperty<Real> & _PeDisp;

};

#endif /* TIGERSOLUTEMATERIALS_H */
