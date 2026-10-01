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

void Config::operator<<(std::istream& istream) {
	auto res = Parser::Parse(istream, m_root, m_on_existing_action, m_before_read_hooks, m_after_read_hooks, m_on_parse_failure_hook);
	if (!res)
		throw *res.error();
}

void Config::operator<<(const StormByte::Safe::String& str) {
	auto res = Parser::Parse(std::string(static_cast<std::string_view>(str)), m_root, m_on_existing_action, m_before_read_hooks, m_after_read_hooks, m_on_parse_failure_hook);
	if (!res)
		throw *res.error();
}

void Config::operator<<(const std::string& str) {
	auto res = Parser::Parse(str, m_root, m_on_existing_action, m_before_read_hooks, m_after_read_hooks, m_on_parse_failure_hook);
	if (!res)
		throw *res.error();
}

Config& StormByte::Config::operator>>(std::istream& istream, Config& config) {
	config << istream;
	return config;
}

Config& StormByte::Config::operator>>(const StormByte::Safe::String& str, Config& config) {
	config << str;
	return config;
}

Config& StormByte::Config::operator>>(const std::string& str, Config& config) {
	config << str;
	return config;
}

Config& Config::operator>>(Config& dest) const {
	dest << *this;
	return dest;
}

std::ostream& Config::operator>>(std::ostream& ostream) const {
	ostream << static_cast<std::string>(Text());
	return ostream;
}

std::string& Config::operator>>(std::string& str) const {
	str += static_cast<std::string>(Text());
	return str;
}

std::ostream& StormByte::Config::operator<<(std::ostream& ostream, const Config& config) {
	ostream << static_cast<std::string>(config.Text());
	return ostream;
}

std::string& operator<<(std::string& str, const Config& config) {
	str += static_cast<std::string>(config.Text());
	return str;
}

StormByte::Safe::String Config::Text() const {
	std::string serialized;
	for (const auto& item : Items()) {
		serialized += static_cast<std::string>(item->Serialize(0));
		serialized += '\n';
	}
	return StormByte::Safe::String(std::string_view(serialized));
}

void Config::Save(std::ostream& stream, Mode mode) const {
	if (mode == Mode::Text) {
		stream << static_cast<std::string>(Text());
		return;
	}

	const auto buffer = Binary::Writer(*this).Serialize();
	if (!buffer.empty()) {
		stream.write(
			reinterpret_cast<const char*>(buffer.data()),
			static_cast<std::streamsize>(buffer.size()));
	}
}

ExpectedConfig Config::Load(std::istream& stream, Mode mode) {
	if (mode == Mode::Text) {
		Config cfg;
		try {
			cfg << stream;
		} catch (const StormByte::Exception& e) {
			return StormByte::Unexpected(e);
		}
		return cfg;
	}

	stream.seekg(0, std::ios::end);
	const std::streamsize size = stream.tellg();
	stream.seekg(0, std::ios::beg);
	if (size < 0) {
		std::vector<std::byte> buffer;
		char chunk[4096];
		while (stream.read(chunk, sizeof(chunk)) || stream.gcount() > 0) {
			const auto n = static_cast<std::size_t>(stream.gcount());
			const auto* p = reinterpret_cast<const std::byte*>(chunk);
			buffer.insert(buffer.end(), p, p + n);
		}

		auto result = Binary::Reader(buffer).Deserialize();
		if (!result)
			return StormByte::Unexpected(result.error());
		return std::move(result.value());
	}

	std::vector<std::byte> buffer(static_cast<std::size_t>(size));
	if (size > 0 && !stream.read(reinterpret_cast<char*>(buffer.data()), size)) {
		return StormByte::Unexpected<StormByte::DeserializeError>(
			"Failed to read binary config stream");
	}

	auto result = Binary::Reader(buffer).Deserialize();
	if (!result)
		return StormByte::Unexpected(result.error());
	return std::move(result.value());
}

void Config::OnExistingAction(const StormByte::Config::OnExistingAction& on_existing) {
	m_on_existing_action = on_existing;
	m_root.SetOnExistingAction(on_existing);
}

void Config::OnParseFailure(OnFailureHook hook) {
	m_on_parse_failure_hook = MakeFailureHook(hook);
}

void Config::OnParseFailure(StormByte::Safe::Shared<FailureHook> hook) {
	m_on_parse_failure_hook = std::move(hook);
}

void Config::AddHookBeforeRead(HookFunction hook) {
	m_before_read_hooks.push_back(MakeReadHook(hook));
}

void Config::AddHookBeforeRead(StormByte::Safe::Shared<ReadHook> hook) {
	m_before_read_hooks.push_back(std::move(hook));
}

void Config::AddHookAfterRead(HookFunction hook) {
	m_after_read_hooks.push_back(MakeReadHook(hook));
}

void Config::AddHookAfterRead(StormByte::Safe::Shared<ReadHook> hook) {
	m_after_read_hooks.push_back(std::move(hook));
}
