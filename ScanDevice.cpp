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

std::vector<std::string> ScanDevice_GetAllTTYs() {
	std::vector<std::string> ret;

	for (const auto &dirEntry : std::filesystem::directory_iterator("/dev/")) {
		auto p = dirEntry.path().string();
#if defined(__linux__)
		const char keyword[] = "/ttyACM";
#elif defined(__APPLE__)
		const char keyword[] = "/cu.usbmodem";
#elif defined(__CYGWIN__)
		const char keyword[] = "/ttyS";
#elif defined(__FreeBSD__)
		const char keyword[] = "/cu.";
#endif

		if (p.find(keyword) != std::string::npos) {
			ret.emplace_back(p);
		}
	}

	return ret;
}

std::vector<std::string> ScanDevice_GetValidDevices() {
	std::vector<std::string> ret;
	std::vector<std::string> last_scan;

	int tries = 0;

	do {
		auto scan = ScanDevice_GetAllTTYs();

		if (scan.size() != last_scan.size()) {
			for (const auto &it : scan) {
//				printf("Trying: %s ...\n", it.c_str());

				Serial tty;
				tty.open(it);

				try {
					tty.make_raw();
				} catch (...) {
#ifndef __CYGWIN__
					continue;
#endif
				}

				try {
					serial_chat(tty, FlasherCommand_GetBootloaderVersion, nullptr, 300);
					ret.push_back(it);
				} catch (...) {

				}
			}

			last_scan = std::move(scan);
		}

		if (ret.empty()) {
			if (tries == 0)
				puts("< waiting for device >");
			usleep(1 * 1000);
		}

		tries++;
	} while (ret.empty());

	return ret;
}