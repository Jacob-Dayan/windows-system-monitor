/*
	Copyright (C) 2026 Jacob Dayan

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received me a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CONSTS_HPP
#define CONSTS_HPP

namespace App {
    inline constexpr const char Name[]    = "Windows System Monitor";
    inline constexpr const char Version[] = "0.1.2";
}

namespace Ansi {
    inline constexpr const char Reset[]        = "\033[0m";
    inline constexpr const char Bold[]         = "\033[1m";
    inline constexpr const char Dim[]          = "\033[2m";

    inline constexpr const char Red[]          = "\033[31m";
    inline constexpr const char Green[]        = "\033[32m";
    inline constexpr const char Yellow[]       = "\033[33m";
    inline constexpr const char Blue[]         = "\033[34m";
    inline constexpr const char Magenta[]      = "\033[35m";
    inline constexpr const char Cyan[]         = "\033[36m";
    inline constexpr const char White[]        = "\033[37m";
    inline constexpr const char Gray[]         = "\033[90m";

    inline constexpr const char BoldRed[]      = "\033[1;31m";
    inline constexpr const char BoldGreen[]    = "\033[1;32m";
    inline constexpr const char BoldYellow[]   = "\033[1;33m";
    inline constexpr const char BoldBlue[]     = "\033[1;34m";
    inline constexpr const char BoldMagenta[]  = "\033[1;35m";
    inline constexpr const char BoldCyan[]     = "\033[1;36m";
    inline constexpr const char BoldWhite[]    = "\033[1;37m";

    inline constexpr const char HeaderBanner[] = "\033[1;37;44m";
    inline constexpr const char TabActive[]    = "\033[1;37;42m";
    inline constexpr const char HotkeysBar[]   = "\033[1;37;40m";

    inline constexpr const char CursorHome[]   = "\033[H";
    inline constexpr const char ClearScreen[]  = "\033[2J";
    inline constexpr const char ClearLine[]    = "\033[K";
    inline constexpr const char ClearAll[]     = "\033[2J\033[H";
}

#endif // CONSTS_HPP
