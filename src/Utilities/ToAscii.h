/*
Copyright (C) 2020 Vadim Zeitlin

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

#ifndef EXTREMA_UTILITIES_TOASCII_H
#define EXTREMA_UTILITIES_TOASCII_H

#include <wx/unichar.h>

// Replacement for the standard toascii() working with wxUniChar: returns 0 for
// non-ASCII characters or a value strictly between 0 and 128 representing a
// 7-bit ASCII character otherwise.
inline int TryConvertToAscii(wxUniChar uc)
{
    return uc.IsAscii() ? uc.GetValue() : 0;
}

#endif // EXTREMA_UTILITIES_TOASCII_H
