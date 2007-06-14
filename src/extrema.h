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
#ifndef EXTREMA_APP
#define EXTREMA_APP

#define wxUSE_UNICODE 1

#include "wx/wx.h"
#include "wx/config.h"

class AnalysisWindow;

class extrema : public wxApp
{
public:
  // OnInit is called on application startup and is a good place for the app
  // initialization (doing it here and not in the ctor allows to have an error
  // return: if OnInit() returns false, the application terminates)
  bool OnInit();

  // OnExit is called after destroying all application windows and controls,
  // but before wxWidgets cleanup.  It is not called at all if OnInit fails.
  int OnExit()
  {
    wxConfigBase *config = wxConfigBase::Get();
    if( config )delete config;
  }

  void WriteOutput( wxString const & );

  void QuitApp();

private:
  AnalysisWindow *analysisWindow_;
};

#endif
