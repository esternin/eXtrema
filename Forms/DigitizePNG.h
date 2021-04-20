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
#ifndef DigitizePNGH
#define DigitizePNGH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>

#include <vector>

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

class TDigitizePNGForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TButton *CloseButton;
  TLabel *XoutLabel;
  TEdit *XoutEdit;
  TLabel *YoutLabel;
  TEdit *YoutEdit;
  TButton *StartStopButton;
  TLabel *PointsLabel1;
  TLabel *PointsLabel2;
  TLabel *PointsLabel3;
  TLabel *NPointsLabel;
  TLabel *XYLabel1;
  TLabel *XYLabel2;
  TLabel *XYLabel3;
  TLabel *NPLabel;
  TLabel *LastPointLabel;
  TLabel *LPLabel;

  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall StartStopClick(TObject *Sender);

private:	// User declarations
  bool digitizing_;
  double xUser_[3], yUser_[3];
  int xImage_[3], yImage_[3];
  int nPick_, nDigitized_;
  double a_, b_, c_, d_, e_, f_;
  std::vector<double> xd_, yd_;
  //
  void __fastcall SetupCoordinateTransform();

public:		// User declarations
  __fastcall TDigitizePNGForm(TComponent* Owner);
  __fastcall ~TDigitizePNGForm();
  void __fastcall CloseIt();
  void __fastcall SetPoints( double, double );
  void __fastcall ChangeButton( bool );
  void __fastcall AbortDigitizing();
  void __fastcall StartDigitizing();
  void __fastcall SetPicked( int, int );
  void __fastcall SetDigitized( int, int );
  void __fastcall GetUserUnits( int ix, int iy, double &x, double &y );
};

extern PACKAGE TDigitizePNGForm *DigitizePNGForm;

#endif
