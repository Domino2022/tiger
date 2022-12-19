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

#include "TigerHydraulicOutflowH.h"

registerMooseObject("TigerApp", TigerHydraulicOutflowH);


InputParameters
TigerHydraulicOutflowH::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  return params;
}

TigerHydraulicOutflowH::TigerHydraulicOutflowH(const InputParameters & parameters)
  : IntegratedBC(parameters),
  _k_vis(getMaterialProperty<RankTwoTensor>("permeability_by_viscosity"))
{
}

Real
TigerHydraulicOutflowH::computeQpResidual()
{
  return -_test[_i][_qp] * _k_vis[_qp] * _grad_u[_qp] * _normals[_qp];
}

Real
TigerHydraulicOutflowH::computeQpJacobian()
{
  return -_test[_i][_qp] * _k_vis[_qp] * _grad_phi[_j][_qp] * _normals[_qp];
}
