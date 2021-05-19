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

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <functional>
#include <filesystem>

#include <cxxopts.hpp>
#include <IODash.hpp>
#include <iHexPP.hpp>

using namespace SudoMaker;
using namespace IODash;

enum PicoBootCommand {
	FlasherCommand_Reset = 1,

	FlasherCommand_GetBootloaderVersion,
	FlasherCommand_GetBoardName,
	FlasherCommand_GetBoardManufacturer,
	FlasherCommand_GetChipName,
	FlasherCommand_GetChipManufacturer,

	FlasherCommand_FlashSetAddr,
	FlasherCommand_FlashSetLength,
	FlasherCommand_FlashErase,
	FlasherCommand_FlashRead,
	FlasherCommand_FlashWrite4,
	FlasherCommand_FlashWrite8,
	FlasherCommand_FlashWrite16,

	FlasherCommand_EnvironmentRead,
	FlasherCommand_EnvironmentWrite,
	FlasherCommand_EnvironmentSave,
};

enum PicoBootFlashEraseMode {
	FlashEraseMode_All = 0,
	FlashEraseMode_App = 1,
};

enum PicoBootResults {
	FlasherResult_OK = 0,
	FlasherResult_CRC_Error = 1,
	FlasherResult_Verify_Error = 2,
	FlasherResult_EPERM = 3,
	FlasherResult_ERANGE = 4,
};

enum PicoBootBootReason {
	BootReason_POR = 0,
	BootReason_RESET = 1,
	BootReason_WDT = 2,
};

enum PicoBootRebootTarget {
	RebootTarget_Default = 0,
	RebootTarget_Bootloader = 1,
};

extern std::string global_device_path;

extern const int8_t flasher_cmd_arg_length_table[];
extern const int8_t flasher_cmd_return_length_table[];

extern uint8_t crc8(const uint8_t *data, size_t len);
extern std::vector<uint8_t> serial_chat(Serial& tty, uint8_t cmd, void *arg, time_t timeout_ms = 800);

extern std::vector<std::string> ScanDevice_GetValidDevices();

extern int Command_Info(const std::vector<std::string>& args);
extern int Command_Flash(const std::vector<std::string>& args);
extern int Command_Devices(const std::vector<std::string>& args);
extern int Command_Reboot(const std::vector<std::string>& args);
extern int Command_Env(const std::vector<std::string>& args);
extern int Command_Erase(const std::vector<std::string>& args);

extern const char *FlasherResult_strerror(uint8_t rc);