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

#ifndef TIGERSUPG_H
#define TIGERSUPG_H

#include "GeneralUserObject.h"
#include "RankTwoTensor.h"

 

class TigerSUPG : public GeneralUserObject
{
public:
  static InputParameters validParams(); 
  TigerSUPG(const InputParameters & parameters);

  virtual void execute();
  virtual void initialize();
  virtual void finalize();

  void PeCrNrsCalculator(const Real & diff, const Real & dt, const Elem * ele, const RealVectorValue & v, Real & PeNr, Real & CrNr) const;
  void SUPGCalculator(const Real & diff, const Real & dt, const Elem * ele, const RealVectorValue & v, RealVectorValue & SUPG_coeff, Real & alpha, Real & CrNr) const;
  Real tau(const Real & alpha, const Real & diff, const Real & dt, const Real & v, const Real & h) const;
  Real Actualtau(const RealVectorValue & alpha, const Real & diff, const Real & dt, const RealVectorValue & v, const RealVectorValue & h) const;

protected:
  RealVectorValue EEL(const Elem * ele) const;
  RealVectorValue ActualEEL(const Elem * ele) const;
  Real Optimal(const Real & alpha) const;
  Real Temporal(const Real & v, const Real & h, const Real & diff, const Real & dt) const;
  Real ActualTemporal(const RealVectorValue & v, const RealVectorValue & h, const Real & diff, const Real & dt) const;
  Real DoublyAsymptotic(const Real & alpha) const;
  Real Critical(const Real & alpha) const;

  MooseEnum _eff_len;
  MooseEnum _method;
  enum EL {min, max, average, directional_min, directional_max, directional_average};
  enum M {optimal, doubly_asymptotic, critical, transient_brooks, transient_tezduyar};
};

#endif /* TIGERSUPG_H */
