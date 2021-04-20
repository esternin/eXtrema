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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ProgressDisplay.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TProgressForm *ProgressForm;

__fastcall TProgressForm::TProgressForm(TComponent* Owner) : TForm(Owner)
{
  ProgressBar1->Min = 0;
  ProgressBar1->Step = 1;
}

void __fastcall TProgressForm::SetMax( int m )
{ ProgressBar1->Max = m; }

void __fastcall TProgressForm::Step()
{ ProgressBar1->StepIt(); }

// end of file
