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
#ifndef CMD_READ
#define CMD_READ

#include "ExString.h"
#include "Command.h"

#define COMMANDS_LINKAGE_
/*
#ifdef BUILDING_COMMANDS_DLL_
#define COMMANDS_LINKAGE_ __declspec(dllexport)
#else
#define COMMANDS_LINKAGE_ __declspec(dllimport)
#endif
*/

class ParseLine;

class COMMANDS_LINKAGE_ CMD_read : public Command
{
public:
  static CMD_read *Definition()
  { return &cmd_read_; }

  void SetErrorFill( double d )
  { errorFill_ = d; }
  
  double const GetErrorFill() const
  { return errorFill_; }
  
  ExString &GetFilename()
  { return readInFilename_; }
  
  void SetReadInFilename( ExString const &s )
  { readInFilename_ = s; }
  
  std::ifstream *GetStream()
  { return &inStream_; }

  void Execute( ParseLine const * );

private:
  CMD_read();
  static CMD_read cmd_read_;
  
  std::ifstream inStream_;
  ExString readInFilename_;
  int startingLine_;
  std::vector<double> lineRange_;
  int lineRangeFlag_;
  double errorFill_;
  
  void ReadScalars( QualifierMap &, bool const );
  void ReadMatrix( QualifierMap &, bool );
  void ReadText( QualifierMap &, bool const );
  void ReadVectors( QualifierMap &, bool const );
  void ReadTensor( QualifierMap &, bool const );
};

#endif
