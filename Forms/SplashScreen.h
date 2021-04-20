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
#ifndef SplashScreenH
#define SplashScreenH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>

class TSplashScreenForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TLabel *VersionLabel;
  TLabel *CopyrightLabel;
  TLabel *WelcomeLabel;
  TTimer *Timer1;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  void __fastcall Timer1Timer(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
  __fastcall TSplashScreenForm(TComponent* Owner);
};

extern PACKAGE TSplashScreenForm *SplashScreenForm;

#endif
