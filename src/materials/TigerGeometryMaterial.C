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

#include "TigerGeometryMaterial.h"
#include "MooseMesh.h"
#include <cfloat>
#include "Function.h"

#define PI 3.141592653589793238462643383279502884197169399375105820974944592308

registerMooseObject("TigerApp", TigerGeometryMaterial);

InputParameters
TigerGeometryMaterial::validParams()
{
  InputParameters params = Material::validParams();

  params.addParam<RealVectorValue>("gravity", RealVectorValue(0,0,0),
        "The gravity acceleration vector (m/s^2)");
  params.addParam<FunctionName>("scale_factor", 1.0, "The scale factor for non-3D "
        "elements ( particularlly lower dimensional elements): if mesh is 3D"
        ", aperture for 2D elements (fractures) and diameter for 1D elements"
        " (wells) should be used; if mesh is 2D, height for 2D elements (2D"
        " matrix) and aperture times height for 1D elements (fractures) "
        "should be used; and if mesh is 1D, area for 1D elements (pipes or "
        "wells) should be used); ParsedFunctions can be used as well.");
  params.addClassDescription("Material for introducing geometrical properties "
        "of defined structural features (e.g unit, fracture and well)");

  return params;
}

TigerGeometryMaterial::TigerGeometryMaterial(const InputParameters & parameters)
  : Material(parameters),
    _gravity(declareProperty<RealVectorValue>("gravity_vector")),
    _rot_mat(declareProperty<RankTwoTensor>("lowerD_rotation_matrix")),
    _scale_factor(declareProperty<Real>("scale_factor")),
    _scale_factor0(getFunction("scale_factor")),
    _g(getParam<RealVectorValue>("gravity"))
{
}

void
TigerGeometryMaterial::computeQpProperties()
{
  _gravity[_qp] = _g;
  _scale_factor[_qp] = Scaling();

  if (_current_elem->dim() < _mesh.dimension())
    _rot_mat[_qp] = lowerDRotationMatrix(_current_elem->dim());
  else
    _rot_mat[_qp] = RankTwoTensor::Identity();
}

Real
TigerGeometryMaterial::Scaling()
{
  Real scale_factor = 1.0;

  switch (_mesh.dimension())
  {
    case 1 ... 2:
      scale_factor = _scale_factor0.value(_t, _q_point[_qp]);
      break;
    case 3:
      if (_current_elem->dim() == 2)
        // fracture aperture
        scale_factor = _scale_factor0.value(_t, _q_point[_qp]);
      else if (_current_elem->dim() == 1)
       // radius of well
        scale_factor = PI * _scale_factor0.value(_t, _q_point[_qp]) * _scale_factor0.value(_t, _q_point[_qp]) / 4.0;
      break;
  }

  return scale_factor;
}

RankTwoTensor
TigerGeometryMaterial::lowerDRotationMatrix(int dim)
{
  RealVectorValue xp, yp, zp;
  xp = _current_elem->point(1) - _current_elem->point(0);

  switch (dim)
  {
    case 1:
      for (unsigned int i = 0; i < 3; ++i)
        yp(i) = 0.0;
      if (std::fabs(xp(0)) > 0.0 && std::fabs(xp(1)) + std::fabs(xp(2)) < DBL_MIN)
        yp(2) = 1.0;
      else if (std::fabs(xp(1)) > 0.0 && std::fabs(xp(0)) + std::fabs(xp(2)) < DBL_MIN)
        yp(0) = 1.0;
      else if (std::fabs(xp(2)) > 0.0 && std::fabs(xp(0)) + std::fabs(xp(1)) < DBL_MIN)
        yp(1) = 1.0;
      else
      {
        for (unsigned int i = 0; i < 3; ++i)
          if (std::fabs(xp(i)) > 0.0)
          {
            yp(i) = -xp(i);
            break;
          }
      }
      zp = xp.cross(yp);
      yp = zp.cross(xp);
      break;

    case 2:
      yp = _current_elem->point(2) - _current_elem->point(1);
      zp = xp.cross(yp);
      if (!((std::fabs(zp(0)) + std::fabs(zp(1)))/zp.norm() < DBL_MIN)) //horizontal fracture check
        xp = RealVectorValue(0.,0.,1.).cross(zp);
      else
        xp = RealVectorValue(1.,0.,0.);
      yp = zp.cross(xp);
      break;
  }

  RankTwoTensor _rm;
  for (unsigned int i = 0; i < 3; ++i)
  {
    (_rm)(i, 0) = xp(i) / xp.norm();
    (_rm)(i, 1) = yp(i) / yp.norm();
    (_rm)(i, 2) = zp(i) / zp.norm();
  }

  return _rm;
}
