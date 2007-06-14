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
#include <iostream>

#include "CommandTable.h"
#include "CMD_alias.h"
#include "CMD_bestfit.h"
#include "CMD_bin2d.h"
#include "CMD_bin.h"
#include "CMD_clear.h"
#include "CMD_contour.h"
#include "CMD_dealias.h"
#include "CMD_defaults.h"
#include "CMD_define.h"
#include "CMD_density.h"
#include "CMD_destroy.h"
#include "CMD_display.h"
#include "CMD_do.h"
#include "CMD_ellipse.h"
#include "CMD_enddo.h"
#include "CMD_endif.h"
#include "CMD_evaluateExpression.h"
#include "CMD_execute.h"
#include "CMD_figure.h"
#include "CMD_filter.h"
#include "CMD_fit.h"
#include "CMD_form.h"
#include "CMD_generate.h"
#include "CMD_get.h"
#include "CMD_goto.h"
#include "CMD_graph.h"
#include "CMD_grid.h"
#include "CMD_hardcopy.h"
#include "CMD_help.h"
#include "CMD_if.h"
#include "CMD_inquire.h"
#include "CMD_list.h"
#include "CMD_matrix.h"
#include "CMD_minuit.h"
#include "CMD_page.h"
#include "CMD_pause.h"
#include "CMD_polygon.h"
#include "CMD_quit.h"
#include "CMD_read.h"
#include "CMD_rebin.h"
#include "CMD_replot.h"
#include "CMD_restore.h"
#include "CMD_return.h"
#include "CMD_save.h"
#include "CMD_scalar.h"
#include "CMD_scales.h"
#include "CMD_set.h"
#include "CMD_show.h"
#include "CMD_sort.h"
#include "CMD_stack.h"
#include "CMD_statistics.h"
#include "CMD_text.h"
#include "CMD_vector.h"
#include "CMD_window.h"
#include "CMD_world.h"
#include "CMD_write.h"
#include "CMD_zerolines.h"

CommandTable *CommandTable::commandTable_ = 0;

CommandTable::CommandTable()
{ cMap_ = new commandMap; }

CommandTable::CommandTable( CommandTable const &rhs )
{
  delete cMap_;
  cMap_ = new commandMap(*(rhs.cMap_));
}

CommandTable::~CommandTable()
{ ClearTable(); }

CommandTable *CommandTable::GetTable()
{
  // CommandTable is a "singleton", and can get its pointer by this function.
  // The first time this function is called, the CommandTable object is created.
  //
  if( !CommandTable::commandTable_ )CommandTable::commandTable_ = new CommandTable();
  return CommandTable::commandTable_;
}

void CommandTable::CreateCommands()
{
  CMD_alias::Instance();
  CMD_bestfit::Instance();
  CMD_bin2d::Instance();
  CMD_bin::Instance();
  CMD_clear::Instance();
  CMD_contour::Instance();
  CMD_dealias::Instance();
  CMD_defaults::Instance();
  CMD_define::Instance();
  CMD_density::Instance();
  CMD_destroy::Instance();
  CMD_display::Instance();
  CMD_do::Instance();
  CMD_ellipse::Instance();
  CMD_enddo::Instance();
  CMD_endif::Instance();
  CMD_evaluateExpression::Instance();
  CMD_execute::Instance();
  CMD_figure::Instance();
  CMD_filter::Instance();
  CMD_fit::Instance();
  CMD_form::Instance();
  CMD_generate::Instance();
  CMD_get::Instance();
  CMD_goto::Instance();
  CMD_graph::Instance();
  CMD_grid::Instance();
  CMD_hardcopy::Instance();
  CMD_help::Instance();
  CMD_if::Instance();
  CMD_inquire::Instance();
  CMD_list::Instance();
  CMD_matrix::Instance();
  CMD_minuit::Instance();
  CMD_page::Instance();
  CMD_pause::Instance();
  CMD_polygon::Instance();
  CMD_quit::Instance();
  CMD_read::Instance();
  CMD_rebin::Instance();
  CMD_replot::Instance();
  CMD_restore::Instance();
  CMD_return::Instance();
  CMD_save::Instance();
  CMD_scalar::Instance();
  CMD_scales::Instance();
  CMD_set::Instance();
  CMD_show::Instance();
  CMD_sort::Instance();
  CMD_stack::Instance();
  CMD_statistics::Instance();
  CMD_text::Instance();
  CMD_vector::Instance();
  CMD_window::Instance();
  CMD_world::Instance();
  CMD_write::Instance();
  CMD_zerolines::Instance();
}

void CommandTable::ClearTable()
{
  commandMap::const_iterator cEnd = cMap_->end();
  for( commandMap::const_iterator i=cMap_->begin(); i!=cEnd; ++i )delete (*i).second;
  delete cMap_;
}

Command *CommandTable::Get( wxString const &name ) const
{
  Command *c = 0;
  commandMap::const_iterator cEnd = cMap_->end();
  for( commandMap::const_iterator i=cMap_->begin(); i!=cEnd; ++i )
  {
    if( (*i).second->IsCommand(name) )
    {
      c = (*i).second;
      break;
    }
  }
  return c;
}

void CommandTable::AddEntry( Command *c )
{
  wxString name = c->Name();
  if( Contains( name ) )
  {
    //wxString s = wxString(wxT("The command "))+name+wxString(wxT(" is already in the table"));
    return;
  }
  cMap_->insert(entryType(name,c));
}

// end of file

