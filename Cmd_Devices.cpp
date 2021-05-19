/*
    This file is part of PICoBoot Utility.

    Copyright (C) 2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "PICoBoot_Utility.hpp"

int Command_Devices(const std::vector<std::string>& args) {

	auto paths = ScanDevice_GetValidDevices();

	printf("%20.20s %20.20s\n", "path", "bl version");

	for (auto &it : paths) {
		Serial tty;
		tty.open(it);

#ifdef __CYGWIN__
		try {
#endif
		tty.make_raw();
		tty.speed() = 2000000;
#ifdef __CYGWIN__
		} catch (...) {

	}
#endif

		auto info_bl_ver = serial_chat(tty, FlasherCommand_GetBootloaderVersion, nullptr);
		*info_bl_ver.end() = 0;

		printf("%20.20s %20.20s\n", it.c_str(), info_bl_ver.data());
	}

	return 0;
}