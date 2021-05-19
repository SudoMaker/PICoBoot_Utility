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

std::vector<uint8_t> serial_chat(Serial& tty, uint8_t cmd, void *arg, time_t timeout_ms) {
	size_t ret_len = flasher_cmd_return_length_table[cmd];
	std::vector<uint8_t> ret(ret_len);
	size_t ret_pos = 0;

	tty.putc(cmd);

	if (tty.write_all(arg, flasher_cmd_arg_length_table[cmd]) == -1) {
		throw std::system_error(errno, std::system_category(), "failed to write to serial port");
	}

	if (ret_len) {
		tty.set_nonblocking();

		EventLoop<> el;
		el.set_idle_time(timeout_ms);
		el.add(tty, EventType::In);

		el.on_events([&](auto &event_loop, File &file, EventType event_type, auto &) {
			auto rc = file.read(ret.data() + ret_pos, ret_len - ret_pos);

			if (rc > 0) {
				ret_pos += rc;
			} else {
				throw std::system_error(errno, std::system_category(), "failed to read from serial port");
			}
		});

		el.on_idle([&](auto &event_loop) {
			errno = ETIMEDOUT;
			throw std::system_error(errno, std::system_category(), "timeout reading from serial port");
		});

		while (ret_pos != ret_len) {
			el.run_once();
		}

		tty.set_nonblocking(false);
	}

	return ret;
}
