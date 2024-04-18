/*
Copyright (C) 2021 Edward Sternin

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
#ifndef GRA_SPECIALCHARACTERS
#define GRA_SPECIALCHARACTERS

#include <cstdint>	// gcc v.13+ requires this for uint32_t

struct GRA_specialCharacter
  {
    char const *ename;    // special character eXtrema name
    char const *pname;    // special character PostScript name
    uint32_t const ucode; // Unicode character code
    wxChar const cid;     // remapped to this Character ID
  };

#endif
