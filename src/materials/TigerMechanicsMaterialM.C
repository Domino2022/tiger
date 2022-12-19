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

#include "TigerMechanicsMaterialM.h"
#include "MooseMesh.h"
#include "Function.h"

registerMooseObject("TigerApp", TigerMechanicsMaterialM);

InputParameters
TigerMechanicsMaterialM::validParams()
{
  InputParameters params = Material::validParams();

  params.addParam<Real>("biot_coefficient", 1.0,
        "Biot's coefficient for poromechanics");
  params.addParam<Real>("solid_bulk_modulus", 1e+99,
        "Solid bulk modulus for poromechanics");
  params.addCoupledVar("disps",
        "The displacements variables (they are required if the incremental is false)");
  params.addRequiredParam<bool>("incremental",
        "Incremental or total strain approach similar to TensorMechanics Action");
  params.addParam<std::string>("base_name", "the identical base name provided "
        "in TensorMechanics Action");
  params.addParam<std::vector<FunctionName>>("extra_stress_vector",
        "Vector of values defining the extra stress "
        "to add, in order 11, 22, 33. Functions can be provided as well.");
  params.addClassDescription("Mechanics material for mechanics kernels");

  return params;
}

TigerMechanicsMaterialM::TigerMechanicsMaterialM(const InputParameters & parameters)
  : Material(parameters),
    _biot(declareProperty<Real>("biot_coefficient")),
    _solid_bulk(declareProperty<Real>("solid_bulk")),
    _ndisp(coupledComponents("disps")),
    _vol_strain_rate(declareProperty<Real>("volumetric_strain_rate_HM")),
    _vol_total_strain(declareProperty<Real>("total_volumetric_strain_HM")),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _TenMech_total_strain(getMaterialProperty<RankTwoTensor>(_base_name + "total_strain")),
    _TenMech_extra_stress(declareProperty<RankTwoTensor>(_base_name + "extra_stress")),
    _b(getParam<Real>("biot_coefficient")),
    _bu(getParam<Real>("solid_bulk_modulus")),
    _incremental(getParam<bool>("incremental"))
{
  _TenMech_strain_rate = _incremental ?
              &getMaterialProperty<RankTwoTensor>(_base_name + "strain_rate") : NULL;

  if (!_incremental && _is_transient)
  {
    if (_ndisp != _mesh.dimension())
      paramError("disps", "The number of displacement variables supplied must "
                  "match the mesh dimension or they are not supplied at all"
                  " in the case of total strain approach");

    _grad_disp = coupledGradients("disps");
    _grad_disp_old = coupledGradientsOld("disps");
    // set unused dimensions to zero
    _grad_disp.resize(3, &_grad_zero);
    _grad_disp_old.resize(3, &_grad_zero);
  }
}

void
TigerMechanicsMaterialM::computeQpProperties()
{
  _biot[_qp] = _b;
  _solid_bulk[_qp] = _bu;
  _vol_total_strain[_qp] = _TenMech_total_strain[_qp].trace();

// Extra stress can be added and included in TensorMechanics Action
  const std::vector<FunctionName> & stress_fct(
      getParam<std::vector<FunctionName>>("extra_stress_vector"));
  const unsigned num = stress_fct.size();
  if (!(num == 0 || num == 3))
    mooseError("Please supply either zero or 3 extra stresses. "
               "You supplied ", num,".\n");

  _extra_stress.resize(num);
  for (unsigned i = 0; i < num; ++i)
    _extra_stress[i] = &getFunctionByName(stress_fct[i]);

  if (_extra_stress.size() == 3)
    for (unsigned i = 0; i < num; ++i)
      _TenMech_extra_stress[_qp](i, i) = _extra_stress[i]->value(_t, _q_point[_qp]);

  if (_incremental && _is_transient)
    _vol_strain_rate[_qp] = (*_TenMech_strain_rate)[_qp].trace();

//
  if (!_incremental && _is_transient)
  {
    RankTwoTensor A   ((*_grad_disp[0])[_qp],
                       (*_grad_disp[1])[_qp],
                       (*_grad_disp[2])[_qp]); // Deformation gradient
    RankTwoTensor Abar((*_grad_disp_old[0])[_qp],
                       (*_grad_disp_old[1])[_qp],
                       (*_grad_disp_old[2])[_qp]); // Old Deformation gradient
    A -= Abar; // A = grad_disp - grad_disp_old
    RankTwoTensor total_strain_increment = 0.5 * (A + A.transpose());
    _vol_strain_rate[_qp] = total_strain_increment.trace() / _dt;
  }
}
