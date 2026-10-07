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

#include <StormByte/config/item/base.hxx>
#include <StormByte/safe/string.hxx>

#include <string>
#include <string_view>

using namespace StormByte::Config::Item;

Base::Base() noexcept = default;

Base::Base(const StormByte::Safe::String& name): m_name(name) {}

Base::Base(const Base& base) noexcept = default;

Base::Base(Base&& base) noexcept = default;

Base& Base::operator=(const Base& base) noexcept = default;

Base& Base::operator=(Base&& base) noexcept = default;

Base::~Base() noexcept = default;

bool Base::operator==(const Base& base) const noexcept {
	if (this->Type() != base.Type())
		return false;
	if (m_name != base.m_name)
		return false;
	return Equals(base);
}

bool Base::operator!=(const Base& base) const noexcept {
	return !(*this == base);
}

bool Base::IsNameValid(std::string_view name) noexcept {
	if (name.empty())
		return false;
	const auto head = static_cast<unsigned char>(name.front());
	if ((head < 'A' || head > 'Z') && (head < 'a' || head > 'z'))
		return false;
	for (const unsigned char c : name.substr(1)) {
		const bool ok = (c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z')
			|| (c >= '0' && c <= '9')
			|| c == '_';
		if (!ok)
			return false;
	}
	return true;
}

StormByte::Safe::String Base::Serialize(const int& indent_level) const {
	std::string serialized(static_cast<std::size_t>(indent_level > 0 ? indent_level : 0), '\t');
	if (!m_name.empty()) {
		serialized += static_cast<std::string>(m_name);
		serialized += " = ";
	}
	return StormByte::Safe::String(std::string_view(serialized));
}
