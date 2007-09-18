/*
Copyright (C) 2005 Joseph L. Chuma, TRIUMF

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
#include "FunctionTable.h"
#include "FunctionDefinition.h"
#include "EExpressionError.h"

#include "FCN_abs.h"
#include "FCN_acosd.h"
#include "FCN_acosh.h"
#include "FCN_acos.h"
#include "FCN_acotd.h"
#include "FCN_acoth.h"
#include "FCN_acot.h"
#include "FCN_acscd.h"
#include "FCN_acsch.h"
#include "FCN_acsc.h"
#include "FCN_adev.h"
#include "FCN_aerfc.h"
#include "FCN_aerf.h"
#include "FCN_agauss.h"
#include "FCN_airy.h"
#include "FCN_area.h"
#include "FCN_asecd.h"
#include "FCN_asech.h"
#include "FCN_asec.h"
#include "FCN_asind.h"
#include "FCN_asinh.h"
#include "FCN_asin.h"
#include "FCN_atan2d.h"
#include "FCN_atan2.h"
#include "FCN_atand.h"
#include "FCN_atanh.h"
#include "FCN_atan.h"
#include "FCN_bei.h"
#include "FCN_ber.h"
#include "FCN_besi0.h"
#include "FCN_besi1.h"
#include "FCN_besj0.h"
#include "FCN_besj1.h"
#include "FCN_besk0.h"
#include "FCN_besk1.h"
#include "FCN_besy0.h"
#include "FCN_besy1.h"
#include "FCN_beta.h"
#include "FCN_betain.h"
#include "FCN_binom.h"
#include "FCN_biry.h"
#include "FCN_bivarnor.h"
#include "FCN_bivinterp.h"
#include "FCN_bivsmooth.h"
#include "FCN_canopen.h"
#include "FCN_char.h"
#include "FCN_cheby.h"
#include "FCN_chisq.h"
#include "FCN_chisqi.h"
#include "FCN_chlogu.h"
#include "FCN_clebsg.h"
#include "FCN_clen.h"
#include "FCN_convolute.h"
#include "FCN_cosd.h"
#include "FCN_cosh.h"
#include "FCN_cos.h"
#include "FCN_cosint.h"
#include "FCN_cotd.h"
#include "FCN_coth.h"
#include "FCN_cot.h"
#include "FCN_cscd.h"
#include "FCN_csch.h"
#include "FCN_csc.h"
#include "FCN_date.h"
#include "FCN_dawson.h"
#include "FCN_deconvolute.h"
#include "FCN_deriv.h"
#include "FCN_det.h"
#include "FCN_digamma.h"
#include "FCN_dilog.h"
#include "FCN_eigen.h"
#include "FCN_ei.h"
#include "FCN_einellic.h"
#include "FCN_ellice.h"
#include "FCN_ellick.h"
#include "FCN_eqs.h"
#include "FCN_erfc.h"
#include "FCN_erf.h"
#include "FCN_evaluate.h"
#include "FCN_exist.h"
#include "FCN_expand.h"
#include "FCN_exp.h"
#include "FCN_expint.h"
#include "FCN_expn.h"
#include "FCN_factorial.h"
#include "FCN_ferdirac.h"
#include "FCN_fft.h"
#include "FCN_finellic.h"
#include "FCN_fisher.h"
#include "FCN_fold.h"
#include "FCN_frec1.h"
#include "FCN_frec2.h"
#include "FCN_fres1.h"
#include "FCN_fres2.h"
#include "FCN_gammacin.h"
#include "FCN_gamma.h"
#include "FCN_gammain.h"
#include "FCN_gammatin.h"
#include "FCN_gammln.h"
#include "FCN_gammq.h"
#include "FCN_gauss.h"
#include "FCN_gaussin.h"
#include "FCN_gaussj.h"
#include "FCN_gmean.h"
#include "FCN_heaviside.h"
#include "FCN_hermite.h"
#include "FCN_hypgeo.h"
#include "FCN_ichar.h"
#include "FCN_iclose.h"
#include "FCN_identity.h"
#include "FCN_iequal.h"
#include "FCN_ifft.h"
#include "FCN_imax.h"
#include "FCN_imin.h"
#include "FCN_index.h"
#include "FCN_integral.h"
#include "FCN_interpolate.h"
#include "FCN_int.h"
#include "FCN_inverse.h"
#include "FCN_jacobi.h"
#include "FCN_jahnuf.h"
#include "FCN_join.h"
#include "FCN_kei.h"
#include "FCN_ker.h"
#include "FCN_kurt.h"
#include "FCN_laguerre.h"
#include "FCN_lcase.h"
#include "FCN_legendre.h"
#include "FCN_len.h"
#include "FCN_lnagamma.h"
#include "FCN_lnbeta.h"
#include "FCN_ln.h"
#include "FCN_log10.h"
#include "FCN_logam.h"
#include "FCN_log.h"
#include "FCN_max.h"
#include "FCN_mean.h"
#include "FCN_median.h"
#include "FCN_min.h"
#include "FCN_mod.h"
#include "FCN_nes.h"
#include "FCN_nint.h"
#include "FCN_normal.h"
#include "FCN_null.h"
#include "FCN_pdiff.h"
#include "FCN_pfactors.h"
#include "FCN_plmn.h"
#include "FCN_plmu.h"
#include "FCN_poica.h"
#include "FCN_poisson.h"
#include "FCN_prob.h"
#include "FCN_prod.h"
#include "FCN_psi.h"
#include "FCN_racah.h"
#include "FCN_radmac.h"
#include "FCN_rchar.h"
#include "FCN_rms.h"
#include "FCN_roll.h"
#include "FCN_rprod.h"
#include "FCN_rsum.h"
#include "FCN_savgol.h"
#include "FCN_secd.h"
#include "FCN_sech.h"
#include "FCN_sec.h"
#include "FCN_sign.h"
#include "FCN_simpleran.h"
#include "FCN_sind.h"
#include "FCN_sinh.h"
#include "FCN_sin.h"
#include "FCN_sinint.h"
#include "FCN_skew.h"
#include "FCN_smooth.h"
#include "FCN_splinterp.h"
#include "FCN_splsmooth.h"
#include "FCN_sqrt.h"
#include "FCN_stdev.h"
#include "FCN_step.h"
#include "FCN_string.h"
#include "FCN_struve0.h"
#include "FCN_struve1.h"
#include "FCN_student.h"
#include "FCN_studenti.h"
#include "FCN_sub.h"
#include "FCN_sum.h"
#include "FCN_sup.h"
#include "FCN_tand.h"
#include "FCN_tanh.h"
#include "FCN_tan.h"
#include "FCN_tcase.h"
#include "FCN_time.h"
#include "FCN_tina.h"
#include "FCN_ucase.h"
#include "FCN_var.h"
#include "FCN_varname.h"
#include "FCN_vartype.h"
#include "FCN_vlen.h"
#include "FCN_voigt.h"
#include "FCN_volume.h"
#include "FCN_walsh.h"
#include "FCN_where.h"
#include "FCN_wign3j.h"
#include "FCN_wign6j.h"
#include "FCN_wign9j.h"
#include "FCN_wrap.h"

FunctionTable *FunctionTable::functionTable_ = 0;

FunctionTable::FunctionTable()
{
  functionMap_ = new functionMap;
}

FunctionTable::FunctionTable( FunctionTable const &rhs )
{
  delete functionMap_;
  functionMap_ = new functionMap(*(rhs.functionMap_));
}

FunctionTable::~FunctionTable()
{ ClearTable(); }

FunctionTable *FunctionTable::GetTable()
{
  // returns the pointer to the FunctionTable object
  // FunctionTable is a "singleton", and can get its
  // pointer by this function.  The first time this
  // function is called, the FunctionTable object
  // is instantiated.
  //
  if( functionTable_ == 0 )functionTable_ = new FunctionTable();
  return functionTable_;
}

void FunctionTable::ClearTable()
{
  functionMap::const_iterator end = functionMap_->end();
  for( functionMap::const_iterator i=functionMap_->begin(); i!=end; ++i )delete (*i).second;
  delete functionMap_;
}

bool FunctionTable::Contains( FunctionDefinition const *f ) const
{ return Contains( f->Name() ); }

bool FunctionTable::Contains( wxString const &name ) const
{ return ( functionMap_->find(name) != functionMap_->end() ); }

FunctionDefinition *FunctionTable::GetFunction( wxString const &name ) const
{
  FunctionDefinition *fd = 0;
  functionMap::iterator const i = functionMap_->find(name);
  if( i != functionMap_->end() )fd = (*i).second;
  return fd;
}

void FunctionTable::AddEntry( FunctionDefinition *f )
{
  wxString name = f->Name();
  if( Contains(name) )
    throw EExpressionError( wxString(wxT("The function "))+name+
                            wxT(" is already in the table") );
  functionMap_->insert(entryType(name,f));
}

void FunctionTable::CreateFunctions()
{
  FCN_abs::Instance();
  FCN_acosd::Instance();
  FCN_acosh::Instance();
  FCN_acos::Instance();
  FCN_acotd::Instance();
  FCN_acoth::Instance();
  FCN_acot::Instance();
  FCN_acscd::Instance();
  FCN_acsch::Instance();
  FCN_acsc::Instance();
  FCN_adev::Instance();
  FCN_aerfc::Instance();
  FCN_aerf::Instance();
  FCN_agauss::Instance();
  FCN_airy::Instance();
  FCN_area::Instance();
  FCN_asecd::Instance();
  FCN_asech::Instance();
  FCN_asec::Instance();
  FCN_asind::Instance();
  FCN_asinh::Instance();
  FCN_asin::Instance();
  FCN_atan2d::Instance();
  FCN_atan2::Instance();
  FCN_atand::Instance();
  FCN_atanh::Instance();
  FCN_atan::Instance();
  FCN_bei::Instance();
  FCN_ber::Instance();
  FCN_besi0::Instance();
  FCN_besi1::Instance();
  FCN_besj0::Instance();
  FCN_besj1::Instance();
  FCN_besk0::Instance();
  FCN_besk1::Instance();
  FCN_besy0::Instance();
  FCN_besy1::Instance();
  FCN_beta::Instance();
  FCN_betain::Instance();
  FCN_binom::Instance();
  FCN_biry::Instance();
  FCN_bivarnor::Instance();
  FCN_bivariateInterpolate::Instance();
  FCN_bivariateSmooth::Instance();
  FCN_canopen::Instance();
  FCN_char::Instance();
  FCN_cheby::Instance();
  FCN_chisq::Instance();
  FCN_chisqi::Instance();
  FCN_chlogu::Instance();
  FCN_clebsg::Instance();
  FCN_clen::Instance();
  FCN_convolute::Instance();
  FCN_cosd::Instance();
  FCN_cosh::Instance();
  FCN_cos::Instance();
  FCN_cosint::Instance();
  FCN_cotd::Instance();
  FCN_coth::Instance();
  FCN_cot::Instance();
  FCN_cscd::Instance();
  FCN_csch::Instance();
  FCN_csc::Instance();
  FCN_date::Instance();
  FCN_dawson::Instance();
  FCN_deconvolute::Instance();
  FCN_deriv::Instance();
  FCN_det::Instance();
  FCN_digamma::Instance();
  FCN_dilog::Instance();
  FCN_eigen::Instance();
  FCN_ei::Instance();
  FCN_einellic::Instance();
  FCN_ellice::Instance();
  FCN_ellick::Instance();
  FCN_eqs::Instance();
  FCN_erfc::Instance();
  FCN_erf::Instance();
  FCN_evaluate::Instance();
  FCN_exist::Instance();
  FCN_expand::Instance();
  FCN_exp::Instance();
  FCN_expint::Instance();
  FCN_expn::Instance();
  FCN_factorial::Instance();
  FCN_ferdirac::Instance();
  FCN_fft::Instance();
  FCN_finellic::Instance();
  FCN_fisher::Instance();
  FCN_fold::Instance();
  FCN_frec1::Instance();
  FCN_frec2::Instance();
  FCN_fres1::Instance();
  FCN_fres2::Instance();
  FCN_gammacin::Instance();
  FCN_gamma::Instance();
  FCN_gammain::Instance();
  FCN_gammatin::Instance();
  FCN_gammln::Instance();
  FCN_gammq::Instance();
  FCN_gauss::Instance();
  FCN_gaussin::Instance();
  FCN_gaussj::Instance();
  FCN_gmean::Instance();
  FCN_heaviside::Instance();
  FCN_hermite::Instance();
  FCN_hypgeo::Instance();
  FCN_ichar::Instance();
  FCN_iclose::Instance();
  FCN_identity::Instance();
  FCN_iequal::Instance();
  FCN_ifft::Instance();
  FCN_imax::Instance();
  FCN_imin::Instance();
  FCN_index::Instance();
  FCN_integral::Instance();
  FCN_interpolate::Instance();
  FCN_int::Instance();
  FCN_inverse::Instance();
  FCN_jacobi::Instance();
  FCN_jahnuf::Instance();
  FCN_join::Instance();
  FCN_kei::Instance();
  FCN_ker::Instance();
  FCN_kurt::Instance();
  FCN_laguerre::Instance();
  FCN_lcase::Instance();
  FCN_legendre::Instance();
  FCN_len::Instance();
  FCN_lnagamma::Instance();
  FCN_lnbeta::Instance();
  FCN_ln::Instance();
  FCN_log10::Instance();
  FCN_logam::Instance();
  FCN_log::Instance();
  FCN_max::Instance();
  FCN_mean::Instance();
  FCN_median::Instance();
  FCN_min::Instance();
  FCN_mod::Instance();
  FCN_nes::Instance();
  FCN_nint::Instance();
  FCN_normal::Instance();
  FCN_null::Instance();
  FCN_pdiff::Instance();
  FCN_pfactors::Instance();
  FCN_plmn::Instance();
  FCN_plmu::Instance();
  FCN_poica::Instance();
  FCN_poisson::Instance();
  FCN_prob::Instance();
  FCN_prod::Instance();
  FCN_psi::Instance();
  FCN_racah::Instance();
  FCN_radmac::Instance();
  FCN_rchar::Instance();
  FCN_rms::Instance();
  FCN_roll::Instance();
  FCN_rprod::Instance();
  FCN_rsum::Instance();
  FCN_savgol::Instance();
  FCN_secd::Instance();
  FCN_sech::Instance();
  FCN_sec::Instance();
  FCN_sign::Instance();
  FCN_simpleran::Instance();
  FCN_sind::Instance();
  FCN_sinh::Instance();
  FCN_sin::Instance();
  FCN_sinint::Instance();
  FCN_skew::Instance();
  FCN_smooth::Instance();
  FCN_splinterp::Instance();
  FCN_splsmooth::Instance();
  FCN_sqrt::Instance();
  FCN_stdev::Instance();
  FCN_step::Instance();
  FCN_string::Instance();
  FCN_struve0::Instance();
  FCN_struve1::Instance();
  FCN_student::Instance();
  FCN_studenti::Instance();
  FCN_sub::Instance();
  FCN_sum::Instance();
  FCN_sup::Instance();
  FCN_tand::Instance();
  FCN_tanh::Instance();
  FCN_tan::Instance();
  FCN_tcase::Instance();
  FCN_time::Instance();
  FCN_tina::Instance();
  FCN_ucase::Instance();
  FCN_var::Instance();
  FCN_varname::Instance();
  FCN_vartype::Instance();
  FCN_vlen::Instance();
  FCN_voigt::Instance();
  FCN_volume::Instance();
  FCN_walsh::Instance();
  FCN_where::Instance();
  FCN_wign3j::Instance();
  FCN_wign6j::Instance();
  FCN_wign9j::Instance();
  FCN_wrap::Instance();
}

// end of file
