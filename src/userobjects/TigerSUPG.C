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

#include "TigerSUPG.h"

registerMooseObject("TigerApp", TigerSUPG);

InputParameters
TigerSUPG::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  MooseEnum EffLen("min max average directional_min directional_max directional_average", "average");
  params.addParam<MooseEnum>("effective_length", EffLen,
        "The characteristic element length for SU/PG.");
  MooseEnum Method("optimal doubly_asymptotic critical transient_brooks transient_tezduyar", "optimal");
  params.addParam<MooseEnum>("supg_coeficient", Method,
        "The method for calculating SU/PG coefficent (tau)");

  return params;
}

TigerSUPG::TigerSUPG(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _eff_len(getParam<MooseEnum>("effective_length")),
    _method(getParam<MooseEnum>("supg_coeficient"))
{
}

void
TigerSUPG::execute(){}

void
TigerSUPG::initialize(){}

void
TigerSUPG::finalize(){}

void
TigerSUPG::PeCrNrsCalculator(const Real & diff, const Real & dt, const Elem * ele, const RealVectorValue & v, Real & PeNr, Real & CrNr) const
{
  Real v_n = v.norm();

  if (v_n != 0.0)
  {
    Real h_n = EEL(ele).norm();

    if (diff == 0.0)
      PeNr = std::numeric_limits<Real>::max();
    else
      PeNr = 0.5 * v_n * h_n / diff;

    CrNr = v_n * dt / h_n;
  }
  else
  {
    PeNr = 0.0;
    CrNr = 0.0;
  }
}

void
TigerSUPG::SUPGCalculator(const Real & diff, const Real & dt, const Elem * ele, const RealVectorValue & v, RealVectorValue & SUPG_coeff, Real & alpha, Real & CrNr) const
{
  Real v_n = v.norm();

  if (v_n != 0.0)
  {
    if (diff == 0.0)
      alpha = std::numeric_limits<Real>::max();

    if (_eff_len<EL::directional_min)
    {
      Real h_n = EEL(ele).norm();

      if (diff != 0)
        alpha = 0.5 * v_n * h_n / diff;

      CrNr = v_n * dt / h_n;
      SUPG_coeff = tau(alpha, diff, dt, v_n, h_n) * v;
    }
    else
    {
      RealVectorValue a,h;
      h = ActualEEL(ele);

      if (diff != 0)
      {
        // vectorial peclet number
        a = 0.5 / diff * RealVectorValue(std::fabs(v(0)*h(0)), std::fabs(v(1)*h(1)), std::fabs(v(2)*h(2)));
        alpha = a.norm();
      }
      else
        a = std::numeric_limits<Real>::max() * RealVectorValue(1.0,1.0,1.0);

      CrNr = v_n * dt / h.norm();
      SUPG_coeff = Actualtau(a, diff, dt, v, h) * v;
    }
  }
  else
  {
    alpha = 0.0;
    SUPG_coeff.zero();
  }
}

Real
TigerSUPG::tau(const Real & alpha, const Real & diff, const Real & dt, const Real & v, const Real & h) const
{
  Real tau = 0.0;

  switch (_method)
  {
    case M::optimal:
      tau += Optimal(alpha) * h / (2.0 * v);
      break;
    case M::doubly_asymptotic:
      tau += DoublyAsymptotic(alpha) * h / (2.0 * v);
      break;
    case M::critical:
      tau += Critical(alpha) * h / (2.0 * v);
      break;
    case M::transient_brooks:
      // Brooks & Hughes 1982
      tau += Optimal(alpha) * h / (sqrt(15.0) * v);
      break;
    case M::transient_tezduyar:
      tau += Temporal(v, h, diff, dt);
      break;
  }

  return tau;
}

Real
TigerSUPG::Actualtau(const RealVectorValue & alpha, const Real & diff, const Real & dt, const RealVectorValue & v, const RealVectorValue & h) const
{
  Real tau = 0.0;

  switch (_method)
  {
    case M::optimal:
      tau = Optimal(alpha(0)) * std::fabs(h(0) * v(0)) +
            Optimal(alpha(1)) * std::fabs(h(1) * v(1)) +
            Optimal(alpha(2)) * std::fabs(h(2) * v(2));
      tau *= 0.5 * v.norm_sq();
      break;
    case M::doubly_asymptotic:
      tau = DoublyAsymptotic(alpha(0)) * std::fabs(h(0) * v(0)) +
            DoublyAsymptotic(alpha(1)) * std::fabs(h(1) * v(1)) +
            DoublyAsymptotic(alpha(2)) * std::fabs(h(2) * v(2));
      tau *= 0.5 * v.norm_sq();
      break;
    case M::critical:
      tau = Critical(alpha(0)) * std::fabs(h(0) * v(0)) +
            Critical(alpha(1)) * std::fabs(h(1) * v(1)) +
            Critical(alpha(2)) * std::fabs(h(2) * v(2));
      tau *= 0.5 * v.norm_sq();
      break;
    case M::transient_brooks:
      // Brooks & Hughes 1982
      tau = Optimal(alpha(0)) * std::fabs(h(0) * v(0)) +
            Optimal(alpha(1)) * std::fabs(h(1) * v(1)) +
            Optimal(alpha(2)) * std::fabs(h(2) * v(2));
      tau /= sqrt(15.0) * v.norm_sq();
      break;
    case M::transient_tezduyar:
      tau += ActualTemporal(v, h, diff, dt);
      break;
  }

  return tau;
}

Real
TigerSUPG::Optimal(const Real & alpha) const
{
  Real s = 0.0;
  if (alpha < 0.01)
    s = alpha * (1.0 / 3.0 + alpha * alpha * (-1.0 / 45.0 + 18.0 / 8505.0 * alpha * alpha)); //taylor expansion
  else
    s = 1.0 / std::tanh(alpha) - 1.0 / alpha;
  return s;
}

Real
TigerSUPG::Temporal(const Real & v, const Real & h, const Real & diff, const Real & dt) const
{
  Real s1, s2, s3;
  // Tezduyar & Osawa 2000
  s1 = 2.0 * v / h;
  s2 = (dt!=0.0 ? 2.0 / dt : 0.0);
  s3 = 4.0 * diff / ( h * h );

  return (1.0 / sqrt(s1 * s1 + s2 * s2 + s3 * s3));
}

Real
TigerSUPG::ActualTemporal(const RealVectorValue & v, const RealVectorValue & h, const Real & diff, const Real & dt) const
{
  Real s1, s2, s3;
  // Tezduyar & Osawa 2000
  s1 = 2.0 * ( std::fabs(v(0) / h(0)) + std::fabs(v(1) / h(1)) + std::fabs(v(2) / h(2)));
  s2 = (dt!=0.0 ? 2.0 / dt : 0.0);
  s3 = 4.0 * diff * (1.0 / (h(0) * h(0)) + 1.0 / (h(1) * h(1)) + 1.0 / (h(2) * h(2)));

  return (1.0 / sqrt(s1 * s1 + s2 * s2 + s3 * s3));
}

Real
TigerSUPG::DoublyAsymptotic(const Real & alpha) const
{
  Real s = 0.0;
  if (alpha <= 3.0)
    s = alpha / 3.0;
  else
    s = 1.0;
  return s;
}

Real
TigerSUPG::Critical(const Real & alpha) const
{
  Real s = 0.0;
  if (alpha <= 1.0)
    s = 0.0;
  else
    s = 1.0 - 1.0 / alpha;
  return s;
}

RealVectorValue
TigerSUPG::EEL(const Elem * ele) const
{
  RealVectorValue L;
  L.zero();

  if (ele->dim() == 1)
    L(0) += ele->volume();
  else
  {
    switch (_eff_len)
    {
      case EL::min:
        L(0) += ele->hmin();
        break;
      case EL::max:
        L(0) += ele->hmax();
        break;
      case EL::average:
        L(0) += 0.5 * (ele->hmin() + ele->hmax());
        break;
    }
  }
  return L;
}

RealVectorValue
TigerSUPG::ActualEEL(const Elem * ele) const
{
  RealVectorValue L;

  if (_eff_len == EL::directional_min)
    L = RealVectorValue(std::numeric_limits<Real>::max());
  else
    L = RealVectorValue(0.0);

  Real counter = 0;

  for (unsigned int n_outer=0; n_outer<ele->n_vertices(); n_outer++)
    for (unsigned int n_inner=n_outer+1; n_inner<ele->n_vertices(); n_inner++)
    {
      const Point diff = (ele->point(n_outer) - ele->point(n_inner));

      switch (_eff_len)
      {
        case EL::directional_min:
          L(0) = std::min( L(0) , std::fabs(diff(0)) );
          L(1) = std::min( L(1) , std::fabs(diff(1)) );
          L(2) = std::min( L(2) , std::fabs(diff(2)) );
          break;
        case EL::directional_max:
          L(0) = std::max( L(0) , std::fabs(diff(0)) );
          L(1) = std::max( L(1) , std::fabs(diff(1)) );
          L(2) = std::max( L(2) , std::fabs(diff(2)) );
          break;
        case EL::directional_average:
          L(0) += std::fabs(diff(0));
          L(1) += std::fabs(diff(1));
          L(2) += std::fabs(diff(2));
          break;
      }
      counter++;
    }

  if (_eff_len == EL::directional_average) L /= counter;

  return L;
}
