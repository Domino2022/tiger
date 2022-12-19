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

#include "TigerDarcyVelocityH.h"

registerMooseObject("TigerApp", TigerDarcyVelocityH);


InputParameters
TigerDarcyVelocityH::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addRequiredCoupledVar("pressure", "Pore pressure nonlinear variable");
  MooseEnum component("x y z", "x");
  params.addRequiredParam<MooseEnum>("component", component,
        "The Darcy velocity component to compute");
  return params;
}

TigerDarcyVelocityH::TigerDarcyVelocityH(const InputParameters & parameters)
  : AuxKernel(parameters),
    _grad_p(coupledGradient("pressure")),
    _k_vis(getMaterialProperty<RankTwoTensor>("permeability_by_viscosity")),
    _rho_f(getMaterialProperty<Real>("fluid_density")),
    _g(getMaterialProperty<RealVectorValue>("gravity_vector")),
    _component(getParam<MooseEnum>("component"))
{
}

Real
TigerDarcyVelocityH::computeValue()
{
  return -(_k_vis[_qp] * (_grad_p[_qp] - _rho_f[_qp] * _g[_qp]))(_component);
}
