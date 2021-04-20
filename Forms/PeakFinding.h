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
#ifndef PeakFindingH
#define PeakFindingH
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include <vector>

class GRA_arrow3;
class GRA_cartesianCurve;

class TPeakFindingForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *PeakFindPanel;
  TSpeedButton *ScanLeftSpeedButton;
  TSpeedButton *ScanRightSpeedButton;
  TRadioGroup *MaxMinRadioGroup;
  TButton *CloseButton;
  TEdit *XPeakValEdit;
  TLabel *XPeakValLabel;
  TEdit *YPeakValEdit;
  TLabel *YPeakValLabel;
  TButton *SaveButton;
  TButton *InitializeButton;
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall ScanLeftClick(TObject *Sender);
  void __fastcall ScanRightClick(TObject *Sender);
  void __fastcall RecordCoordinates(TObject *Sender);
  void __fastcall NewCurve(TObject *Sender);

private:	// User declarations
  void EraseLastArrow();
  void CloseForm();
  void Initialize();
  void ShowValues();
  //
  bool first_;
  double xCurrent_, yCurrent_;
  int jPeak_;
  GRA_arrow3 *currentArrow_;
  GRA_cartesianCurve *curve_;
  std::vector<double> xVec_, yVec_;
  std::vector<double> xData_, yData_;
  int nPts_;
  double arrowLength_;
  double xMin_, xMax_, xLogBase_, yMin_, yMax_, yLogBase_, yRange_;

public:		// User declarations
  __fastcall TPeakFindingForm(TComponent* Owner);
  __fastcall ~TPeakFindingForm();
};

extern PACKAGE TPeakFindingForm *PeakFindingForm;

#endif
