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
 * material shipped with this repository. The bundled StormByte Base tree
 * under thirdparty/ remains under its own license.
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
#include <StormByte/config/binary/writer.hxx>
#include <StormByte/config/config.hxx>
#include <StormByte/config/parser/parser.hxx>
#include <StormByte/safe/string.hxx>

#include <string>
#include <string_view>
#include <vector>

using namespace StormByte::Config;

Config::Config(): m_on_existing_action(StormByte::Config::OnExistingAction::ThrowException) {}

Config::Config(const Config& config) = default;

Config::Config(Config&& config) noexcept = default;

Config& Config::operator=(const Config& config) = default;

Config& Config::operator=(Config&& config) noexcept = default;

Config::~Config() noexcept = default;

Config& Config::operator<<(const Config& source) {
	for (const auto& item: source.Items())
		Add(*item->Clone());
	return *this;
}

void Config::operator<<(const StormByte::Safe::String& str) {
	auto res = Parser::Parse(std::string(static_cast<std::string_view>(str)), m_root, m_on_existing_action, m_before_read_hooks, m_after_read_hooks, m_on_parse_failure_hook);
	if (!res)
		throw *res.error();
}

Config& Config::operator>>(Config& dest) const {
	dest << *this;
	return dest;
}

StormByte::Safe::String Config::Text() const {
	std::string serialized;
	for (const auto& item : Items()) {
		serialized += static_cast<std::string>(item->Serialize(0));
		serialized += '\n';
	}
	return StormByte::Safe::String(std::string_view(serialized));
}


StormByte::BinaryData Config::Binary() const {
	return StormByte::BinaryData(Binary::Writer(*this).Serialize());
}

ExpectedConfig Config::Load(const StormByte::Safe::String& text) {
	Config config;
	try {
		config << text;
	} catch (const StormByte::Exception& error) {
		return StormByte::Unexpected(error);
	}
	return config;
}

ExpectedConfig Config::Load(const StormByte::BinaryData& data) {
	auto result = Binary::Reader(data.span()).Deserialize();
	if (!result)
		return StormByte::Unexpected(result.error());
	return std::move(result.value());
}

void Config::OnExistingAction(const StormByte::Config::OnExistingAction& on_existing) {
	m_on_existing_action = on_existing;
	m_root.SetOnExistingAction(on_existing);
}

void Config::OnParseFailure(FailureHook hook) {
	if (hook.HasValue())
		m_on_parse_failure_hook = StormByte::Safe::Heap::MakeShared<FailureHook>(std::move(hook));
	else
		m_on_parse_failure_hook.reset();
}

void Config::AddHookBeforeRead(ReadHook hook) {
	if (hook.HasValue())
		m_before_read_hooks.push_back(StormByte::Safe::Heap::MakeShared<ReadHook>(std::move(hook)));
}

void Config::AddHookAfterRead(ReadHook hook) {
	if (hook.HasValue())
		m_after_read_hooks.push_back(StormByte::Safe::Heap::MakeShared<ReadHook>(std::move(hook)));
}
