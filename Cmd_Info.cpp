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

int Command_Info(const std::vector<std::string>& args) {
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

	auto info_bl_ver = serial_chat(tty, FlasherCommand_GetBootloaderVersion, nullptr);
	info_bl_ver.back() = 0;

	auto info_board = serial_chat(tty, FlasherCommand_GetBoardName, nullptr);
	info_board.back() = 0;

	auto info_board_mf = serial_chat(tty, FlasherCommand_GetBoardManufacturer, nullptr);
	info_board_mf.back() = 0;

	auto info_chip = serial_chat(tty, FlasherCommand_GetChipName, nullptr);
	info_chip.back() = 0;

	auto info_chip_mf = serial_chat(tty, FlasherCommand_GetChipManufacturer, nullptr);
	info_chip_mf.back() = 0;

	printf("Device: %s\n", global_device_path.c_str());
	puts("");
	printf("Bootloader Version: %s\n", info_bl_ver.data());
	printf("Board name: %s\n", info_board.data());
	printf("Board Manufacturer: %s\n", info_board_mf.data());
	printf("Chip name: %s\n", info_chip.data());
	printf("Chip Manufacturer: %s\n", info_chip_mf.data());

	uint8_t cat = 0;
	auto env_block0 = serial_chat(tty, FlasherCommand_EnvironmentRead, &cat);

	if (env_block0.size() >= 52) {
		printf("Serial: ");
		fflush(stdout);
		write(STDOUT_FILENO, env_block0.data()+36, 16);
		puts("");
	}

	return 0;
}