/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Config.
 *
 * StormByte-Config original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte-Config source in this
 * repository. They do not cover other StormByte modules or any third-party
 * material shipped with this repository (including everything under
 * thirdparty/, and in particular the bundled StormByte-String tree and
 * the StormByte Base tree it vendors), which remains under its own license.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte-Config is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte-Config. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#include <StormByte/config/binary/reader.hxx>
#include <StormByte/config/binary/reader_v1.hxx>
#include <StormByte/serializable.hxx>

#include <cstring>
#include <format>

using namespace StormByte;
using namespace StormByte::Config;
using namespace StormByte::Config::Binary;

namespace {
	bool MagicMatches(BufferView data) noexcept {
		if (data.size() < Magic.size())
			return false;
		return std::memcmp(data.data(), Magic.data(), Magic.size()) == 0;
	}
}

Reader::Reader(BufferView data) noexcept: m_data(data) {}

Binary::ExpectedConfig Reader::Deserialize() const noexcept {
	try {
		if (m_data.size() < HeaderSize)
			return Unexpected<DeserializeError>("Truncated config binary header");

		if (!MagicMatches(m_data))
			return Unexpected<DeserializeError>("Not a StormByte config binary (bad magic)");

		auto version = Serializable<std::uint8_t>::Deserialize(m_data.subspan(Magic.size(), 1));
		if (!version)
			return Unexpected(version.error());

		if (version.value() > CurrentVersion) {
			return Unexpected<DeserializeError>(std::format(
				"Config binary version {} is newer than this library (max {})",
				version.value(), CurrentVersion));
		}

		if (version.value() < 1) {
			return Unexpected<DeserializeError>(std::format(
				"Unsupported config binary version {}", version.value()));
		}

		const BufferView payload = m_data.subspan(HeaderSize);

		Expected<std::pair<OnExistingAction, Item::Group>, DeserializeError> body;
		switch (version.value()) {
			case 1:
				body = ReaderV1::Read(payload);
				break;
			default:
				return Unexpected<DeserializeError>(std::format(
					"Unsupported config binary version {}", version.value()));
		}

		if (!body)
			return Unexpected(body.error());

		Config cfg;
		cfg.m_on_existing_action = body.value().first;
		cfg.m_root = std::move(body.value().second);
		cfg.m_root.SetOnExistingAction(cfg.m_on_existing_action);
		return cfg;
	} catch (const Exception& e) {
		return Unexpected<DeserializeError>(e.what());
	}
}
