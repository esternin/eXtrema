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
#include <cmath>
#include <fstream>

#include "wx/wx.h"

#include "CMD_restore.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "TextVariable.h"

#ifdef HAVE_MUD
extern "C"
{
#include "mud.h"
}
#endif

CMD_restore *CMD_restore::cmd_restore_ = 0;

CMD_restore::CMD_restore() : Command( wxT("RESTORE") )
{
  AddQualifier( wxT("MUD"), false );
  AddQualifier( wxT("TITLE"), false );
  AddQualifier( wxT("MESSAGES"), true );
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
  wxString command( wxT("RESTORE: ") );
  //
  bool output = qualifiers[wxT("MESSAGES")] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers[wxT("MESSAGES")] )output = false;
  //
  if( !qualifiers[wxT("MUD")] && qualifiers[wxT("TITLE")] )
    throw ECommandError( command+wxT("\\TITLE qualifier only applies to MUD files") );
  //
  if( p->GetNumberOfTokens() < 2 )throw ECommandError( command+wxT("filename not entered") );
  wxString fileName;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, fileName );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::ifstream in;
  in.open( fileName.mb_str(wxConvUTF8), std::ios_base::in );
  wxString defaultExtension = qualifiers[wxT("MUD")] ? wxT(".msr") : wxT(".xml");
  if( !in.is_open() )
  {
    in.clear();
    //
    // get the file extension, if there was one given
    //
    wxString extension( ExGlobals::GetFileExtension(fileName) );
    if( extension.empty() ) // no extension given, append default extension to the filename
    {
      fileName += defaultExtension;
      in.open( fileName.mb_str(wxConvUTF8), std::ios_base::in );
    }
    if( !in.is_open() )
      throw ECommandError( wxString()<<command<<wxT("could not open ")<<fileName );
  }
  in.close();
  AddToStackLine( p->GetString(1) );
  //
  if( qualifiers[wxT("MUD")] )
  {
#ifdef HAVE_MUD
    try
    {
      RestoreMUD( p, command, qualifiers[wxT("TITLE")], output, fileName );
    }
    catch ( ECommandError &e )
    {
      throw;
    }
#else
    throw ECommandError( wxString()<<command<<wxT("the MUon Data package is not available") );
#endif
  }
  else
  {
    try
    {
      ExGlobals::RestoreSession( fileName );
    }
    catch ( std::runtime_error &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
}

void CMD_restore::RestoreMUD( ParseLine const *p, wxString &command,
                              bool titleFlag, bool output,
                              wxString const &fileName )
{
#ifdef HAVE_MUD
  command += wxT("\\MUD");
  int fileHandle;
  unsigned long dum;
  fileHandle = MUD_openRead( const_cast<char*>((char const*)fileName.mb_str(wxConvUTF8)), &dum );
  if( fileHandle == -1 )
    throw ECommandError( wxString()<<command<<wxT(" could not open file ")<<fileName );
  //
  int const MAXSTRINGLENGTH = 80;
  wxString name;
  int len = MAXSTRINGLENGTH;
  if( titleFlag )
  {
    char title[MAXSTRINGLENGTH];
    if( !MUD_getTitle( fileHandle, title, len ) )
      throw ECommandError( command+wxT(" could not read title") );
    try
    {
      name = wxT("RUNTITLE");
      TextVariable::PutVariable( name, wxString(title,wxConvUTF8), p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )
      ExGlobals::WriteOutput( wxString(wxT("text variable RUNTITLE= \""))+
                              wxString(title,wxConvUTF8)+wxT("\"") );
    return;
  }
  unsigned long runNumber;
  if( !MUD_getRunNumber( fileHandle, &runNumber ) )
    throw ECommandError( command+wxT(" could not get run number") );
  unsigned long elapsedSec;
  if( !MUD_getElapsedSec( fileHandle, &elapsedSec ) )
    throw ECommandError( command+wxT(" could not get elapsed seconds") );
  //
  if( runNumber == runNumber_ && elapsedSec == elapsedSec_ )return;
  runNumber_ = runNumber;
  elapsedSec_ = elapsedSec;
  //
  try
  {
    name = wxT("RUNNUMBER");
    NumericVariable::PutVariable( name, (double)runNumber, p->GetInputLine() );
    name = wxT("ELAPSEDSEC");
    NumericVariable::PutVariable( name, (double)elapsedSec, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  unsigned long runDesc;
  if( !MUD_getRunDesc( fileHandle, &runDesc ) )
    throw ECommandError( command+wxT(" could not get run descriptor") );
  try
  {
    name = wxT("RUNDESC");
    NumericVariable::PutVariable( name, (double)runDesc, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  unsigned long exptNumber;
  if( !MUD_getExptNumber( fileHandle, &exptNumber ) )
    throw ECommandError( command+wxT(" could not get exptNumber") );
  try
  {
    name = wxT("EXPTNUMBER");
    NumericVariable::PutVariable( name, (double)exptNumber, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  unsigned long timeBegin;
  if( !MUD_getTimeBegin( fileHandle, &timeBegin ) )
    throw ECommandError( command+wxT(" could not get timeBegin") );
  try
  {
    name = wxT("TIMEBEGIN");
    NumericVariable::PutVariable( name, (double)timeBegin, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  unsigned long timeEnd;
  if( !MUD_getTimeEnd( fileHandle, &timeEnd ) )
    throw ECommandError( command+wxT(" could not get timeEnd") );
  try
  {
    name = wxT("TIMEEND");
    NumericVariable::PutVariable( name, (double)timeEnd, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char title[MAXSTRINGLENGTH];
  if( !MUD_getTitle( fileHandle, title, len ) )
    throw ECommandError( command+wxT(" could not get title") );
  try
  {
    name = wxT("MUDTITLE");
    TextVariable::PutVariable( name, wxString(title,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char lab[MAXSTRINGLENGTH];
  if( !MUD_getLab( fileHandle, lab, len ) )
    throw ECommandError( command+wxT(" could not get lab") );
  try
  {
    name = wxT("LAB");
    TextVariable::PutVariable( name, wxString(lab,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char area[MAXSTRINGLENGTH];
  if( !MUD_getArea( fileHandle, area, len ) )
    throw ECommandError( command+wxT(" could not get area") );
  try
  {
    name = wxT("MUDAREA");
    TextVariable::PutVariable( name, wxString(area,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char method[MAXSTRINGLENGTH];
  if( !MUD_getMethod( fileHandle, method, len ) )
    throw ECommandError( command+wxT(" could not get method") );
  try
  {
    name = wxT("METHOD");
    TextVariable::PutVariable( name, wxString(method,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char apparatus[MAXSTRINGLENGTH];
  if( !MUD_getApparatus( fileHandle, apparatus, len ) )
    throw ECommandError( command+wxT(" could not get apparatus") );
  try
  {
    name = wxT("APPARATUS");
    TextVariable::PutVariable( name, wxString(apparatus,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char insert[MAXSTRINGLENGTH];
  if( !MUD_getInsert( fileHandle, insert, len ) )
    throw ECommandError( command+wxT(" could not get insert") );
  try
  {
    name = wxT("INSERT");
    TextVariable::PutVariable( name, wxString(insert,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char sample[MAXSTRINGLENGTH];
  if( !MUD_getSample( fileHandle, sample, len ) )
    throw ECommandError( command+wxT(" could not get sample") );
  try
  {
    name = wxT("SAMPLE");
    TextVariable::PutVariable( name, wxString(sample,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char orient[MAXSTRINGLENGTH];
  if( !MUD_getOrient( fileHandle, orient, len ) )
    throw ECommandError( command+wxT(" could not get orient") );
  try
  {
    name = wxT("ORIENT");
    TextVariable::PutVariable( name, wxString(orient,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char temperature[MAXSTRINGLENGTH];
  if( !MUD_getTemperature( fileHandle, temperature, len ) )
    throw ECommandError( command+wxT(" could not get temperature") );
  try
  {
    name = wxT("TEMPERATURE");
    TextVariable::PutVariable( name, wxString(temperature,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char field[MAXSTRINGLENGTH];
  if( !MUD_getField( fileHandle, field, len ) )
    throw ECommandError( command+wxT(" could not get field") );
  try
  {
    name = wxT("FIELD");
    TextVariable::PutVariable( name, wxString(field,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char das[MAXSTRINGLENGTH];
  if( !MUD_getDas( fileHandle, das, len ) )
    throw ECommandError( command+wxT(" could not get das") );
  try
  {
    name = wxT("DAS");
    TextVariable::PutVariable( name, wxString(das,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  char experimenter[MAXSTRINGLENGTH];
  if( !MUD_getExperimenter( fileHandle, experimenter, len ) )
    throw ECommandError( command+wxT(" could not get experimenter") );
  try
  {
    name = wxT("EXPERIMENTER");
    TextVariable::PutVariable( name, wxString(experimenter,wxConvUTF8), p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  unsigned long pType, nComments;
  if( !MUD_getComments( fileHandle, &pType, &nComments ) )
  {
    pType = 0;
    nComments = 0;
  }
  try
  {
    name = wxT("COMMENTTYPE");
    NumericVariable::PutVariable( name, (double)pType, p->GetInputLine() );
    name = wxT("COMMENTNUM");
    NumericVariable::PutVariable( name, (double)nComments, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( nComments > 0 )
  {
    std::vector<double> data;
    for( unsigned long i=1; i<=nComments; ++i )
    {
      unsigned long a;
      if( !MUD_getCommentPrev( fileHandle, i, &a ) )
        throw ECommandError( command+wxT(" could not get commentPrev") );
      data.push_back(a);
    }
    try
    {
      name = wxT("COMMENTPREV");
      NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=nComments; ++i )
    {
      unsigned long a;
      if( !MUD_getCommentNext( fileHandle, i, &a ) )
        throw ECommandError( command+wxT(" could not get commentNext") );
      data.push_back(a);
    }
    try
    {
      name = wxT("COMMENTNEXT");
      NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=nComments; ++i )
    {
      unsigned long a;
      if( !MUD_getCommentTime( fileHandle, i, &a ) )
        throw ECommandError( command+wxT(" could not get commentTime") );
      data.push_back(a);
    }
    try
    {
      name = wxT("COMMENTTIME");
      NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<wxString> sdata;
    for( unsigned long i=1; i<=nComments; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getCommentAuthor( fileHandle, i, a, len ) )
        throw ECommandError( command+wxT(" could not get commentAuthor") );
      sdata.push_back(wxString(a,wxConvUTF8));
    }
    try
    {
      name = wxT("COMMENTAUTHOR");
      TextVariable::PutVariable( name, sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    sdata.clear();
    for( unsigned long i=1; i<=nComments; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getCommentTitle( fileHandle, i, a, len ) )
        throw ECommandError( command+wxT(" could not get commentTitle") );
      sdata.push_back(wxString(a,wxConvUTF8));
    }
    try
    {
      name = wxT("COMMENTTITLE");
      TextVariable::PutVariable( name, sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    sdata.clear();
    for( unsigned long i=1; i<=nComments; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getCommentBody( fileHandle, i, a, len ) )
        throw ECommandError( command+wxT(" could not get commentBody") );
      sdata.push_back(wxString(a,wxConvUTF8));
    }
    try
    {
      name = wxT("COMMENTBODY");
      TextVariable::PutVariable( name, sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  unsigned long pNum;
  if( !MUD_getHists( fileHandle, &pType, &pNum ) )
    throw ECommandError( command+wxT(" could not get hists") );
  try
  {
    name = wxT("HISTNUM");
    NumericVariable::PutVariable( name, (double)pNum, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  unsigned long nbintot = 0;
  unsigned long start = 0;
  std::vector<wxString> sdata;
  std::vector<double> nbins, starts, ends;
  for( unsigned long i=1; i<=pNum; ++i )
  {
    char c[MAXSTRINGLENGTH];
    if( !MUD_getHistTitle( fileHandle, i, c, len ) )
      throw ECommandError( command+wxT(" could not get histTitle") );
    sdata.push_back(wxString(c,wxConvUTF8));
    unsigned long p;
    if( !MUD_getHistNumBins( fileHandle, i, &p ) )
      throw ECommandError( command+wxT(" could not get histNumBins") );
    nbins.push_back(static_cast<double>(p));
    nbintot += p;
    starts.push_back(static_cast<double>(start+1));
    start += p;
    ends.push_back(static_cast<double>(start));
  }
  try
  {
    name = wxT("HISTTITLE");
    TextVariable::PutVariable( name, sdata, p->GetInputLine() );
    name = wxT("NUMBINS");
    NumericVariable::PutVariable( name, nbins, 0, p->GetInputLine() );
    name = wxT("HISTSTART");
    NumericVariable::PutVariable( name, starts, 0, p->GetInputLine() );
    name = wxT("HISTEND");
    NumericVariable::PutVariable( name, ends, 0, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  int *pdata = new int[nbintot];
  int istart = 0;
  for( unsigned long i=1; i<=pNum; ++i )
  {
    if( !MUD_getHistData( fileHandle, i, pdata+istart ) )
    {
      delete [] pdata;
      throw ECommandError( command+wxT(" could not get histData") );
    }
    istart += static_cast<int>(nbins[i-1]);
  }
  std::vector<double> data;
  for( unsigned long i=0; i<nbintot; ++i )data.push_back( static_cast<double>(pdata[i]) );
  delete [] pdata;
  try
  {
    name = wxT("IH");
    NumericVariable::PutVariable( name, data, (std::size_t)nbins[0],
                                  (std::size_t)pNum, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistType( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histType") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTTYPE");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistNumBytes( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histNumBytes") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTNUMBYTES");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistBytesPerBin( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histBytesPerBin") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTBYTESPERBIN");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistFsPerBin( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histFsPerBin") );
    double d = static_cast<double>(j);
    if( j < 16 )d = 78125*pow(2.0,d);
    else if( j < 32 )d = 48828.125*pow(2.0,d-16);
    data.push_back(d);
  }
  try
  {
    name = wxT("HISTFSPERBIN");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistT0_Ps( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histT0_Ps") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTT0_PS");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistT0_Bin( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histT0_Bin") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTT0_BIN");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistGoodBin1( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histGoodBin1") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTGOODBIN1");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistGoodBin2( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histGoodBin2") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTGOODBIN2");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistBkgd1( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histBkgd1") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTBKGD1");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistBkgd2( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histBkgd2") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("HISTBKGD2");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  std::vector<double>().swap( data );
  for( unsigned long i=1; i<=pNum; ++i )
  {
    unsigned long j;
    if( !MUD_getHistNumEvents( fileHandle, i, &j ) )
      throw ECommandError( command+wxT(" could not get histNumEvents") );
    data.push_back(static_cast<double>(j));
  }
  try
  {
    name = wxT("NUMEVENTS");
    NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  unsigned long scalerType, nscalers;
  if( !MUD_getScalers( fileHandle, &scalerType, &nscalers ) )
    throw ECommandError( command+wxT(" could not get scalers") );
  try
  {
    name = wxT("NUMSCALERS");
    NumericVariable::PutVariable( name, (double)nscalers, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( nscalers > 0 )
  {
    try
    {
      name = wxT("SCALERTYPE");
      NumericVariable::PutVariable( name, (double)scalerType, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<double>().swap( data );
    data.resize( 2*nscalers );
    unsigned long pCounts[2];
    for( unsigned long i=1; i<=nscalers; ++i )
    {
      if( !MUD_getScalerCounts( fileHandle, i, pCounts ) )
        throw ECommandError( command+wxT(" could not get scalerCounts") );
      data[i-1] = static_cast<double>(pCounts[0]);
      data[i-1+nscalers] = static_cast<double>(pCounts[1]);
    }
    try
    {
      name = wxT("SCALERCOUNTS");
      NumericVariable::PutVariable( name, data, nscalers, 2, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<wxString> sdata;
    for( unsigned long i=1; i<=nscalers; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getScalerLabel( fileHandle, i, a, len ) )
        throw ECommandError( command+wxT(" could not get scalerLabel") );
      if( strlen(a) == 0 )sdata.push_back(wxString(wxT(" ")));
      else                sdata.push_back(wxString(a,wxConvUTF8));
    }
    try
    {
      name = wxT("SCALERLABEL");
      TextVariable::PutVariable( name, sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  unsigned long indvartype, indvarnum;
  if( !MUD_getIndVars( fileHandle, &indvartype, &indvarnum ) )
    ExGlobals::WarningMessage( command + wxT(" could not get indVars") );
  else
  {
    MUD_getIndVars( fileHandle, &indvartype, &indvarnum );
    try
    {
      name = wxT("INDVARTYPE");
      NumericVariable::PutVariable( name, (double)indvartype, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarLow( fileHandle, i, &tmp ) )
        throw ECommandError( command+wxT(" could not get indVarLow") );
      data.push_back(tmp);
    }
    try
    {
      name = wxT("INDVARLOW");
      NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarHigh( fileHandle, i, &tmp ) )
        throw ECommandError( command+wxT(" could not get indVarHigh") );
      data.push_back(tmp);
    }
    try
    {
      name = wxT("INDVARHIGH");
      NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarMean( fileHandle, i, &tmp ) )
        throw ECommandError( command+wxT(" could not get indVarMean") );
      data.push_back(tmp);
    }
    try
    {
      name = wxT("INDVARMEAN");
      NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarStddev( fileHandle, i, &tmp ) )
        throw ECommandError( command+wxT(" could not get indVarStddev") );
      data.push_back(tmp);
    }
    try
    {
      name = wxT("INDVARSTDDEV");
      NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::vector<double>().swap( data );
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      double tmp;
      if( !MUD_getIndVarSkewness( fileHandle, i, &tmp ) )
        throw ECommandError( command+wxT(" could not get indVarSkewness") );
      data.push_back(tmp);
    }
    try
    {
      name = wxT("INDVARSKEWNESS");
      NumericVariable::PutVariable( name, data, 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    sdata.clear();
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getIndVarName( fileHandle, i, a, len ) )
        throw ECommandError( command+wxT(" could not get indVarName") );
      sdata.push_back(wxString(a,wxConvUTF8));
    }
    try
    {
      name = wxT("INDVARNAME");
      TextVariable::PutVariable( name, sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    sdata.clear();
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getIndVarDescription( fileHandle, i, a, len ) )
        throw ECommandError( command+wxT(" could not get indVarDescription") );
      sdata.push_back(wxString(a,wxConvUTF8));
    }
    try
    {
      name = wxT("INDVARDESCRIPTION");
      TextVariable::PutVariable( name, sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    sdata.clear();
    for( unsigned long i=1; i<=indvarnum; ++i )
    {
      char a[MAXSTRINGLENGTH];
      if( !MUD_getIndVarUnits( fileHandle, i, a, len ) )
        throw ECommandError( command+wxT(" could not get indVarUnits") );
      sdata.push_back(wxString(a,wxConvUTF8));
    }
    try
    {
      name = wxT("INDVARUNITS");
      TextVariable::PutVariable( name, sdata, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
#endif
}

// end of file
