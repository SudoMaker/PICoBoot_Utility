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

const int8_t flasher_cmd_arg_length_table[] = {
	-1,	// 0: FlasherCommand_Unknown

	1,	// void FlasherCommand_Reset(uint8_t mode)

	0,	// char[32] FlasherCommand_GetBootloaderVersion()
	0,	// char[32] FlasherCommand_GetBoardName()
	0,	// char[32] FlasherCommand_GetBoardManufacturer()
	0,	// char[32] FlasherCommand_GetChipName()
	0,	// char[32] FlasherCommand_GetChipManufacturer()

	4,	// uint32_t FlasherCommand_FlashSetAddr(uint32_t addr)
	2,	// uint16_t FlasherCommand_FlashSetLength(uint16_t len)
	1,	// Result FlasherCommand_FlashErase()
	0,	// uint8_t[] FlasherCommand_FlashRead()
	5,	// Result FlasherCommand_FlashWrite4(uint8_t[4], uint8_t crc8)
	9,	// Result FlasherCommand_FlashWrite8(uint8_t[8], uint8_t crc8)
	17,	// Result FlasherCommand_FlashWrite16(uint8_t[16], uint8_t crc8)


	1,	// char[64] FlasherCommand_EnvironmentRead(uint8_t category)
	4,	// Result FlasherCommand_EnvironmentWrite(uint8_t category, uint8_t offset, uint8_t value, uint8_t crc)
	1,	// Result FlasherCommand_EnvironmentSave(uint8_t category)
};

const int8_t flasher_cmd_return_length_table[] = {
	-1,	// 0: FlasherCommand_Unknown

	0,	// void FlasherCommand_Reset()

	32,	// char[32] FlasherCommand_GetBootloaderVersion()
	32,	// char[32] FlasherCommand_GetBoardName()
	32,	// char[32] FlasherCommand_GetBoardManufacturer()
	32,	// char[32] FlasherCommand_GetChipName()
	32,	// char[32] FlasherCommand_GetChipManufacturer()

	4,	// uint32_t FlasherCommand_FlashSetAddr(uint32_t addr)
	2,	// uint16_t FlasherCommand_FlashSetLength(uint16_t len)
	1,	// Result FlasherCommand_FlashErase()
	-2,	// uint8_t[] FlasherCommand_FlashRead()
	1,	// Result FlasherCommand_FlashWrite4(uint8_t[4])
	1,	// Result FlasherCommand_FlashWrite8(uint8_t[8])
	1,	// Result FlasherCommand_FlashWrite8(uint8_t[16])


	64,	// char[64] FlasherCommand_EnvironmentRead(uint8_t category)
	1,	// Result FlasherCommand_EnvironmentWrite(uint8_t category, uint8_t offset, uint8_t value, uint8_t crc)
	1,	// Result FlasherCommand_EnvironmentSave(uint8_t category)
};

const char *FlasherResult_strerror(uint8_t rc) {
	switch (rc) {
		case FlasherResult_EPERM:
			return "Permission denied";
		case FlasherResult_ERANGE:
			return "Requested range not satisfiable";
		case FlasherResult_Verify_Error:
			return "Verification failed";
		case FlasherResult_CRC_Error:
			return "Checksum error";
		default:
			return "OK";
	}
}