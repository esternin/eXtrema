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
#ifndef FitNoviceH
#define FitNoviceH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>

#include "ExString.h"

class NumericVariable;

class TFitNoviceForm : public TForm
{
__published:	// IDE-managed Components
  TComboBox *DataVecComboBox;
  TLabel *DataVecLabel;
  TLabel *ExpressionLabel;
  TGroupBox *FitResultsGroupBox;
  TLabel *ChiSquareLabel;
  TEdit *ChiSquareEdit;
  TLabel *ConfidenceLevelLabel;
  TEdit *ConfidenceLevelEdit;
  TLabel *DegFreeLabel;
  TEdit *DegFreeEdit;
  TButton *DoFitButton;
  TButton *CloseButton;
  TLabel *IndepVecLabel;
  TComboBox *IndepVecComboBox;
  TLabel *FitTypeLabel;
  TComboBox *FitTypeComboBox;
  TLabel *NumberLabel;
  TComboBox *NumberComboBox;
  TStringGrid *ParameterStringGrid;
  TButton *TestFitButton;
  TButton *ClearButton;
  TLabel *ErrorVecLabel;
  TComboBox *ErrorVecComboBox;
  TEdit *IVMinEdit;
  TEdit *IVMaxEdit;
  TLabel *IVMinLabel;
  TLabel *IVMaxLabel;
  TLabel *FixedLabel;
  TCheckBox *FixedCheckBox1;
  TCheckBox *FixedCheckBox2;
  TCheckBox *FixedCheckBox3;
  TCheckBox *FixedCheckBox4;
  TCheckBox *FixedCheckBox5;
  TCheckBox *FixedCheckBox6;
  TCheckBox *FixedCheckBox7;
  TCheckBox *FixedCheckBox8;
  TLabel *LegendXLabel;
  TEdit *LegendXEdit;
  TLabel *LegendYLabel;
  TEdit *LegendYEdit;
  TLabel *LegendLocLabel;
  TRichEdit *ExpressionRichEdit;
  void __fastcall UpdateVectors(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall CloseButtonClick(TObject *Sender);
  void __fastcall DoFitButtonClick(TObject *Sender);
  void __fastcall FitTypeChange(TObject *Sender);
  void __fastcall NumberChange(TObject *Sender);
  void __fastcall SelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
  void __fastcall TestFitButtonClick(TObject *Sender);
  void __fastcall DataVecChange(TObject *Sender);
  void __fastcall IndepVecChange(TObject *Sender);
  void __fastcall ClearButtonClick(TObject *Sender);
  void __fastcall ErrorVecChange(TObject *Sender);
  void __fastcall ExpressionEditChange(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);

private:	// User declarations
  void __fastcall FillOutForm();
  void __fastcall CreateParameters();
  void __fastcall UpdateExpression();
  void __fastcall PlotFit();
  void __fastcall ClearResults();

  ExString fitExpression_, fitTitle_, dVecName_, iVecName_, eVecName_, pvStr_;
  std::vector<ExString> pStrings_;
  double chisq_, confidenceLevel_, minRange_, maxRange_;
  std::size_t nFree_;
  std::vector< std::vector<double> > pStartValues_;
  NumericVariable *iVec_;

public:		// User declarations
  __fastcall TFitNoviceForm(TComponent* Owner);
  __fastcall ~TFitNoviceForm();
  void Set( ExString const &, ExString const &, double, double, ExString const &,
            ExString const &, int, ExString const &, std::vector<double> &,
            std::vector<double> &, double, double, ExString const & );
};

extern PACKAGE TFitNoviceForm *FitNoviceForm;

#endif
