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

uint8_t crc8(const uint8_t *data, size_t len) {
	uint8_t crc = 0xff;
	size_t i, j;
	for (i = 0; i < len; i++) {
		crc ^= data[i];
		for (j = 0; j < 8; j++) {
			if ((crc & 0x80) != 0)
				crc = (uint8_t)((crc << 1) ^ 0x31);
			else
				crc <<= 1;
		}
	}

	return crc;
}

void ShowHelpExtra() {
	puts(
		"\n"
		" Commands:\n"
		"  devices                   List devices in bootloader\n"
		"  info                      Show device info\n"
		"  reboot [bootloader]       Reboot device\n"
		"  flash <FILE.hex>          Flash from image\n"
		"  readback <FILE.hex>       Readback to image\n"
		"  erase <app|all>           Erase device\n"
		"  env <show|get|set|save>   Manipulate bootloader environment\n"
	);
}

std::string global_device_path;

int main(int argc, char **argv) {
	cxxopts::Options options("picoboot", "picoboot - Utility for the PICoBoot bootloader.");

	std::vector<std::string> positional_args;

	options.add_options("Main")
		("h,help", "Show this help")
		("d,device", "Manually specify device path", cxxopts::value<std::string>(global_device_path)->default_value(""))
		;

	options.add_options("positional")
		("positional", "Positional parameters: The file to write/read", cxxopts::value<std::vector<std::string>>(positional_args))
		;

	options.parse_positional("positional");
	options.positional_help("COMMAND...").show_positional_help();


	try {
		auto cmd = options.parse(argc, argv);

		if (cmd.count("help")) {
			std::cout << options.help({"Main"});
			ShowHelpExtra();
			return 0;
		}


		if (positional_args.empty()) {
			std::cout << options.help({"Main"});
			ShowHelpExtra();
			return 0;
		}
	} catch (std::exception &e) {
		std::cout << "Error: " << e.what() << "\n";
		std::cout << options.help({"Main"});
		ShowHelpExtra();
		return 1;
	}

	if (geteuid()) {
		printf("Warning: You may encounter problems without root permissions\n");
	}

	std::unordered_map<std::string, std::function<int(const std::vector<std::string>&)>> command_map = {
		{"devices", [](auto &arg){return Command_Devices(arg);}},
		{"info", [](auto &arg){return Command_Info(arg);}},
		{"reboot", [](auto &arg){return Command_Reboot(arg);}},
		{"flash", [](auto &arg){return Command_Flash(arg);}},
		{"erase", [](auto &arg){return Command_Erase(arg);}},
		{"env", [](auto &arg){return Command_Env(arg);}},
	};

	auto it = command_map.find(positional_args[0]);

	if (it != command_map.end()) {
		return it->second(positional_args);
	} else {
		printf("error: no such command: %s, read the help!\n", positional_args[0].c_str());
	}

	return 2;
}
