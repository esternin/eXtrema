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

#include "FunctionTable.h"
#include "FunctionDefinition.h"

FunctionDefinition::FunctionDefinition( char const *name, std::size_t minArg, std::size_t maxArg,
                                        Type fcnType,
                                        ArgType a1, ArgType a2, ArgType a3,
                                        ArgType a4, ArgType a5, ArgType a6,
                                        ArgType a7, ArgType a8, ArgType a9,
                                        ArgType a10, ArgType a11, ArgType a12,
                                        ArgType a13, ArgType a14, ArgType a15,
                                        ArgType a16, ArgType a17, ArgType a18,
                                        ArgType a19, ArgType a20 )
    : name_(name), minimumNumberOfArguments_(minArg), maximumNumberOfArguments_(maxArg), type_(fcnType)
{
  argumentType_[0] = a1;
  argumentType_[1] = a2;
  argumentType_[2] = a3;
  argumentType_[3] = a4;
  argumentType_[4] = a5;
  argumentType_[5] = a6;
  argumentType_[6] = a7;
  argumentType_[7] = a8;
  argumentType_[8] = a9;
  argumentType_[9] = a10;
  argumentType_[10] = a11;
  argumentType_[11] = a12;
  argumentType_[12] = a13;
  argumentType_[13] = a14;
  argumentType_[14] = a15;
  argumentType_[15] = a16;
  argumentType_[16] = a17;
  argumentType_[17] = a18;
  argumentType_[18] = a19;
  argumentType_[19] = a20;
  FunctionTable::GetTable()->AddEntry( this );
}

void FunctionDefinition::Dump() const
{
  /*
  ExString message = "--- FunctionDefinition ---\n";
  message += "Function Name : " + name_ + "\n";
  char temp[20];
  sprintf( temp, "min # arg's : %2d", minimumNumberOfArguments_ );
  message += temp;
  sprintf( temp, ", max # arg's : %2d\n", maximumNumberOfArguments_ );
  message += temp;
  sprintf( temp, "type : %2d\n", type_ );
  message += temp;
  sprintf( temp, "arg1type : %2d", argumentType_[0] );
  message += temp;
  sprintf( temp, ", arg2type : %2d\n", argumentType_[1] );
  message += temp;
  sprintf( temp, "arg3type : %2d", argumentType_[2] );
  message += temp;
  sprintf( temp, ", arg4type : %2d\n", argumentType_[3] );
  message += temp;
  Application->MessageBox( const_cast<char*>(message.c_str()), "Function Definition Dump", MB_OK );
  */
}

// end of file
