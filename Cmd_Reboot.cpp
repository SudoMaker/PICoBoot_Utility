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

int Command_Reboot(const std::vector<std::string>& args) {
	uint8_t mode = 0;

	if (args.size() > 1) {
		if (args[1] == "bootloader") {
			mode = 1;
		}
	}

	if (global_device_path.empty()) {
		auto paths = ScanDevice_GetValidDevices();
		if (paths.size() > 1) {
			puts("More than one devices connected. Please specify one using the `-d' option.");
			return 1;
		} else {
			global_device_path = paths[0];
		}
	}

	Serial tty;
	tty.open(global_device_path);

#ifdef __CYGWIN__
	try {
#endif
	tty.make_raw();
#ifdef __CYGWIN__
	} catch (...) {

	}
#endif

	serial_chat(tty, FlasherCommand_Reset, &mode);

	return 0;
}