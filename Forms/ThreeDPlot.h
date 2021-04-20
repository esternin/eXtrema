/*
Copyright (C) 2010 Joseph L. Chuma

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
#ifndef ThreeDPlotH
#define ThreeDPlotH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include <vector>

class GRA_threeDPlot;

class TThreeDPlotForm : public TForm
{
__published:	// IDE-managed Components
        TButton *CloseB;
        TPanel *ParameterPanel;
        TSpeedButton *RotateLeftSB;
        TSpeedButton *RotateRightSB;
        TSpeedButton *RotateUpSB;
        TSpeedButton *RotateDownSB;
        TSpeedButton *ZoomInSB;
        TSpeedButton *ZoomOutSB;
        TLabel *AngleIncL;
        TLabel *EyeL;
        TEdit *AngleIncE;
        TEdit *EyeE;
        TPanel *VectorPanel;
        TLabel *XVecLabel;
        TComboBox *XVecComboBox;
        TLabel *YVecLabel;
        TComboBox *YVecComboBox;
        TLabel *ZVecLabel;
        TComboBox *ZVecComboBox;
        TButton *DrawButton;
        void __fastcall UpdateVectors(TObject *Sender);
        void __fastcall DrawClick(TObject *Sender);
        void __fastcall CloseButtonClick(TObject *Sender);
        void __fastcall RotateLeftClick(TObject *Sender);
        void __fastcall RotateRightClick(TObject *Sender);
        void __fastcall AngleIncChange(TObject *Sender);
        void __fastcall RotateUpClick(TObject *Sender);
        void __fastcall RotateDownClick(TObject *Sender);
        void __fastcall ZoomInClick(TObject *Sender);
        void __fastcall ZoomOutClick(TObject *Sender);
        void __fastcall Eye2ObjectChange(TObject *Sender);

private:	// User declarations
        GRA_threeDPlot *threeDPlot_;
        double angleIncrement_;
        bool initializing_;

        void GetDataVectors( std::vector<double> &, std::vector<double> &, std::vector<double> & );
        void ReDraw();
        
public:		// User declarations
        __fastcall TThreeDPlotForm( TComponent *Owner );
        __fastcall ~TThreeDPlotForm();

        void ZeroPlot()
        { threeDPlot_ = 0; }
};

extern PACKAGE TThreeDPlotForm *ThreeDPlotForm;

#endif
