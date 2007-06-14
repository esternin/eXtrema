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
#ifndef EXTREMA_FITFORM
#define EXTREMA_FITFORM

#include <vector>

#include "wx/wx.h"

class AnalysisWindow;
class NumericVariable;

class FitForm : public wxFrame
{
public:
  FitForm( AnalysisWindow * );

  void Set( wxString const &, wxString const &, double, double, wxString const &,
            wxString const &, int, wxString const &, std::vector<double> &,
            std::vector<double> &, double, double, wxString const & );
  void ClearGraphics();

private:
  void CreateForm();
  void FillOutForm();
  void UpdateVectors( wxComboBox * );
  void UpdateExpression();
  void FitTypeChange();
  void DegreeChange();
  void ClearResults();
  void TestTheFit();
  void DoTheFit();
  void SetupParameterFields( int, bool, bool, wxString const & =wxString(wxT("")) );
  void CreateParameters();
  void PlotFit();

  // event handlers
  void OnDataVectorChange( wxCommandEvent & );
  void OnIndepVectorChange( wxCommandEvent & );
  void OnErrorVectorChange( wxCommandEvent & );
  void OnFitTypeChange( wxCommandEvent & );
  void OnDegreeChange( wxCommandEvent & );
  void OnTest( wxCommandEvent & );
  void OnClear( wxCommandEvent & );
  void OnApply( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  wxString fitExpression_, fitTitle_, dVecName_, iVecName_, eVecName_, pvStr_;
  std::vector<wxString> pStrings_;
  double chisq_, confidenceLevel_, minRange_, maxRange_;
  unsigned int nFree_;
  std::vector< std::vector<double> > pStartValues_;
  NumericVariable *iVec_;

  AnalysisWindow *analysisWindow_;
  wxStaticText *dataST_, *indepST_, *errorST_, *typeST_, *degreeST_, *minST_, *maxST_, *exprST_;
  wxComboBox *dataCB_, *indepCB_, *errorCB_, *typeCB_, *degreeCB_;
  wxCheckBox *pCkB_[8];
  wxTextCtrl *minTC_, *maxTC_, *chiTC_, *confTC_, *degFreeTC_, *exprTC_;
  wxTextCtrl *pnameTC_[8], *pstartTC_[8], *presultTC_[8];
  wxTextCtrl *legxTC_, *legyTC_;
  wxButton *testButton_, *applyButton_;

  enum {
      ID_dvec,
      ID_ivec,
      ID_evec,
      ID_type,
      ID_degree,
      ID_test,
      ID_clear
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
