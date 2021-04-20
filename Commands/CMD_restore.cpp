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

#include <fstream>

#include "CMD_restore.h"
#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EExpressionError.h"
#include "NumericVariable.h"
#include "TextVariable.h"

extern "C"
{
#include "mud.h"
}

CMD_restore CMD_restore::cmd_restore_;

CMD_restore::CMD_restore() : Command( "RESTORE" )
{
  AddQualifier( "MUD", false );
  AddQualifier( "TITLE", false );
  AddQualifier( "MESSAGES", true );
  runNumber_ = 0;
  elapsedSec_ = 0;
}

void CMD_restore::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  ExString command( "RESTORE" );
  //
  bool output = qualifiers["MESSAGES"] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers["MESSAGES"] )output = false;
  //
  if( !qualifiers["MUD"] && qualifiers["TITLE"] )
    throw ECommandError( command, "\TITLE qualifier only applies to MUD files" );
  //
  if( p->GetNumberOfTokens()<2 )throw ECommandError(command,"filename not entered");
  ExString fileName;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, fileName );
  }
  catch (EVariableError &e)
  {
    throw;
  }
  std::ifstream in;
  in.open( fileName.c_str(), std::ios_base::in );
  ExString defaultExtension = qualifiers["MUD"] ? ".msr" : ".xml";
  if( !in.is_open() )
  {
    in.clear();
    //
    // get the file extension
    //
    ExString extension( fileName.ExtractFileExt() );
    if( extension.empty() )
    {
      fileName += defaultExtension;
      in.open( fileName.c_str(), std::ios_base::in );
    }
    if( !in.is_open() )
      throw ECommandError( command, ExString("could not open ")+fileName );
  }
  in.close();
  AddToStackLine( p->GetString(1) );
  if( qualifiers["MUD"] )
  {
    try
    {
      RestoreMUD( p, command, qualifiers["TITLE"], output, fileName );
    }
    catch ( ECommandError &e )
    {
      throw;
    }
  }
  else
  {
    try
    {
      ExGlobals::RestoreSession( fileName );
    }
    catch (runtime_error &rte)
    {
      throw ECommandError( "RESTORE", rte.what() );
    }
  }
}

void CMD_restore::RestoreMUD( ParseLine const *p, ExString &command,
                              bool titleFlag, bool output,
                              ExString const &fileName )
{
  command += "\\MUD";
  int fileHandle;
  unsigned long dum;
  fileHandle = MUD_openRead( const_cast<char*>(fileName.c_str()), &dum );
  if( fileHandle == -1 )
    throw ECommandError( command, ExString("could not open file ")+fileName );
  //
  ExString name;
  int const MAXSTRINGLENGTH = 80;
  int len = MAXSTRINGLENGTH;
  if( titleFlag )
  {
    char title[MAXSTRINGLENGTH];
    if( !MUD_getTitle( fileHandle, title ) )
      throw ECommandError( command, "could not read title" );
    try
    {
      TextVariable::PutVariable( "RUNTITLE", ExString(title), p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "RESTORE", e.what() );
    }
    if( output )
      ExGlobals::WriteOutput( ExString("text variable RUNTITLE= \"")+ExString(title)+"\"" );
  }
  unsigned long runNumber;
  if( !MUD_getRunNumber( fileHandle, &runNumber ) )
    throw ECommandError( command, "could not get run number" );
  unsigned long elapsedSec;
  if( !MUD_getElapsedSec( fileHandle, &elapsedSec ) )
    throw ECommandError( command, "could not get elapsed seconds" );
  //
  if( runNumber == runNumber_ && elapsedSec == elapsedSec_ )return;
  runNumber_ = runNumber;
  elapsedSec_ = elapsedSec;
  //
  try
  {
    NumericVariable::PutVariable( "RUNNUMBER", static_cast<double>(runNumber), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  try
  {
    NumericVariable::PutVariable( "ELAPSEDSEC", static_cast<double>(elapsedSec), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  unsigned long runDesc;
  if( !MUD_getRunDesc( fileHandle, &runDesc ) )
    throw ECommandError( command, "could not get run descriptor" );
  try
  {
    NumericVariable::PutVariable( "RUNDESC", static_cast<double>(runDesc), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  unsigned long exptNumber;
  if( !MUD_getExptNumber( fileHandle, &exptNumber ) )
    throw ECommandError( command, "could not get exptNumber" );
  try
  {
    NumericVariable::PutVariable( "EXPTNUMBER", static_cast<double>(exptNumber), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  unsigned long timeBegin;
  if( !MUD_getTimeBegin( fileHandle, &timeBegin ) )
    throw ECommandError( command, "could not get timeBegin" );
  try
  {
    NumericVariable::PutVariable( "TIMEBEGIN", static_cast<double>(timeBegin), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  unsigned long timeEnd;
  if( !MUD_getTimeEnd( fileHandle, &timeEnd ) )
    throw ECommandError( command, "could not get timeEnd" );
  try
  {
    NumericVariable::PutVariable( "TIMEEND", static_cast<double>(timeEnd), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  char title[MAXSTRINGLENGTH];
  if( !MUD_getTitle( fileHandle, title ) )
    throw ECommandError( command, "could not get title" );
  try
  {
    TextVariable::PutVariable( "MUDTITLE", ExString(title), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char lab[MAXSTRINGLENGTH];
  if( !MUD_getLab( fileHandle, lab ) )
    throw ECommandError( command, "could not get lab" );
  try
  {
    TextVariable::PutVariable( "LAB", ExString(lab), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char area[MAXSTRINGLENGTH];
  if( !MUD_getArea( fileHandle, area ) )
    throw ECommandError( command, "could not get area" );
  try
  {
    TextVariable::PutVariable( "MUDAREA", ExString(area), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char method[MAXSTRINGLENGTH];
  if( !MUD_getMethod( fileHandle, method ) )
    throw ECommandError( command, "could not get method" );
  try
  {
    TextVariable::PutVariable( "METHOD", ExString(method), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char apparatus[MAXSTRINGLENGTH];
  if( !MUD_getApparatus( fileHandle, apparatus ) )
    throw ECommandError( command, "could not get apparatus" );
  try
  {
    TextVariable::PutVariable( "APPARATUS", ExString(apparatus), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char insert[MAXSTRINGLENGTH];
  if( !MUD_getInsert( fileHandle, insert ) )
    throw ECommandError( command, "could not get insert" );
  try
  {
    TextVariable::PutVariable( "INSERT", ExString(insert), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char sample[MAXSTRINGLENGTH];
  if( !MUD_getSample( fileHandle, sample ) )
    throw ECommandError( command, "could not get sample" );
  try
  {
    TextVariable::PutVariable( "SAMPLE", ExString(sample), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char orient[MAXSTRINGLENGTH];
  if( !MUD_getOrient( fileHandle, orient ) )
    throw ECommandError( command, "could not get orient" );
  try
  {
    TextVariable::PutVariable( "ORIENT", ExString(orient), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char temperature[MAXSTRINGLENGTH];
  if( !MUD_getTemperature( fileHandle, temperature ) )
    throw ECommandError( command, "could not get temperature" );
  try
  {
    TextVariable::PutVariable( "TEMPERATURE", ExString(temperature), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char field[MAXSTRINGLENGTH];
  if( !MUD_getField( fileHandle, field ) )
    throw ECommandError( command, "could not get field" );
  try
  {
    TextVariable::PutVariable( "FIELD", ExString(field), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char das[MAXSTRINGLENGTH];
  if( !MUD_getDas( fileHandle, das ) )
    throw ECommandError( command, "could not get das" );
  try
  {
    TextVariable::PutVariable( "DAS", ExString(das), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  char experimenter[MAXSTRINGLENGTH];
  if( !MUD_getExperimenter( fileHandle, experimenter ) )
    throw ECommandError( command, "could not get experimenter" );
  try
  {
    TextVariable::PutVariable( "EXPERIMENTER", ExString(experimenter), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  unsigned long pType, nComments;
  if( !MUD_getComments( fileHandle, &pType, &nComments ) )
  {
    pType = 0;
    nComments = 0;
  }
  try
  {
    NumericVariable::PutVariable( "COMMENTTYPE", static_cast<double>(pType), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  try
  {
    NumericVariable::PutVariable( "COMMENTNUM", static_cast<double>(nComments), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  if( nComments > 0 )
  {
    std::vector<double> data;
    for( unsigned long i=1; i<=nComments; ++i )
    {
      unsigned long a;
      if( !MUD_getCommentPrev( fileHandle, i, &a ) )
        throw ECommandError( command, "could not get commentPrev" );
      data.push_back(a);
    }
    try
    {
      NumericVariable::PutVariable( "COMMENTPREV", data, 0, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=nComments; ++i )
    {
      unsigned long a;
      if( !MUD_getCommentNext( fileHandle, i, &a ) )
        throw ECommandError( command, "could not get commentNext" );
      data.push_back(a);
    }
    try
    {
      NumericVariable::PutVariable( "COMMENTNEXT", data, 0, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=nComments; ++i )
    {
      unsigned long a;
      if( !MUD_getCommentTime( fileHandle, i, &a ) )
        throw ECommandError( command, "could not get commentTime" );
      data.push_back(a);
    }
    try
    {
      NumericVariable::PutVariable( "COMMENTTIME", data, 0, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<ExString> sdata;
    for( unsigned long i=1; i<=nComments; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getCommentAuthor( fileHandle, i, a ) )
        throw ECommandError( command, "could not get commentAuthor" );
      sdata.push_back(ExString(a));
    }
    try
    {
      TextVariable::PutVariable( "COMMENTAUTHOR", sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
    sdata.clear();
    for( unsigned long i=1; i<=nComments; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getCommentTitle( fileHandle, i, a ) )
        throw ECommandError( command, "could not get commentTitle" );
      sdata.push_back(ExString(a));
    }
    try
    {
      TextVariable::PutVariable( "COMMENTTITLE", sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
    sdata.clear();
    for( unsigned long i=1; i<=nComments; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getCommentBody( fileHandle, i, a ) )
        throw ECommandError( command, "could not get commentBody" );
      sdata.push_back(ExString(a));
    }
    try
    {
      TextVariable::PutVariable( "COMMENTBODY", sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
  }
  unsigned long pNum;
  if( !MUD_getHists( fileHandle, &pType, &pNum ) )
    throw ECommandError( command, "could not get hists" );
  try
  {
    NumericVariable::PutVariable( "HISTNUM", static_cast<double>(pNum), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  unsigned long nbintot = 0;
  unsigned long start = 0;
  std::vector<ExString> sdata;
  std::vector<double> nbins, starts, ends;
  for( unsigned long i=1; i<=pNum; ++i )
  {
    char c[MAXSTRINGLENGTH];
    if( !MUD_getHistTitle( fileHandle, i, c ) )
      throw ECommandError( command, "could not get histTitle" );
    sdata.push_back(ExString(c));
    unsigned long p;
    if( !MUD_getHistNumBins( fileHandle, i, &p ) )
      throw ECommandError( command, "could not get histNumBins" );
    nbins.push_back(static_cast<double>(p));
    nbintot += p;
    starts.push_back(static_cast<double>(start+1));
    start += p;
    ends.push_back(static_cast<double>(start));
  }
  try
  {
    TextVariable::PutVariable( "HISTTITLE", sdata, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
  try
  {
    NumericVariable::PutVariable( "NUMBINS", nbins, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  try
  {
    NumericVariable::PutVariable( "HISTSTART", starts, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  try
  {
    NumericVariable::PutVariable( "HISTEND", ends, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  int *pdata = new int[nbintot];
  int istart = 0;
  for( unsigned long i=1; i<=pNum; ++i )
  {
    if( !MUD_getHistData( fileHandle, i, pdata+istart ) )
    {
      delete [] pdata;
      throw ECommandError( command, "could not get histData" );
    }
    istart += static_cast<int>(nbins[i-1]);
  }
  std::vector<double> data;
  for( unsigned long i=0; i<nbintot; ++i )
    data.push_back( static_cast<double>(pdata[i]) );
  delete [] pdata;
  try
  {
    NumericVariable::PutVariable( "IH", data, (std::size_t)nbins[0],
                                  (std::size_t)pNum, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistType( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histType" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTTYPE", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistNumBytes( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histNumBytes" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTNUMBYTES", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistBytesPerBin( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histBytesPerBin" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTBYTESPERBIN", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistFsPerBin( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histFsPerBin" );
    double d = static_cast<double>(j);
    if( j < 16 )d = 78125*pow(2.0,d);
    else if( j < 32 )d = 48828.125*pow(2.0,d-16);
    data.push_back(d);
  }
  try
  {
    NumericVariable::PutVariable( "HISTFSPERBIN", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistT0_Ps( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histT0_Ps" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTT0_PS", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistT0_Bin( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histT0_Bin" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTT0_BIN", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistGoodBin1( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histGoodBin1" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTGOODBIN1", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistGoodBin2( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histGoodBin2" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTGOODBIN2", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistBkgd1( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histBkgd1" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTBKGD1", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistBkgd2( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histBkgd2" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "HISTBKGD2", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistNumEvents( fileHandle, i, &j ) )
      throw ECommandError( command, "could not get histNumEvents" );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    NumericVariable::PutVariable( "NUMEVENTS", data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  unsigned long scalerType, nscalers;
  if( !MUD_getScalers( fileHandle, &scalerType, &nscalers ) )
   throw ECommandError( command, "could not get scalers" );
  try
  {
    NumericVariable::PutVariable( "NUMSCALERS", static_cast<double>(nscalers), p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command, e.what() );
  }
  if( nscalers > 0 )
  {
    try
    {
      NumericVariable::PutVariable( "SCALERTYPE", static_cast<double>(scalerType), p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<double>().swap( data );
    data.resize( 2*nscalers );
    unsigned long pCounts[2];
    for( unsigned long i=1; i<=nscalers; ++i )
    {
      if( !MUD_getScalerCounts( fileHandle, i, pCounts ) )
        throw ECommandError( command, "could not get scalerCounts" );
      data[i-1] = static_cast<double>(pCounts[0]);
      data[i-1+nscalers] = static_cast<double>(pCounts[1]);
    }
    try
    {
      NumericVariable::PutVariable( "SCALERCOUNTS", data, nscalers, 2, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<ExString> sdata;
    for( unsigned long i=1; i<=nscalers; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getScalerLabel( fileHandle, i, a ) )
        throw ECommandError( command, "could not get scalerLabel" );
      ExString s(a);
      if( strlen(a) == 0 )sdata.push_back(ExString(" "));
      else                sdata.push_back(ExString(a));
    }
    try
    {
      TextVariable::PutVariable( "SCALERLABEL", sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
  }
  unsigned long indvartype, indvarnum;
  if( MUD_getIndVars( fileHandle, &indvartype, &indvarnum ) == 0 )
    ExGlobals::WarningMessage( command + "could not get indVars" );
  else
  {
    MUD_getIndVars( fileHandle, &indvartype, &indvarnum );
    try
    {
      NumericVariable::PutVariable( "INDVARTYPE", static_cast<double>(indvartype), p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarLow( fileHandle, i, &tmp ) )
        throw ECommandError( command, "could not get indVarLow" );
      data.push_back(tmp);
    }
    try
    {
      NumericVariable::PutVariable( "INDVARLOW", data, 0, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarHigh( fileHandle, i, &tmp ) )
        throw ECommandError( command, "could not get indVarHigh" );
      data.push_back(tmp);
    }
    try
    {
      NumericVariable::PutVariable( "INDVARHIGH", data, 0, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarMean( fileHandle, i, &tmp ) )
        throw ECommandError( command, "could not get indVarMean" );
      data.push_back(tmp);
    }
    try
    {
      NumericVariable::PutVariable( "INDVARMEAN", data, 0, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarStddev( fileHandle, i, &tmp ) )
        throw ECommandError( command, "could not get indVarStddev" );
      data.push_back(tmp);
    }
    try
    {
      NumericVariable::PutVariable( "INDVARSTDDEV", data, 0, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarSkewness( fileHandle, i, &tmp ) )
        throw ECommandError( command, "could not get indVarSkewness" );
      data.push_back(tmp);
    }
    try
    {
      NumericVariable::PutVariable( "INDVARSKEWNESS", data, 0, p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    sdata.clear();
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getIndVarName( fileHandle, i, a ) )
        throw ECommandError( command, "could not get indVarName" );
      sdata.push_back(ExString(a));
    }
    try
    {
      TextVariable::PutVariable( "INDVARNAME", sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
    sdata.clear();
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getIndVarDescription( fileHandle, i, a ) )
        throw ECommandError( command, "could not get indVarDescription" );
      sdata.push_back(ExString(a));
    }
    try
    {
      TextVariable::PutVariable( "INDVARDESCRIPTION", sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
    sdata.clear();
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getIndVarUnits( fileHandle, i, a ) )
        throw ECommandError( command, "could not get indVarUnits" );
      sdata.push_back(ExString(a));
    }
    try
    {
      TextVariable::PutVariable( "INDVARUNITS", sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
  }
}

// end of file
