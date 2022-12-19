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

#include "TigerThermalSourceKernelT.h"
#include "Function.h"

registerMooseObject("TigerApp", TigerThermalSourceKernelT);

InputParameters
TigerThermalSourceKernelT::validParams()
{
  InputParameters params = Kernel::validParams();

  params.addParam<Real>("value", 1.0, "Constant heat source (sink) (W/m^3) "
        "(positive is a source, and negative is a sink) or a multiplier "
        "for the the provided function");
  params.addParam<FunctionName>("function", "1.0", "Heat source (sink) as "
        "a function (W/m^3) (positive is a source, and negative is a sink)");
  return params;
}

TigerThermalSourceKernelT::TigerThermalSourceKernelT(const InputParameters & parameters)
  : Kernel(parameters),
    _scale(getParam<Real>("value")),
    _function(getFunction("function")),
    _scale_factor(getMaterialProperty<Real>("scale_factor")),
    _SUPG_p(getMaterialProperty<RealVectorValue>("thermal_petrov_supg_p_function")),
    _SUPG_ind(getMaterialProperty<bool>("thermal_supg_indicator"))
{
}

Real
TigerThermalSourceKernelT::computeQpResidual()
{
  Real factor = -_scale * _function.value(_t, _q_point[_qp]);

  Real test = 0.0;

  if (_SUPG_ind[_qp])
    test = _test[_i][_qp] + _SUPG_p[_qp] * _grad_test[_i][_qp];
  else
    test = _test[_i][_qp];

  return _scale_factor[_qp] * test * factor;
}
