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

#include "DigitizePNG.h"
#include "ProgressDisplay.h"
#include "GetCoordinates.h"

#include "PNGDisplay.h"

extern "C"
{
#include "gd.h"
}

#pragma package(smart_init)
#pragma resource "*.dfm"

TPNGForm *PNGForm;

__fastcall TPNGForm::TPNGForm(TComponent* Owner) : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "PNGForm", "Top", 220 );
  this->Left = ini->ReadInteger( "PNGForm", "Left", 850 );
  delete ini;
  digitizing_ = false;
  picking_ = false;
}

__fastcall TPNGForm::~TPNGForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "PNGForm", "Top", this->Top );
  ini->WriteInteger( "PNGForm", "Left", this->Left );
  delete ini;
}

void __fastcall TPNGForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  if( DigitizePNGForm )DigitizePNGForm->CloseIt();
  Action = caFree;
}

void __fastcall TPNGForm::FormClose2(TObject *Sender, bool &CanClose)
{
  if( DigitizePNGForm )DigitizePNGForm->CloseIt();
  PNGForm = 0;
}

void __fastcall TPNGForm::LoadPNG( FILE *f )
{
  gdImagePtr im = gdImageCreateFromPng( f );
  int h = gdImageSY(im);
  int w = gdImageSX(im);
  //
  this->Width = w+8;
  this->Height = h+89;
  int sbWidth = PNGStatusBar->Width;
  PNGStatusBar->Panels->Items[0]->Width = sbWidth/2;
  PNGStatusBar->Panels->Items[1]->Width = sbWidth/2;
  //
  Graphics::TBitmap *bitmap = PNGImage->Picture->Bitmap;
  bitmap->Width = w;
  bitmap->Height = h;
  bitmap->Canvas->CopyMode = cmWhiteness;
  TRect aRect = Rect( 0, 0, w, h );
  bitmap->Canvas->CopyRect( aRect, bitmap->Canvas, aRect );
  bitmap->Canvas->CopyMode = cmSrcCopy;
  //
  ProgressForm = new TProgressForm(this);
  ProgressForm->Top = Top+Height+5;
  ProgressForm->Left = Left+(Width-ProgressForm->Width)/2;
  ProgressForm->Show();
  ProgressForm->SetMax( h );
  //
  for( int j=0; j<h; ++j )
  {
    for( int i=0; i<w; ++i )
    {
      int c = gdImageGetPixel(im,i,j);
      AnsiString color( "0x00" );
      color += AnsiString(IntToHex(gdImageBlue(im,c),2)) +
               AnsiString(IntToHex(gdImageGreen(im,c),2)) +
               AnsiString(IntToHex(gdImageRed(im,c),2));
      bitmap->Canvas->Pixels[i][j] = StringToColor(color);
    }
    ProgressForm->Step();
  }
  delete ProgressForm;
  ProgressForm = 0;
}

void __fastcall TPNGForm::DigitizeClick(TObject *Sender)
{
  if( DigitizePNGForm )
  {
    DigitizePNGForm->WindowState = wsNormal;
    DigitizePNGForm->Visible = true;
    DigitizePNGForm->BringToFront();
  }
  else
  {
    DigitizePNGForm = new TDigitizePNGForm(this);
    DigitizePNGForm->Show();
  }
}

void __fastcall TPNGForm::StartDigitizing()
{ digitizing_ = true; }

void __fastcall TPNGForm::StopDigitizing()
{ digitizing_ = false; }

void __fastcall TPNGForm::StartPicking()
{ picking_ = true; }

void __fastcall TPNGForm::ClearStatusBar()
{
  PNGStatusBar->Panels->Items[0]->Text = AnsiString("");
  PNGStatusBar->Panels->Items[1]->Text = AnsiString("");
}

void __fastcall TPNGForm::ImageMouseUp( TObject *Sender, TMouseButton Button,
                                        TShiftState Shift, int x, int y )
{
  if( picking_ )
  {
    DigitizePNGForm->SetPicked( x, y );
    picking_ = false;
    std::auto_ptr<TGetCoordinatesForm> f( new TGetCoordinatesForm(this) );
    f->Top = y;
    f->Left = x;
    f->ShowModal();
  }
  else if( digitizing_ )
  {
    DigitizePNGForm->SetDigitized( x, y );
  }
}

void __fastcall TPNGForm::ImageMouseMove(TObject *Sender,TShiftState Shift,int ix,int iy)
{
  if( digitizing_ )
  {
    double xu, yu;
    DigitizePNGForm->GetUserUnits( ix, iy, xu, yu );
    char c[50];
    sprintf( c, "%f", xu );
    PNGStatusBar->Panels->Items[0]->Text = "x = " + AnsiString( c );
    sprintf( c, "%f", yu );
    PNGStatusBar->Panels->Items[1]->Text = "y = " + AnsiString( c );
  }
}

// end of file
