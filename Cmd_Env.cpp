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

std::vector<std::string> env_list = {
	"watchdog_config",
	"watchdog_max_fails",
	"allow_read",
	"allow_write",
	"app_name",
	"app_version",
	"serial",

	"reboot_count",
	"watchdog_failed_count",
	"boot_reason",
	"reboot_target",
};

enum {
	EnvType_u8, EnvType_s8,
	EnvType_u16, EnvType_s16,
	EnvType_u32, EnvType_s32,
	EnvType_u64, EnvType_s64,

	EnvType_str, EnvType_bin,
};

std::unordered_map<std::string, std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>> env_table = {
	// Category, Type, Offset, Size
	{"watchdog_config", {0, EnvType_u8, 1, 1}},
	{"watchdog_max_fails", {0, EnvType_u8, 2, 1}},
	{"allow_read", {0, EnvType_u32, 4, 4}},
	{"allow_write", {0, EnvType_u32, 8, 4}},
	{"app_name", {0, EnvType_str, 12, 16}},
	{"app_version", {0, EnvType_str, 28, 8}},
	{"serial", {0, EnvType_str, 36, 16}},

	{"reboot_count", {1, EnvType_u32, 0, 4}},
	{"watchdog_failed_count", {1, EnvType_u32, 4, 4}},
	{"boot_reason", {1, EnvType_u8, 8, 1}},
	{"reboot_target", {1, EnvType_u8, 9, 1}},
};

static void print_val(uint8_t type, uint8_t *target, uint8_t len) {
	switch (type) {
		case EnvType_u8: {
			uint8_t val = *((uint8_t *) target);
			printf("%" PRIu8 " (0x%02" PRIx8 ")\n", val, val);
		}
			break;
		case EnvType_s8: {
			int8_t val = *((int8_t *) target);
			printf("%" PRId8 " (0x%02" PRIx8 ")\n", val, val);
		}
			break;
		case EnvType_u16: {
			uint16_t val = *((uint16_t *) target);
			printf("%" PRIu16 " (0x%04" PRIx16 ")\n", val, val);
		}
			break;
		case EnvType_s16: {
			int16_t val = *((int16_t *) target);
			printf("%" PRId16 " (0x%04" PRIx16 ")\n", val, val);
		}
			break;
		case EnvType_u32: {
			uint32_t val = *((uint32_t *) target);
			printf("%" PRIu32 " (0x%08" PRIx32 ")\n", val, val);
		}
			break;
		case EnvType_s32: {
			int32_t val = *((int32_t *) target);
			printf("%" PRId32 " (0x%08" PRIx32 ")\n", val, val);
		}
			break;
		case EnvType_u64: {
			uint64_t val = *((uint64_t *) target);
			printf("%" PRIu64 " (0x%016" PRIx64 ")\n", val, val);
		}
			break;
		case EnvType_s64: {
			int64_t val = *((int64_t *) target);
			printf("%" PRId64 " (0x%016" PRIx64 ")\n", val, val);
		}
			break;
		case EnvType_str: {
			printf("%.*s (len = %u)\n", len, target, len);
		}
			break;
		case EnvType_bin: {
			for (size_t i = 0; i < len; i++) {
				printf("%02x ", target[i]);
			}
			printf("(len = %u)\n", len);
		}
			break;

	}
}

int write_env(Serial &tty, uint8_t cat, uint8_t type, uint8_t offset, uint8_t len, const std::string& user_input) {
	uint8_t buf[len];
	bool hex = (user_input.find("0x") == 0);
	int base = hex ? 16 : 10;

	switch (type) {
		case EnvType_u8:
			*((uint8_t *) buf) = strtoul(user_input.c_str(), nullptr, base);
			break;
		case EnvType_s8:
			*((int8_t *) buf) = strtol(user_input.c_str(), nullptr, base);
			break;
		case EnvType_u16:
			*((uint16_t *) buf) = strtoul(user_input.c_str(), nullptr, base);
			break;
		case EnvType_s16:
			*((int16_t *) buf) = strtol(user_input.c_str(), nullptr, base);
			break;
		case EnvType_u32:
			*((uint32_t *) buf) = strtoul(user_input.c_str(), nullptr, base);
			break;
		case EnvType_s32:
			*((int32_t *) buf) = strtol(user_input.c_str(), nullptr, base);
			break;
		case EnvType_u64:
			*((uint64_t *) buf) = strtoul(user_input.c_str(), nullptr, base);
			break;
		case EnvType_s64:
			*((int64_t *) buf) = strtol(user_input.c_str(), nullptr, base);
			break;
		case EnvType_str:
			strncpy(reinterpret_cast<char *>(buf), user_input.c_str(), len);
			break;
		case EnvType_bin:
			puts("Unsupported at this moment.");
			abort();
			break;
	}


	for (size_t i=0; i<len; i++) {
		uint8_t pkt[4] = {cat, static_cast<uint8_t>(offset+i), buf[i], 0};
		pkt[3] = crc8(pkt, 3);

		auto rcv = serial_chat(tty, FlasherCommand_EnvironmentWrite, pkt);
		uint8_t rc = *((uint8_t *)rcv.data());

		if (rc != FlasherResult_OK) {
			printf("error: failed to write environment variable: %s\n", FlasherResult_strerror(rc));
			return 2;
		}
	}

	return 0;
}

int Command_Env(const std::vector<std::string>& args) {
	if (args.size() < 2) {
		puts("usage: env <show|get KEY|set KEY VALUE|save [static|runtime]>>");
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

	if (args[1] == "show") {
		uint8_t cat = 0;
		auto env_block0 = serial_chat(tty, FlasherCommand_EnvironmentRead, &cat);
		cat = 1;
		auto env_block1 = serial_chat(tty, FlasherCommand_EnvironmentRead, &cat);

		for (auto &it : env_list) {
			auto &envdef = env_table[it];
			uint8_t *target;

			const char *category;

			if (std::get<0>(envdef) == 0) {
				target = env_block0.data();
				category = "static";
			} else {
				target = env_block1.data();
				category = "runtime";
			}

			printf("%s.%s: ", category, it.c_str());

			print_val(std::get<1>(envdef), target + std::get<2>(envdef), std::get<3>(envdef));
		}
	} else if (args[1] == "save") {
		uint8_t arg = 0x3;

		if (args.size() > 2) {
			if (args[2] == "static") {
				arg = 0x1;
			} else if (args[2] == "runtime") {
				arg = 0x2;
			}
		}

		auto rcv = serial_chat(tty, FlasherCommand_EnvironmentSave, &arg);
		uint8_t rc = *((uint8_t *)rcv.data());

		if (rc != FlasherResult_OK) {
			printf("error: failed to save environment: %s\n", FlasherResult_strerror(rc));
		}
		return 2;
	} else if (args[1] == "get") {
		if (args.size() != 3) {
			puts("error: please specify a name");
			return 2;
		}

		auto it = env_table.find(args[2]);

		if (it != env_table.end()) {
			auto &envdef = it->second;
			uint8_t *target;

			uint8_t cat = 0;
			auto env_block0 = serial_chat(tty, FlasherCommand_EnvironmentRead, &cat);
			cat = 1;
			auto env_block1 = serial_chat(tty, FlasherCommand_EnvironmentRead, &cat);

			if (std::get<0>(envdef) == 0) {
				target = env_block0.data();
			} else {
				target = env_block1.data();
			}

			print_val(std::get<1>(envdef), target + std::get<2>(envdef), std::get<3>(envdef));
		} else {
			puts("error: not a valid environment name (please don't write prefixes like `static.' or `runtime.' here)");
			return 2;
		}
	} else if (args[1] == "set") {
		if (args.size() != 4) {
			puts("error: please specify key and value");
			return 2;
		}

		auto it = env_table.find(args[2]);

		if (it != env_table.end()) {
			auto &envdef = it->second;
			return write_env(tty, std::get<0>(envdef), std::get<1>(envdef), std::get<2>(envdef), std::get<3>(envdef), args[3]);
		} else {
			puts("error: not a valid environment name (please don't write prefixes like `static.' or `runtime.' here)");
			return 2;
		}
	}

	return 0;
}