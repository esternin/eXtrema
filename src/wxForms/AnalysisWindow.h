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
#ifndef EXTREMA_ANALYSIS_WINDOW
#define EXTREMA_ANALYSIS_WINDOW

#include <iostream>
#include <vector>

#include "wx/wx.h"

class extrema;
class AnalysisMessagePanel;
class AnalysisCommandPanel;
class ReadVectorsForm;
class ReadScalarsForm;
class ReadMatrixForm;
class ReadTextForm;
class WriteVectorsForm;
class WriteScalarsForm;
class WriteMatrixForm;
class WriteTextForm;
class ExecuteDialog;
class ShowVariablesForm;
class GenerateVectorForm;
class FitForm;
class StackDialog;

// The AnalysisWindow is the main extrema window. It contains the command history
// and message text control and the command input text control, as well as various
// menus and speed buttons.

class AnalysisWindow : public wxFrame
{
public:
  AnalysisWindow( extrema * );
  
  void WriteOutput( wxString const & );
  void ClearOutput();
  void AddCommandString( wxString const & );
  std::vector<wxString> &GetCommandStrings();

  void ZeroExecuteDialog();
  void ZeroShowVariables();
  void ZeroLoadVectors();
  void ZeroLoadScalars();
  void ZeroLoadMatrix();
  void ZeroLoadText();
  void ZeroWriteVectors();
  void ZeroWriteScalars();
  void ZeroWriteMatrix();
  void ZeroWriteText();
  void ZeroGenerateVector();
  void ZeroFit();
  void ZeroStackDialog();

  FitForm *GetFitForm();
  void SetFitForm( FitForm * );

  ExecuteDialog *GetExecuteDialog();
  void SetExecuteDialog( ExecuteDialog * );

  GenerateVectorForm *GetGenerateVectorForm();
  void SetGenerateVectorForm( GenerateVectorForm * );

  // event handlers: these functions should NOT be virtual
  void OnQuit( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );
  void OnAbout( wxCommandEvent & );
  void OnHelp( wxCommandEvent & );
  void OnExecute( wxCommandEvent & );
  void OnShowVariables( wxCommandEvent & );
  void OnLoadVectors( wxCommandEvent & );
  void OnLoadScalars( wxCommandEvent & );
  void OnLoadMatrix( wxCommandEvent & );
  void OnLoadText( wxCommandEvent & );
  void OnWriteVectors( wxCommandEvent & );
  void OnWriteScalars( wxCommandEvent & );
  void OnWriteMatrix( wxCommandEvent & );
  void OnWriteText( wxCommandEvent & );
  void OnGenerateVector( wxCommandEvent & );
  void OnFit( wxCommandEvent & );
  void OnStackToggle( wxCommandEvent & );

  friend std::ostream &operator<<( std::ostream &, AnalysisWindow const * );

private:
  extrema *extrema_;
  AnalysisMessagePanel *messagePanel_;
  AnalysisCommandPanel *commandPanel_;
  ReadVectorsForm *readVectorsForm_;
  ReadScalarsForm *readScalarsForm_;
  ReadMatrixForm *readMatrixForm_;
  ReadTextForm *readTextForm_;
  WriteVectorsForm *writeVectorsForm_;
  WriteScalarsForm *writeScalarsForm_;
  WriteMatrixForm *writeMatrixForm_;
  WriteTextForm *writeTextForm_;
  ExecuteDialog *executeDialog_;
  ShowVariablesForm *showVariablesForm_;
  GenerateVectorForm *generateVectorForm_;
  FitForm *fitForm_;
  StackDialog *stackDialog_;

  enum {
      ID_execute,
      ID_loadVectors,
      ID_loadScalars,
      ID_loadMatrix,
      ID_loadText,
      ID_writeVectors,
      ID_writeScalars,
      ID_writeMatrix,
      ID_writeText
  };

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
