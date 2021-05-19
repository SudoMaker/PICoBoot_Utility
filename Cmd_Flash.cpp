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

int Command_Flash(const std::vector<std::string>& args) {
	if (args.size() != 2) {
		puts("error: no file specified");
		return 2;
	}

	auto &filepath = args[1];

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
	tty.speed() = 2000000;
#ifdef __CYGWIN__
	} catch (...) {

	}
#endif

	std::ifstream infile(filepath);
	infile.seekg(0, std::ios_base::end);
	size_t infile_size = infile.tellg();
	infile.seekg(0, std::ios_base::beg);
	size_t processed_size = 0;

	std::string line;

	iHexPP::Decoder dec;

	uint32_t last_addr = 0;
	uint8_t cnt = 0;

	dec.read_callback = [&](uint8_t record_type, uint32_t addr, const uint8_t *data, size_t len){
		if (record_type == iHexPP::IHEX_DATA_RECORD) {
			last_addr = addr;

			auto rc_addr = serial_chat(tty, FlasherCommand_FlashSetAddr, &addr);

			if (*((uint32_t *)rc_addr.data()) != addr) {
				throw std::logic_error("address verification failed!");
			}

			if (cnt % 16 == 0) {
				printf("\033[2K\rFlashing ... [0x%08x+%zu] %zu/%zu %.2f%%", addr, len, processed_size, infile_size, ((float) processed_size / infile_size) * 100);
				fflush(stdout);
			}

			uint8_t buf[32];
			uint8_t rc;

			if (len == 8) {
				memcpy(buf, data, 8);
				buf[8] = crc8(buf, 8);
				auto rcv = serial_chat(tty, FlasherCommand_FlashWrite8, buf);
				rc = *((uint8_t *)rcv.data());
			} else if (len == 16) {
				memcpy(buf, data, 16);
				buf[16] = crc8(buf, 16);
				auto rcv = serial_chat(tty, FlasherCommand_FlashWrite16, buf);
				rc = *((uint8_t *)rcv.data());
			} else {
				for (size_t i=0; i<len; i+=4) {
					memcpy(buf, data + i, 4);
					buf[4] = crc8(buf, 4);
					auto rcv = serial_chat(tty, FlasherCommand_FlashWrite4, buf);
					rc = *((uint8_t *)rcv.data());

					if (rc != FlasherResult_OK) {
						printf("error: failed to write flash: %s\n", FlasherResult_strerror(rc));
						abort();
					}
				}
			}

			if (rc != FlasherResult_OK) {
				printf("error: failed to write flash: %s\n", FlasherResult_strerror(rc));
				abort();
			}



			cnt++;
		}
	};

	puts("");

	while (std::getline(infile, line)) {
		dec.read(line);
		processed_size += line.size();
	}

	printf("\033[2K\rFlashing ... [0x%08x] %zu/%zu %.2f%%\n", last_addr, infile_size, infile_size, (float)100);

	return 0;
}