/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef EX_USEFULFUNCTIONS
#define EX_USEFULFUNCTIONS

#include <vector>

#include "wx/wx.h"

class Expression;

namespace UsefulFunctions
{
  void LeastSquaresFit( Expression *, wxString &,
                        std::vector<double> &, std::vector<double> &,
                        std::vector<double> &, std::vector<double> &,
                        std::vector<double> &, std::vector<double> &,
                        unsigned int, double, double &, double &,
                        std::vector< std::vector<double> > &, bool,
                        bool, bool, bool, unsigned int & );
  void SolveLinearEquations( std::vector<double> &,
                             std::vector< std::vector<double> > & );

  void LQline( std::vector<double> const &, std::vector<double> const &,
               double &, double &, double &, double & );
  void LQline( std::vector<double> const &, std::vector<double> const &,
               double &, double &, double &, double &, double & );

  void EllipseFit( std::vector<double> const &, std::vector<double> const &, double const,
                   double &, double &, double &, double &, double & );

  double Median( std::vector<double> & );

  void Scale1( double &, double &, double &, int, double, double );
  void Scale2( double &, double &, double &, int, double, double );
  //
  double GetSimpleRandomNumber();
  //
  void QuickSort( std::vector<double> &, bool );
  void QuickSort( std::vector<double> &, std::vector<double> &, bool );
  //
  void MinMax( std::vector<double> const &, int, int, double &, double & );
  void MinMax( std::vector<double> const &, int, int, int, int, int,
               double &, double & );
  //
  bool InsidePolygon( double, double, std::vector<double> const &,
                      std::vector<double> const & );
  double exp10( double, int );
  double Factorial( int );
  double Chisq( double, int );
  double ChisqInverse( double, int );
  double Normal( double, double, double );
  double Error( double );
  double ErrorInverse( double );
  double ComplementaryError( double );
  double ComplementaryErrorInverse( double );
  double Gauss( double );
  double GaussInverse( double );
  double GaussInverse2( double );
  int Sign( int, int );
  double Sign( double, double );
  void Splsmooth( std::vector<double> &, std::vector<double> &,
                  std::vector<double> &, std::vector<double> &,
                  std::vector<double> & );
  void SplineSmooth( std::vector<double> &, std::vector<double> &,
                     std::vector<double> &, std::vector<double> &,
                     std::vector<double> &, std::vector<double> & );
  void SplineIntegral( std::vector<double> &, std::vector<double> &,
                       std::vector<double> &, std::vector<double> & );
  void Splinterp( std::vector<double> const &, std::vector<double> const &,
                  std::vector<double> &, std::vector<double> & );
  void SplineInterpolate( std::vector<double> const &,
                          std::vector<double> const &,
                          std::vector<double> &, std::vector<double> & );
  void SplineDerivative( std::vector<double> &, std::vector<double> &,
                         std::vector<double> &, std::vector<double> &,
                         std::size_t );
  double SplineDefiniteIntegral( std::vector<double> &,
                                 std::vector<double> &,
                                 double const, double );
  void SplineSetup( std::vector<double> const &,
                    std::vector<double> const &,
                    std::vector<double> &, std::vector<double> &,
                    double, double );
  void LinearInterpolate( std::vector<double> &, std::vector<double> &,
                          std::vector<double> &, std::vector<double> & );
  void LagrangeInterpolate( std::vector<double> &, std::vector<double> &,
                            std::vector<double> &, std::vector<double> & );
  void LagrangeDerivative( std::vector<double> &, std::vector<double> &,
                           std::vector<double> &, std::vector<double> &,
                           int, int );
  void FritchCarlsonInterpolate( std::vector<double> &,
                                 std::vector<double> &,
                                 std::vector<double> &,
                                 std::vector<double> &, int );
  double Deriv3( double, double, double, double, double, double, double );
  double CompleteGamma( double );
  double ReciprocalGamma( double );
  double IncompleteGamma( double, double );
  double IncompleteGamma2( double, double );
  double ComplementaryIncompleteGamma( double, double );
  double ComplementaryIncompleteGamma2( double, double, double );
  double LogComplementaryIncompleteGamma( double, double, double );
  double TricomiSIncomplete( double, double );
  double LogTricomiS( double, double, double );
  double TricomiS( double, double, double, int );
  double LnAbsGamma( double );
  double LnGamma( double );
  double IncompleteBeta( double, double, double );
  double LnCompleteBeta( double, double );
  double LnRel( double );
  int Initds( double const *, int, double );
  double LogGammaCorrection( double );
  double NTermChebyshevSeries( double, double const *, int );
  void ComputeGammaBounds( double &, double & );
  double Airy( double );
  void AiryModulusPhase( double, double &, double & );
  double AiryNegative( double );
  double BesselI0( double );
  double EBesselI0( double );
  double BesselI1( double );
  double EBesselI1( double );
  double BesselJ0( double );
  double BesselJ1( double );
  void BesselJn( double, double, int, int, double * );
  double BesselK0( double );
  double EBesselK0( double );
  double BesselK1( double );
  double EBesselK1( double );
  double BesselY0( double );
  double BesselY1( double );
  void Bessel0ModulusPhase( double, double &, double & );
  void Bessel1ModulusPhase( double, double &, double & );
  double CompleteBeta( double, double );
  double Psi( double );
  double DiGamma( double );
  double AssociatedLegendreFunctions( int, int, double, bool );
  double BinomialCoefficient( int, int );
  double ChebyshevPolynomial( int, double );
  double HermitePolynomial( int, double );
  double JacobiPolynomial( double, double, int, double );
  double PoissonCharlierPolynomial( double, int, double );
  double LaguerrePolynomial( int, double );
  double LegendrePolynomial( int, double );
  double UnnormalizedLegendre( int, int, double );
  double NormalizedLegendre( int, int, double );
  double Rademacher( int, double );
  double BinaryOrderedWalsh( int, double );
  double VoigtProfile( double, double, double, double );
  double Struve0( double );
  double Struve1( double );
  double Hypergeometric2F1( double, double, double, double );
  double LogarithmicConfluentHypergeometric( double, double, double );
  double Chu( double, double, double );
  double PochhammerSSymbol( double, double );
  double PochhammerSSpecial( double, double );
  double RelativeErrorExponential( double );
  double BivariateNormalDistribution( double, double, double );
  double StudentTDistribution( double, int );
  double StudentTDistributionInverse( double, int );
  double NormalizedTina( double, double, double, double );
  double CosineIntegral( double );
  double SineIntegral( double );
  double DawsonIntegral( double );
  double Airy2( double );
  double DPBairy( double );
  double DiLog( double );
  double IncompleteEllipticIntegral1( double, double );
  double CompleteEllipticIntegral1( double );
  double IncompleteEllipticIntegral2( double, double );
  double CompleteEllipticIntegral2( double );
  double ExponentialIntegral( double );
  double ExponentialIntegralN( double, int );
  double Fn1( double, int );
  double FermiDirac( double, double );
  double FisherDistribution( int, int, double );
  double FresnelC( double );
  double FresnelCAssociated( double );
  double FresnelS( double );
  double FresnelSAssociated( double );
  double Ber( double );
  double Bei( double );
  double Ker( double );
  double Kei( double );
  double ClebschGordanCoefficient( double, double, double, double,
                                   double, double );
  double Wigner3JSymbol( double, double, double, double, double, double );
  double Wigner6JSymbol( double, double, double, double, double, double );
  double Wigner9JSymbol( double, double, double, double, double, double,
                         double, double, double );
  double RacahCoefficient( double, double, double, double,
                           double, double );
  double JahnUFunction( double, double, double, double, double, double );
  int LUDecomposition( std::vector<double> &, std::vector<int> & );
  void LUSubstitution( std::vector<double> const &,
                       std::vector<int> const &, std::vector<double> & );
  void Eigen( std::vector<double> & );
  int NextPrime( int );
  void ComplexFourierTransform( std::vector<double> &,
                                std::vector<double> &, int, int, bool );
  void SavitzkyGolay( std::vector<double> &, std::vector<double> &,
                      std::size_t const, std::size_t const );
  void SavGol( std::vector<double> &, std::size_t, std::size_t,
               std::size_t, std::size_t );
  void LudcmpSvg( double *, std::size_t, std::size_t, int * );
  void LubksbSvg( double *, std::size_t, std::size_t, int *, double * );
  double Poisson( double, int );
  //
  void WindowClip( double, double, double, double,
                   double, double, double, double,
                   double *, double *, int & );
  bool SegmentCross( double, double, double, double,
                     double, double, double, double,
                     double &, double &, double & );
  void BivariateInterpolation( std::vector<double> const &,
                               std::vector<double> const &,
                               std::vector<double> const &,
                               std::vector<double> const &,
                               std::vector<double> const &,
                               std::vector<double> & );
  void BivariateSmoothing( std::vector<double> const &,
                           std::vector<double> const &,
                           std::vector<double> const &, int, int,
                           std::vector<double> &,
                           std::vector<double> &,
                           std::vector<double> & );
}
#endif
