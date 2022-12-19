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

#ifndef TIGERGEOMETRYMATERIAL_H
#define TIGERGEOMETRYMATERIAL_H

#include "Material.h"
#include "RankTwoTensor.h"

 

class TigerGeometryMaterial : public Material
{
public:
  static InputParameters validParams();
  TigerGeometryMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;
  // Calculates rotation matrix for lower dimensional elements
  RankTwoTensor lowerDRotationMatrix(int dim);
  // computes scaling factor for lower dimensional elements
  Real Scaling();

  // Gravity vector
  MaterialProperty<RealVectorValue> & _gravity;
  // Material for rotation matrix for local cordinates
  MaterialProperty<RankTwoTensor> & _rot_mat;
  // scaling factor
  MaterialProperty<Real> & _scale_factor;
  // Initial scaling factor
  const Function & _scale_factor0;
  
private:
  // Gravity vector
  RealVectorValue _g;
};

#endif /* TIGERGEOMETRYMATERIAL_H */
