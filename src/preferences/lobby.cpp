/*
	Copyright (C) 2009 - 2023
	by Tomasz Sniatowski <kailoran@gmail.com>
	Part of the Battle for Wesnoth Project https://www.wesnoth.org/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY.

	See the COPYING file for more details.
*/

#include "preferences/lobby.hpp"
#include "preferences/game.hpp"

namespace preferences
{
bool whisper_friends_only()
{
	return preferences::get("lobby_whisper_friends_only", false);
}

void set_whisper_friends_only(bool v)
{
	preferences::set("lobby_whisper_friends_only", v);
}

bool auto_open_whisper_windows()
{
	return preferences::get("lobby_auto_open_whisper_windows", true);
}

bool fi_invert()
{
	return preferences::get("fi_invert", false);
}

void set_fi_invert(bool value)
{
	preferences::set("fi_invert", value);
}

bool fi_vacant_slots()
{
	return preferences::get("fi_vacant_slots", false);
}

void set_fi_vacant_slots(bool value)
{
	preferences::set("fi_vacant_slots", value);
}

bool fi_friends_in_game()
{
	return preferences::get("fi_friends_in_game", false);
}

void set_fi_friends_in_game(bool value)
{
	preferences::set("fi_friends_in_game", value);
}

bool fi_blocked_in_game()
{
	return preferences::get("fi_blocked_in_game", false);
}

void set_fi_blocked_in_game(bool value)
{
	preferences::set("fi_blocked_in_game", value);
}

} // end namespace preferences
