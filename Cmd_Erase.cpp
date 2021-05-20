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

int Command_Erase(const std::vector<std::string>& args) {
	uint8_t mode = 0;

	if (args.size() != 2) {
		puts("error: erase mode not specified");
		return 2;
	}

	if (args[1] == "all") {
		mode = FlashEraseMode_All;
	} else if (args[1] == "app") {
		mode = FlashEraseMode_App;
	} else {
		puts("error: invalid erase mode!");
		return 2;
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

	try {
		auto rcv = serial_chat(tty, FlasherCommand_FlashErase, &mode, 20000);

		uint8_t rc = *((uint8_t *) rcv.data());

		if (rc != FlasherResult_OK) {
			printf("error: failed to erase: %s\n", FlasherResult_strerror(rc));
			return 2;
		}
	} catch (...) {

	}

	return 0;
}