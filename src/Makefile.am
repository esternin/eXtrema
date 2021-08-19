# Copyright (C) 2007 Joseph L. Chuma <chuma@triumf.ca>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AM_CXXFLAGS = \
    -I$(top_srcdir)/src \
    -I$(top_srcdir)/src/Commands \
    -I$(top_srcdir)/src/Expression \
    -I$(top_srcdir)/src/Functions \
    -I$(top_srcdir)/src/Graphics \
    -I$(top_srcdir)/src/Operators \
    -I$(top_srcdir)/src/Parsing \
    -I$(top_srcdir)/src/Utilities \
    -I$(top_srcdir)/src/Variables \
    -I$(top_srcdir)/src/wxForms \
    $(WX_CXXFLAGS) $(MUD_CXXFLAGS) $(MINUIT_CXXFLAGS)

# We now use non-recursive automake, but still create separate libraries for
# each subdirectory contents. Not doing it would be more efficient, as each of
# these libraries is currently only used once, so we don't gain anything from
# the time spend on linking it, but keep it like this for now in case we need
# separate libraries in the future, e.g. if we ever add tests using them.
noinst_LTLIBRARIES = libcommands.la
libcommands_la_SOURCES = \
    Commands/CMD_alias.cpp Commands/CMD_alias.h \
    Commands/CMD_bestfit.cpp Commands/CMD_bestfit.h \
    Commands/CMD_bin2d.cpp Commands/CMD_bin2d.h \
    Commands/CMD_bin.cpp Commands/CMD_bin.h \
    Commands/CMD_clear.cpp Commands/CMD_clear.h \
    Commands/CMD_contour.cpp Commands/CMD_contour.h \
    Commands/CMD_dealias.cpp Commands/CMD_dealias.h \
    Commands/CMD_defaults.cpp Commands/CMD_defaults.h \
    Commands/CMD_define.cpp Commands/CMD_define.h \
    Commands/CMD_density.cpp Commands/CMD_density.h \
    Commands/CMD_destroy.cpp Commands/CMD_destroy.h \
    Commands/CMD_display.cpp Commands/CMD_display.h \
    Commands/CMD_do.cpp Commands/CMD_do.h \
    Commands/CMD_ellipse.cpp Commands/CMD_ellipse.h \
    Commands/CMD_enddo.cpp Commands/CMD_enddo.h \
    Commands/CMD_endif.cpp Commands/CMD_endif.h \
    Commands/CMD_evaluateExpression.cpp Commands/CMD_evaluateExpression.h \
    Commands/CMD_execute.cpp Commands/CMD_execute.h \
    Commands/CMD_figure.cpp Commands/CMD_figure.h \
    Commands/CMD_filter.cpp Commands/CMD_filter.h \
    Commands/CMD_fit.cpp Commands/CMD_fit.h \
    Commands/CMD_form.cpp Commands/CMD_form.h \
    Commands/CMD_generate.cpp Commands/CMD_generate.h \
    Commands/CMD_get.cpp Commands/CMD_get.h \
    Commands/CMD_goto.cpp Commands/CMD_goto.h \
    Commands/CMD_graph.cpp Commands/CMD_graph.h \
    Commands/CMD_grid.cpp Commands/CMD_grid.h \
    Commands/CMD_hardcopy.cpp Commands/CMD_hardcopy.h \
    Commands/CMD_help.cpp Commands/CMD_help.h \
    Commands/CMD_if.cpp Commands/CMD_if.h \
    Commands/CMD_inquire.cpp Commands/CMD_inquire.h \
    Commands/CMD_list.cpp Commands/CMD_list.h \
    Commands/CMD_matrix.cpp Commands/CMD_matrix.h \
    Commands/CMD_minuit.cpp Commands/CMD_minuit.h \
    Commands/CMD_page.cpp Commands/CMD_page.h \
    Commands/CMD_pause.cpp Commands/CMD_pause.h \
    Commands/CMD_polygon.cpp Commands/CMD_polygon.h \
    Commands/CMD_quit.cpp Commands/CMD_quit.h \
    Commands/CMD_read.cpp Commands/CMD_read.h \
    Commands/CMD_rebin.cpp Commands/CMD_rebin.h \
    Commands/CMD_replot.cpp Commands/CMD_replot.h \
    Commands/CMD_restore.cpp Commands/CMD_restore.h \
    Commands/CMD_return.cpp Commands/CMD_return.h \
    Commands/CMD_save.cpp Commands/CMD_save.h \
    Commands/CMD_scalar.cpp Commands/CMD_scalar.h \
    Commands/CMD_scales.cpp Commands/CMD_scales.h \
    Commands/CMD_set.cpp Commands/CMD_set.h \
    Commands/CMD_show.cpp Commands/CMD_show.h \
    Commands/CMD_sort.cpp Commands/CMD_sort.h \
    Commands/CMD_stack.cpp Commands/CMD_stack.h \
    Commands/CMD_statistics.cpp Commands/CMD_statistics.h \
    Commands/CMD_text.cpp Commands/CMD_text.h \
    Commands/CMD_vector.cpp Commands/CMD_vector.h \
    Commands/CMD_window.cpp Commands/CMD_window.h \
    Commands/CMD_world.cpp Commands/CMD_world.h \
    Commands/CMD_write.cpp Commands/CMD_write.h \
    Commands/CMD_zerolines.cpp Commands/CMD_zerolines.h \
    Commands/Command.cpp Commands/Command.h \
    Commands/CommandTable.cpp Commands/CommandTable.h \
    Commands/Extrema_fcn.cpp Commands/Extrema_fcn.h \
    Commands/ECommandError.h

noinst_LTLIBRARIES += libexpression.la
libexpression_la_SOURCES = \
    Expression/ExprCodes.cpp Expression/ExprCodes.h \
    Expression/Expression.cpp Expression/Expression.h \
    Expression/ListVector.cpp Expression/ListVector.h \
    Expression/Workspace.cpp Expression/Workspace.h \
    Expression/BndChk.h Expression/EExpressionError.h \
    Expression/RangeVector.h

noinst_LTLIBRARIES += libfunctions.la
libfunctions_la_SOURCES = \
    Functions/FCN_abs.cpp Functions/FCN_abs.h \
    Functions/FCN_acos.cpp Functions/FCN_acos.h \
    Functions/FCN_acosd.cpp Functions/FCN_acosd.h \
    Functions/FCN_acosh.cpp Functions/FCN_acosh.h \
    Functions/FCN_acot.cpp Functions/FCN_acot.h \
    Functions/FCN_acotd.cpp Functions/FCN_acotd.h \
    Functions/FCN_acoth.cpp Functions/FCN_acoth.h \
    Functions/FCN_acsc.cpp Functions/FCN_acsc.h \
    Functions/FCN_acscd.cpp Functions/FCN_acscd.h \
    Functions/FCN_acsch.cpp Functions/FCN_acsch.h \
    Functions/FCN_adev.cpp Functions/FCN_adev.h \
    Functions/FCN_aerfc.cpp Functions/FCN_aerfc.h \
    Functions/FCN_aerf.cpp Functions/FCN_aerf.h \
    Functions/FCN_agauss.cpp Functions/FCN_agauss.h \
    Functions/FCN_airy.cpp Functions/FCN_airy.h \
    Functions/FCN_area.cpp Functions/FCN_area.h \
    Functions/FCN_asec.cpp Functions/FCN_asec.h \
    Functions/FCN_asecd.cpp Functions/FCN_asecd.h \
    Functions/FCN_asech.cpp Functions/FCN_asech.h \
    Functions/FCN_asin.cpp Functions/FCN_asin.h \
    Functions/FCN_asind.cpp Functions/FCN_asind.h \
    Functions/FCN_asinh.cpp Functions/FCN_asinh.h \
    Functions/FCN_atan2.cpp Functions/FCN_atan2.h \
    Functions/FCN_atan2d.cpp Functions/FCN_atan2d.h \
    Functions/FCN_atan.cpp Functions/FCN_atan.h \
    Functions/FCN_atand.cpp Functions/FCN_atand.h \
    Functions/FCN_atanh.cpp Functions/FCN_atanh.h \
    Functions/FCN_bei.cpp Functions/FCN_bei.h \
    Functions/FCN_ber.cpp Functions/FCN_ber.h \
    Functions/FCN_besi0.cpp Functions/FCN_besi0.h \
    Functions/FCN_besi1.cpp Functions/FCN_besi1.h \
    Functions/FCN_besj0.cpp Functions/FCN_besj0.h \
    Functions/FCN_besj1.cpp Functions/FCN_besj1.h \
    Functions/FCN_besk0.cpp Functions/FCN_besk0.h \
    Functions/FCN_besk1.cpp Functions/FCN_besk1.h \
    Functions/FCN_besy0.cpp Functions/FCN_besy0.h \
    Functions/FCN_besy1.cpp Functions/FCN_besy1.h \
    Functions/FCN_beta.cpp Functions/FCN_beta.h \
    Functions/FCN_betain.cpp Functions/FCN_betain.h \
    Functions/FCN_binom.cpp Functions/FCN_binom.h \
    Functions/FCN_biry.cpp Functions/FCN_biry.h \
    Functions/FCN_bivarnor.cpp Functions/FCN_bivarnor.h \
    Functions/FCN_bivinterp.cpp Functions/FCN_bivinterp.h \
    Functions/FCN_bivsmooth.cpp Functions/FCN_bivsmooth.h \
    Functions/FCN_canopen.cpp Functions/FCN_canopen.h \
    Functions/FCN_char.cpp Functions/FCN_char.h \
    Functions/FCN_cheby.cpp Functions/FCN_cheby.h \
    Functions/FCN_chisq.cpp Functions/FCN_chisq.h \
    Functions/FCN_chisqi.cpp Functions/FCN_chisqi.h \
    Functions/FCN_chlogu.cpp Functions/FCN_chlogu.h \
    Functions/FCN_clebsg.cpp Functions/FCN_clebsg.h \
    Functions/FCN_clen.cpp Functions/FCN_clen.h \
    Functions/FCN_convolute.cpp Functions/FCN_convolute.h \
    Functions/FCN_cos.cpp Functions/FCN_cos.h \
    Functions/FCN_cosd.cpp Functions/FCN_cosd.h \
    Functions/FCN_cosh.cpp Functions/FCN_cosh.h \
    Functions/FCN_cosint.cpp Functions/FCN_cosint.h \
    Functions/FCN_cot.cpp Functions/FCN_cot.h \
    Functions/FCN_cotd.cpp Functions/FCN_cotd.h \
    Functions/FCN_coth.cpp Functions/FCN_coth.h \
    Functions/FCN_csc.cpp Functions/FCN_csc.h \
    Functions/FCN_cscd.cpp Functions/FCN_cscd.h \
    Functions/FCN_csch.cpp Functions/FCN_csch.h \
    Functions/FCN_date.cpp Functions/FCN_date.h \
    Functions/FCN_dawson.cpp Functions/FCN_dawson.h \
    Functions/FCN_deconvolute.cpp Functions/FCN_deconvolute.h \
    Functions/FCN_deriv.cpp Functions/FCN_deriv.h \
    Functions/FCN_det.cpp Functions/FCN_det.h \
    Functions/FCN_digamma.cpp Functions/FCN_digamma.h \
    Functions/FCN_dilog.cpp Functions/FCN_dilog.h \
    Functions/FCN_ei.cpp Functions/FCN_ei.h \
    Functions/FCN_eigen.cpp Functions/FCN_eigen.h \
    Functions/FCN_einellic.cpp Functions/FCN_einellic.h \
    Functions/FCN_ellice.cpp Functions/FCN_ellice.h \
    Functions/FCN_ellick.cpp Functions/FCN_ellick.h \
    Functions/FCN_eqs.cpp Functions/FCN_eqs.h \
    Functions/FCN_erfc.cpp Functions/FCN_erfc.h \
    Functions/FCN_erf.cpp Functions/FCN_erf.h \
    Functions/FCN_evaluate.cpp Functions/FCN_evaluate.h \
    Functions/FCN_exist.cpp Functions/FCN_exist.h \
    Functions/FCN_expand.cpp Functions/FCN_expand.h \
    Functions/FCN_exp.cpp Functions/FCN_exp.h \
    Functions/FCN_expint.cpp Functions/FCN_expint.h \
    Functions/FCN_expn.cpp Functions/FCN_expn.h \
    Functions/FCN_factorial.cpp Functions/FCN_factorial.h \
    Functions/FCN_ferdirac.cpp Functions/FCN_ferdirac.h \
    Functions/FCN_fft.cpp Functions/FCN_fft.h \
    Functions/FCN_finellic.cpp Functions/FCN_finellic.h \
    Functions/FCN_fisher.cpp Functions/FCN_fisher.h \
    Functions/FCN_fold.cpp Functions/FCN_fold.h \
    Functions/FCN_frec1.cpp Functions/FCN_frec1.h \
    Functions/FCN_frec2.cpp Functions/FCN_frec2.h \
    Functions/FCN_fres1.cpp Functions/FCN_fres1.h \
    Functions/FCN_fres2.cpp Functions/FCN_fres2.h \
    Functions/FCN_gammacin.cpp Functions/FCN_gammacin.h \
    Functions/FCN_gamma.cpp Functions/FCN_gamma.h \
    Functions/FCN_gammain.cpp Functions/FCN_gammain.h \
    Functions/FCN_gammatin.cpp Functions/FCN_gammatin.h \
    Functions/FCN_gammln.cpp Functions/FCN_gammln.h \
    Functions/FCN_gammq.cpp Functions/FCN_gammq.h \
    Functions/FCN_gauss.cpp Functions/FCN_gauss.h \
    Functions/FCN_gaussin.cpp Functions/FCN_gaussin.h \
    Functions/FCN_gaussj.cpp Functions/FCN_gaussj.h \
    Functions/FCN_gmean.cpp Functions/FCN_gmean.h \
    Functions/FCN_heaviside.cpp Functions/FCN_heaviside.h \
    Functions/FCN_hermite.cpp Functions/FCN_hermite.h \
    Functions/FCN_hypgeo.cpp Functions/FCN_hypgeo.h \
    Functions/FCN_ichar.cpp Functions/FCN_ichar.h \
    Functions/FCN_iclose.cpp Functions/FCN_iclose.h \
    Functions/FCN_identity.cpp Functions/FCN_identity.h \
    Functions/FCN_iequal.cpp Functions/FCN_iequal.h \
    Functions/FCN_ifft.cpp Functions/FCN_ifft.h \
    Functions/FCN_imax.cpp Functions/FCN_imax.h \
    Functions/FCN_imin.cpp Functions/FCN_imin.h \
    Functions/FCN_index.cpp Functions/FCN_index.h \
    Functions/FCN_int.cpp Functions/FCN_int.h \
    Functions/FCN_integral.cpp Functions/FCN_integral.h \
    Functions/FCN_interpolate.cpp Functions/FCN_interpolate.h \
    Functions/FCN_inverse.cpp Functions/FCN_inverse.h \
    Functions/FCN_jacobi.cpp Functions/FCN_jacobi.h \
    Functions/FCN_jahnuf.cpp Functions/FCN_jahnuf.h \
    Functions/FCN_join.cpp Functions/FCN_join.h \
    Functions/FCN_kei.cpp Functions/FCN_kei.h \
    Functions/FCN_ker.cpp Functions/FCN_ker.h \
    Functions/FCN_kurt.cpp Functions/FCN_kurt.h \
    Functions/FCN_laguerre.cpp Functions/FCN_laguerre.h \
    Functions/FCN_lcase.cpp Functions/FCN_lcase.h \
    Functions/FCN_legendre.cpp Functions/FCN_legendre.h \
    Functions/FCN_len.cpp Functions/FCN_len.h \
    Functions/FCN_lnagamma.cpp Functions/FCN_lnagamma.h \
    Functions/FCN_lnbeta.cpp Functions/FCN_lnbeta.h \
    Functions/FCN_ln.cpp Functions/FCN_ln.h \
    Functions/FCN_log10.cpp Functions/FCN_log10.h \
    Functions/FCN_logam.cpp Functions/FCN_logam.h \
    Functions/FCN_log.cpp Functions/FCN_log.h \
    Functions/FCN_max.cpp Functions/FCN_max.h \
    Functions/FCN_mean.cpp Functions/FCN_mean.h \
    Functions/FCN_median.cpp Functions/FCN_median.h \
    Functions/FCN_min.cpp Functions/FCN_min.h \
    Functions/FCN_mod.cpp Functions/FCN_mod.h \
    Functions/FCN_nes.cpp Functions/FCN_nes.h \
    Functions/FCN_nint.cpp Functions/FCN_nint.h \
    Functions/FCN_normal.cpp Functions/FCN_normal.h \
    Functions/FCN_null.cpp Functions/FCN_null.h \
    Functions/FCN_pdiff.cpp Functions/FCN_pdiff.h \
    Functions/FCN_pfactors.cpp Functions/FCN_pfactors.h \
    Functions/FCN_plmn.cpp Functions/FCN_plmn.h \
    Functions/FCN_plmu.cpp Functions/FCN_plmu.h \
    Functions/FCN_poica.cpp Functions/FCN_poica.h \
    Functions/FCN_poisson.cpp Functions/FCN_poisson.h \
    Functions/FCN_prob.cpp Functions/FCN_prob.h \
    Functions/FCN_prod.cpp Functions/FCN_prod.h \
    Functions/FCN_psi.cpp Functions/FCN_psi.h \
    Functions/FCN_racah.cpp Functions/FCN_racah.h \
    Functions/FCN_radmac.cpp Functions/FCN_radmac.h \
    Functions/FCN_rchar.cpp Functions/FCN_rchar.h \
    Functions/FCN_rms.cpp Functions/FCN_rms.h \
    Functions/FCN_roll.cpp Functions/FCN_roll.h \
    Functions/FCN_rprod.cpp Functions/FCN_rprod.h \
    Functions/FCN_rsum.cpp Functions/FCN_rsum.h \
    Functions/FCN_savgol.cpp Functions/FCN_savgol.h \
    Functions/FCN_sec.cpp Functions/FCN_sec.h \
    Functions/FCN_secd.cpp Functions/FCN_secd.h \
    Functions/FCN_sech.cpp Functions/FCN_sech.h \
    Functions/FCN_sign.cpp Functions/FCN_sign.h \
    Functions/FCN_simpleran.cpp Functions/FCN_simpleran.h \
    Functions/FCN_sin.cpp Functions/FCN_sin.h \
    Functions/FCN_sind.cpp Functions/FCN_sind.h \
    Functions/FCN_sinh.cpp Functions/FCN_sinh.h \
    Functions/FCN_skew.cpp Functions/FCN_skew.h \
    Functions/FCN_sinint.cpp Functions/FCN_sinint.h \
    Functions/FCN_smooth.cpp Functions/FCN_smooth.h \
    Functions/FCN_splinterp.cpp Functions/FCN_splinterp.h \
    Functions/FCN_splsmooth.cpp Functions/FCN_splsmooth.h \
    Functions/FCN_sqrt.cpp Functions/FCN_sqrt.h \
    Functions/FCN_stdev.cpp Functions/FCN_stdev.h \
    Functions/FCN_step.cpp Functions/FCN_step.h \
    Functions/FCN_string.cpp Functions/FCN_string.h \
    Functions/FCN_struve0.cpp Functions/FCN_struve0.h \
    Functions/FCN_struve1.cpp Functions/FCN_struve1.h \
    Functions/FCN_student.cpp Functions/FCN_student.h \
    Functions/FCN_studenti.cpp Functions/FCN_studenti.h \
    Functions/FCN_sub.cpp Functions/FCN_sub.h \
    Functions/FCN_sum.cpp Functions/FCN_sum.h \
    Functions/FCN_sup.cpp Functions/FCN_sup.h \
    Functions/FCN_tan.cpp Functions/FCN_tan.h \
    Functions/FCN_tand.cpp Functions/FCN_tand.h \
    Functions/FCN_tanh.cpp Functions/FCN_tanh.h \
    Functions/FCN_tcase.cpp Functions/FCN_tcase.h \
    Functions/FCN_time.cpp Functions/FCN_time.h \
    Functions/FCN_tina.cpp Functions/FCN_tina.h \
    Functions/FCN_ucase.cpp Functions/FCN_ucase.h \
    Functions/FCN_var.cpp Functions/FCN_var.h \
    Functions/FCN_varname.cpp Functions/FCN_varname.h \
    Functions/FCN_vartype.cpp Functions/FCN_vartype.h \
    Functions/FCN_vlen.cpp Functions/FCN_vlen.h \
    Functions/FCN_voigt.cpp Functions/FCN_voigt.h \
    Functions/FCN_volume.cpp Functions/FCN_volume.h \
    Functions/FCN_walsh.cpp Functions/FCN_walsh.h \
    Functions/FCN_where.cpp Functions/FCN_where.h \
    Functions/FCN_wign3j.cpp Functions/FCN_wign3j.h \
    Functions/FCN_wign6j.cpp Functions/FCN_wign6j.h \
    Functions/FCN_wign9j.cpp Functions/FCN_wign9j.h \
    Functions/FCN_wrap.cpp Functions/FCN_wrap.h \
    Functions/FunctionDefinition.cpp Functions/FunctionDefinition.h \
    Functions/FunctionTable.cpp Functions/FunctionTable.h \
    Functions/ScalarFunction.cpp Functions/ScalarFunction.h \
    Functions/ArrayFunction.h

noinst_LTLIBRARIES += libgraphics.la
libgraphics_la_SOURCES = \
    Graphics/GRA_angleCharacteristic.cpp Graphics/GRA_angleCharacteristic.h \
    Graphics/GRA_arrow1.cpp Graphics/GRA_arrow1.h \
    Graphics/GRA_arrow2.cpp Graphics/GRA_arrow2.h \
    Graphics/GRA_arrow3.cpp Graphics/GRA_arrow3.h \
    Graphics/GRA_asterisk.cpp Graphics/GRA_asterisk.h \
    Graphics/GRA_axis.cpp Graphics/GRA_axis.h \
    Graphics/GRA_bitmap.cpp Graphics/GRA_bitmap.h \
    Graphics/GRA_boolCharacteristic.cpp Graphics/GRA_boolCharacteristic.h \
    Graphics/GRA_boxPlot.cpp Graphics/GRA_boxPlot.h \
    Graphics/GRA_cartesianAxes.cpp Graphics/GRA_cartesianAxes.h \
    Graphics/GRA_cartesianCurve.cpp Graphics/GRA_cartesianCurve.h \
    Graphics/GRA_colorCharacteristic.cpp Graphics/GRA_colorCharacteristic.h \
    Graphics/GRA_colorControl.cpp Graphics/GRA_colorControl.h \
    Graphics/GRA_color.cpp Graphics/GRA_color.h \
    Graphics/GRA_colorMap.cpp Graphics/GRA_colorMap.h \
    Graphics/GRA_contourLine.cpp Graphics/GRA_contourLine.h \
    Graphics/GRA_cross.cpp Graphics/GRA_cross.h \
    Graphics/GRA_crossPlus.cpp Graphics/GRA_crossPlus.h \
    Graphics/GRA_densityPlot.cpp Graphics/GRA_densityPlot.h \
    Graphics/GRA_diamond.cpp Graphics/GRA_diamond.h \
    Graphics/GRA_diamondPlus.cpp Graphics/GRA_diamondPlus.h \
    Graphics/GRA_diffusionPlot.cpp Graphics/GRA_diffusionPlot.h \
    Graphics/GRA_distanceCharacteristic.cpp Graphics/GRA_distanceCharacteristic.h \
    Graphics/GRA_ditheringPlot.cpp Graphics/GRA_ditheringPlot.h \
    Graphics/GRA_doubleCharacteristic.cpp Graphics/GRA_doubleCharacteristic.h \
    Graphics/GRA_drawableObject.cpp Graphics/GRA_drawableObject.h \
    Graphics/GRA_drawableText.cpp Graphics/GRA_drawableText.h \
    Graphics/GRA_ellipse.cpp Graphics/GRA_ellipse.h \
    Graphics/GRA_errorBar.cpp Graphics/GRA_errorBar.h \
    Graphics/GRA_fontCharacteristic.cpp Graphics/GRA_fontCharacteristic.h \
    Graphics/GRA_fontControl.cpp Graphics/GRA_fontControl.h \
    Graphics/GRA_font.cpp Graphics/GRA_font.h \
    Graphics/GRA_gradientPlot.cpp Graphics/GRA_gradientPlot.h \
    Graphics/GRA_intCharacteristic.cpp Graphics/GRA_intCharacteristic.h \
    Graphics/GRA_legend.cpp Graphics/GRA_legend.h \
    Graphics/GRA_multiLineFigure.cpp Graphics/GRA_multiLineFigure.h \
    Graphics/GRA_plotSymbol.cpp Graphics/GRA_plotSymbol.h \
    Graphics/GRA_plus.cpp Graphics/GRA_plus.h \
    Graphics/GRA_point.cpp Graphics/GRA_point.h \
    Graphics/GRA_polarAxes.cpp Graphics/GRA_polarAxes.h \
    Graphics/GRA_polarCurve.cpp Graphics/GRA_polarCurve.h \
    Graphics/GRA_polygon.cpp Graphics/GRA_polygon.h \
    Graphics/GRA_polyline.cpp Graphics/GRA_polyline.h \
    Graphics/GRA_postscript.cpp Graphics/GRA_postscript.h \
    Graphics/GRA_rectangle.cpp Graphics/GRA_rectangle.h \
    Graphics/GRA_setOfCharacteristics.cpp Graphics/GRA_setOfCharacteristics.h \
    Graphics/GRA_simpleText.cpp Graphics/GRA_simpleText.h \
    Graphics/GRA_sizeCharacteristic.cpp Graphics/GRA_sizeCharacteristic.h \
    Graphics/GRA_star5pt.cpp Graphics/GRA_star5pt.h \
    Graphics/GRA_stringCharacteristic.cpp Graphics/GRA_stringCharacteristic.h \
    Graphics/GRA_thiessenTriangulation.cpp Graphics/GRA_thiessenTriangulation.h \
    Graphics/GRA_threeDPlot.cpp Graphics/GRA_threeDPlot.h \
    Graphics/GRA_triangle.cpp Graphics/GRA_triangle.h \
    Graphics/GRA_window.cpp Graphics/GRA_window.h \
    Graphics/GRA_wxWidgets.cpp Graphics/GRA_wxWidgets.h \
    Graphics/GRA_characteristic.h Graphics/EGraphicsError.h Graphics/GRA_shape.h

noinst_LTLIBRARIES += liboperators.la
liboperators_la_SOURCES = \
    Operators/OperatorTable.cpp Operators/OperatorTable.h \
    Operators/OperatorDefinition.cpp Operators/OperatorDefinition.h \
    Operators/ScalarOperator.cpp Operators/ScalarOperator.h \
    Operators/OPR_and.cpp Operators/OPR_and.h \
    Operators/OPR_concatenation.cpp Operators/OPR_concatenation.h \
    Operators/OPR_divide.cpp Operators/OPR_divide.h \
    Operators/OPR_equals.cpp Operators/OPR_equals.h \
    Operators/OPR_exclusiveOr.cpp Operators/OPR_exclusiveOr.h \
    Operators/OPR_exponentiation2.cpp Operators/OPR_exponentiation2.h \
    Operators/OPR_exponentiation.cpp Operators/OPR_exponentiation.h \
    Operators/OPR_ge.cpp Operators/OPR_ge.h \
    Operators/OPR_gt.cpp Operators/OPR_gt.h \
    Operators/OPR_innerProduct.cpp Operators/OPR_innerProduct.h \
    Operators/OPR_intersection.cpp Operators/OPR_intersection.h \
    Operators/OPR_le.cpp Operators/OPR_le.h \
    Operators/OPR_lt.cpp Operators/OPR_lt.h \
    Operators/OPR_minus.cpp Operators/OPR_minus.h \
    Operators/OPR_multiply.cpp Operators/OPR_multiply.h \
    Operators/OPR_ne.cpp Operators/OPR_ne.h \
    Operators/OPR_not.cpp Operators/OPR_not.h \
    Operators/OPR_or.cpp Operators/OPR_or.h \
    Operators/OPR_outerProduct.cpp Operators/OPR_outerProduct.h \
    Operators/OPR_plus.cpp Operators/OPR_plus.h \
    Operators/OPR_reflect.cpp Operators/OPR_reflect.h \
    Operators/OPR_transpose.cpp Operators/OPR_transpose.h \
    Operators/OPR_union.cpp Operators/OPR_union.h \
    Operators/ArrayOperator.h

noinst_LTLIBRARIES += libparsing.la
libparsing_la_SOURCES = \
    Parsing/ParseLine.cpp Parsing/ParseLine.h \
    Parsing/ParseToken.cpp Parsing/ParseToken.h \
    Parsing/Script.cpp Parsing/Script.h \
    Parsing/ESyntaxError.h

noinst_LTLIBRARIES += libutilities.la
libutilities_la_SOURCES = \
    Utilities/ExGlobals.cpp Utilities/ExGlobals.h \
    Utilities/ExXML.cpp Utilities/ExXML.h \
    Utilities/ToAscii.h \
    Utilities/UsefulFunctions.cpp Utilities/UsefulFunctions.h

noinst_LTLIBRARIES += libvariables.la
libvariables_la_SOURCES = \
    Variables/NumericData.cpp Variables/NumericData.h \
    Variables/NumericVariable.cpp Variables/NumericVariable.h \
    Variables/NVariableTable.cpp Variables/NVariableTable.h \
    Variables/TextData.cpp Variables/TextData.h \
    Variables/TextVariable.cpp Variables/TextVariable.h \
    Variables/TVariableTable.cpp Variables/TVariableTable.h \
    Variables/Variable.cpp Variables/Variable.h \
    Variables/EVariableError.h

noinst_LTLIBRARIES += libwxforms.la
libwxforms_la_SOURCES = \
    wxForms/AnalysisCommandPanel.cpp wxForms/AnalysisCommandPanel.h \
    wxForms/AnalysisMessagePanel.cpp wxForms/AnalysisMessagePanel.h \
    wxForms/AnalysisSpeedButtonPanel.cpp wxForms/AnalysisSpeedButtonPanel.h \
    wxForms/AnalysisWindow.cpp wxForms/AnalysisWindow.h \
    wxForms/AxisPopup.cpp wxForms/AxisPopup.h \
    wxForms/BasicColors.cpp wxForms/BasicColors.h \
    wxForms/ChooseFilePanel.cpp wxForms/ChooseFilePanel.h \
    wxForms/CommandTextCtrl.cpp wxForms/CommandTextCtrl.h \
    wxForms/CurvePopup.cpp wxForms/CurvePopup.h \
    wxForms/ExecuteDialog.cpp wxForms/ExecuteDialog.h \
    wxForms/FigureForm.cpp wxForms/FigureForm.h \
    wxForms/FitForm.cpp wxForms/FitForm.h \
    wxForms/FontChooser.cpp wxForms/FontChooser.h \
    wxForms/GenerateVectorForm.cpp wxForms/GenerateVectorForm.h \
    wxForms/GraphForm.cpp wxForms/GraphForm.h \
    wxForms/GraphicsPage.cpp wxForms/GraphicsPage.h \
    wxForms/HintForm.cpp wxForms/HintForm.h \
    wxForms/ImportWindow.cpp wxForms/ImportWindow.h \
    wxForms/InquireYNDialog.cpp wxForms/InquireYNDialog.h \
    wxForms/InquireDialog.cpp wxForms/InquireDialog.h \
    wxForms/LegendPopup.cpp wxForms/LegendPopup.h \
    wxForms/PeakFindForm.cpp wxForms/PeakFindForm.h \
    wxForms/ReadVectorsForm.cpp wxForms/ReadVectorsForm.h \
    wxForms/ReadScalarsForm.cpp wxForms/ReadScalarsForm.h \
    wxForms/ReadMatrixForm.cpp wxForms/ReadMatrixForm.h \
    wxForms/ReadTextForm.cpp wxForms/ReadTextForm.h \
    wxForms/SetAspectRatioForm.cpp wxForms/SetAspectRatioForm.h \
    wxForms/ShowVariablesForm.cpp wxForms/ShowVariablesForm.h \
    wxForms/StackDialog.cpp wxForms/StackDialog.h \
    wxForms/TextForm.cpp wxForms/TextForm.h \
    wxForms/TextPopup.cpp wxForms/TextPopup.h \
    wxForms/ThreeDPlotForm.cpp wxForms/ThreeDPlotForm.h \
    wxForms/VarInfoForm.cpp wxForms/VarInfoForm.h \
    wxForms/VisualizationSpeedButtonPanel.cpp wxForms/VisualizationSpeedButtonPanel.h \
    wxForms/VisualizationWindow.cpp wxForms/VisualizationWindow.h \
    wxForms/WriteVectorsForm.cpp wxForms/WriteVectorsForm.h \
    wxForms/WriteScalarsForm.cpp wxForms/WriteScalarsForm.h \
    wxForms/WriteMatrixForm.cpp wxForms/WriteMatrixForm.h \
    wxForms/WriteTextForm.cpp wxForms/WriteTextForm.h

lib_LTLIBRARIES = libextrema.la
libextrema_la_SOURCES =
nodist_EXTRA_libextrema_la_SOURCES = dummy.cxx
libextrema_la_LIBADD = \
    libcommands.la \
    libexpression.la \
    libfunctions.la \
    libgraphics.la \
    liboperators.la \
    libparsing.la \
    libutilities.la \
    libvariables.la \
    libwxforms.la

bin_PROGRAMS = extrema
extrema_SOURCES = extrema.cpp extrema.h
extrema_LDADD = libextrema.la $(WX_LIBS) $(MUD_LIB) $(MINUIT_LIB)

if USE_MSW
# Note that Automake doesn't recognize pattern rules, so use the old style
# suffix rule instead.
.rc.o:
	$(WINDRES) -I $(top_srcdir) $(WX_CPPFLAGS) $< $@

extrema_SOURCES += extrema_res.rc
endif

install-data-local:
	-cp -Rf ../Images $(pkgdatadir)/Images
	-cp -Rf ../Help $(pkgdatadir)/Help
