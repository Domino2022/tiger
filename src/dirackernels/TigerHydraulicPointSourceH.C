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

#include "TigerHydraulicPointSourceH.h"
#include "FEProblemBase.h"

registerMooseObject("TigerApp", TigerHydraulicPointSourceH);


InputParameters
TigerHydraulicPointSourceH::validParams()
{
  InputParameters params = DiracKernel::validParams();
  params.addParam<Real>("mass_flux",0.0,
        "The constant mass flow rate at this point (well bottom) in kg/s "
        "(negative is injection, positive is production)");
  params.addParam<FunctionName>("mass_flux_function", "The mass flow rate as a "
        "function of time at this point (well bottom) in kg/s (negative-valued "
        "function is injection, positive-valued function is production)");
  params.addRequiredParam<Point>("point", "The x,y,z coordinates of the "
        "injection or production well point");
  params.addParam<Real>("start_time", 0.0, "The time at which the source will "
        "start (the case of the constant flow rate)");
  params.addParam<Real>("end_time", 1.0e30, "The time at which the source will "
        "end (the case of the constant flow rate)");
  params.addClassDescription("Injection/Production well that adds (removes) "
        "fluid at the well point");
  return params;
}

TigerHydraulicPointSourceH::TigerHydraulicPointSourceH(
    const InputParameters & parameters)
  : DiracKernel(parameters),
    _mass_flux(getParam<Real>("mass_flux")),
    _p(getParam<Point>("point")),
    _start_time(getParam<Real>("start_time")),
    _end_time(getParam<Real>("end_time")),
    _rhof(getMaterialProperty<Real>("fluid_density"))
{
  _mass_flux_function = isParamValid("mass_flux_function") ?
                        &getFunction("mass_flux_function") : NULL;

  // Sanity check to ensure that the end_time is greater than the start_time
  if (_end_time <= _start_time)
    mooseError("Start time for TigerHydraulicPointSourceH is ",_start_time,
                " but it must be less than end time ",_end_time);
}

void
TigerHydraulicPointSourceH::addPoints()
{
  addPoint(_p);
}

Real
TigerHydraulicPointSourceH::computeQpResidual()
{
  // to make injection negative and compatible as a sourceterm
  Real factor = -1.0;

  if (isParamValid("mass_flux_function"))
      factor *= _mass_flux_function->value(_t, Point());
  else
  {
    /**
     * There are six cases for the start and end time in relation to t-dt and t.
     * If the interval (t-dt,t) is only partly but not fully within the (start,
     * end) interval, then the  mass_flux is scaled so that the total mass added
     * (or removed) is correct
     */
    if (_t < _start_time || _t - _dt >= _end_time)
      factor = 0.0;
    else if (_t - _dt < _start_time)
    {
      if (_t <= _end_time)
        factor *= (_t - _start_time) / _dt;
      else
        factor *= (_end_time - _start_time) / _dt;
    }
    else
    {
      if (_t <= _end_time)
        factor *= 1.0;
      else
        factor *= (_end_time - (_t - _dt)) / _dt;
    }
      factor *=_mass_flux;
  }
  // Negative sign to make a positive mass_flux as a source
  return -_test[_i][_qp] * factor /_rhof[_qp];
}
