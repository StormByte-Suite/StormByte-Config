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

#include <StormByte/base64.hxx>
#include <StormByte/config/exception.hxx>
#include <StormByte/config/item/value.hxx>
#include <StormByte/safe/string.hxx>

#include <format>
#include <string>
#include <string_view>
#include <utility>

using namespace StormByte::Config::Item;

namespace {
	[[noreturn]] void FailKind(const char* wanted) {
		throw StormByte::Config::Exception("Value is not {}", wanted);
	}
}

Value::Value(int value): Base(), m_store(value) {}

Value::Value(double value): Base(), m_store(value) {}

Value::Value(bool value): Base(), m_store(value) {}

Value::Value(const char* value): Base(), m_store(StormByte::Safe::String(value)) {}

Value::Value(std::string_view value): Base(), m_store(StormByte::Safe::String(value)) {}

Value::Value(StormByte::Safe::String&& value): Base(), m_store(std::move(value)) {}

Value::Value(const StormByte::Safe::Binary& value): Base(), m_store(value) {}

Value::Value(StormByte::Safe::Binary&& value): Base(), m_store(std::move(value)) {}

Value::Value(std::string_view name, int value): Base(StormByte::Safe::String(name)), m_store(value) {}

Value::Value(std::string_view name, double value): Base(StormByte::Safe::String(name)), m_store(value) {}

Value::Value(std::string_view name, bool value): Base(StormByte::Safe::String(name)), m_store(value) {}

Value::Value(std::string_view name, const char* value)
	: Base(StormByte::Safe::String(name)), m_store(StormByte::Safe::String(value)) {}

Value::Value(std::string_view name, std::string_view value)
	: Base(StormByte::Safe::String(name)), m_store(StormByte::Safe::String(value)) {}

Value::Value(std::string_view name, const StormByte::Safe::Binary& value)
	: Base(StormByte::Safe::String(name)), m_store(value) {}

Value::Value(std::string_view name, StormByte::Safe::Binary&& value)
	: Base(StormByte::Safe::String(name)), m_store(std::move(value)) {}

Value::Value(const Value& value): Base(value), m_store(value.m_store) {}

Value::Value(Value&& value) noexcept: Base(std::move(value)), m_store(std::move(value.m_store)) {}

Value::~Value() noexcept = default;

Value& Value::operator=(const Value& value) {
	if (this == &value)
		return *this;
	Base::operator=(value);
	m_store = value.m_store;
	return *this;
}

Value& Value::operator=(Value&& value) noexcept {
	if (this == &value)
		return *this;
	Base::operator=(std::move(value));
	m_store = std::move(value.m_store);
	return *this;
}

Type Value::Kind() const noexcept {
	if (StormByte::Safe::holds_alternative<int>(m_store))
		return Type::Integer;
	if (StormByte::Safe::holds_alternative<double>(m_store))
		return Type::Double;
	if (StormByte::Safe::holds_alternative<bool>(m_store))
		return Type::Bool;
	if (StormByte::Safe::holds_alternative<StormByte::Safe::String>(m_store))
		return Type::String;
	return Type::Binary;
}

Type Value::Type() const noexcept {
	return Kind();
}

bool Value::Equals(const Base& base) const {
	const auto& value = static_cast<const Value&>(base);
	return m_store == value.m_store;
}

Value& Value::operator=(int value) {
	if (!StormByte::Safe::holds_alternative<int>(m_store))
		Fail("Integer");
	StormByte::Safe::get<int>(m_store) = value;
	return *this;
}

Value& Value::operator=(double value) {
	if (!StormByte::Safe::holds_alternative<double>(m_store))
		Fail("Double");
	StormByte::Safe::get<double>(m_store) = value;
	return *this;
}

Value& Value::operator=(bool value) {
	if (!StormByte::Safe::holds_alternative<bool>(m_store))
		Fail("Bool");
	StormByte::Safe::get<bool>(m_store) = value;
	return *this;
}

Value& Value::operator=(const char* value) {
	return operator=(std::string_view(value ? value : ""));
}

Value& Value::operator=(std::string_view value) {
	if (!StormByte::Safe::holds_alternative<StormByte::Safe::String>(m_store))
		Fail("String");
	StormByte::Safe::get<StormByte::Safe::String>(m_store) = StormByte::Safe::String(value);
	return *this;
}

Value& Value::operator=(const StormByte::Safe::Binary& value) {
	if (!StormByte::Safe::holds_alternative<StormByte::Safe::Binary>(m_store))
		Fail("Binary");
	StormByte::Safe::get<StormByte::Safe::Binary>(m_store) = value;
	return *this;
}

Value::operator int&() {
	if (!StormByte::Safe::holds_alternative<int>(m_store))
		Fail("Integer");
	return StormByte::Safe::get<int>(m_store);
}

Value::operator const int&() const {
	if (!StormByte::Safe::holds_alternative<int>(m_store))
		Fail("Integer");
	return StormByte::Safe::get<int>(m_store);
}

Value::operator double() const {
	if (StormByte::Safe::holds_alternative<double>(m_store))
		return StormByte::Safe::get<double>(m_store);
	if (StormByte::Safe::holds_alternative<int>(m_store))
		return static_cast<double>(StormByte::Safe::get<int>(m_store));
	Fail("Double");
}

Value::operator double&() {
	if (!StormByte::Safe::holds_alternative<double>(m_store))
		Fail("Double");
	return StormByte::Safe::get<double>(m_store);
}

Value::operator bool&() {
	if (!StormByte::Safe::holds_alternative<bool>(m_store))
		Fail("Bool");
	return StormByte::Safe::get<bool>(m_store);
}

Value::operator const bool&() const {
	if (!StormByte::Safe::holds_alternative<bool>(m_store))
		Fail("Bool");
	return StormByte::Safe::get<bool>(m_store);
}

Value::operator StormByte::Safe::String&() {
	if (!StormByte::Safe::holds_alternative<StormByte::Safe::String>(m_store))
		Fail("String");
	return StormByte::Safe::get<StormByte::Safe::String>(m_store);
}

Value::operator const StormByte::Safe::String&() const {
	if (!StormByte::Safe::holds_alternative<StormByte::Safe::String>(m_store))
		Fail("String");
	return StormByte::Safe::get<StormByte::Safe::String>(m_store);
}

Value::operator StormByte::Safe::Binary&() {
	if (!StormByte::Safe::holds_alternative<StormByte::Safe::Binary>(m_store))
		Fail("Binary");
	return StormByte::Safe::get<StormByte::Safe::Binary>(m_store);
}

Value::operator const StormByte::Safe::Binary&() const {
	if (!StormByte::Safe::holds_alternative<StormByte::Safe::Binary>(m_store))
		Fail("Binary");
	return StormByte::Safe::get<StormByte::Safe::Binary>(m_store);
}

void Value::Fail(const char* wanted) const {
	FailKind(wanted);
}

StormByte::Safe::String Value::Serialize(const int& indent_level) const {
	std::string out;
	if (!m_name.empty())
		out += std::string(static_cast<std::size_t>(indent_level > 0 ? indent_level : 0), '\t') + static_cast<std::string>(m_name) + " = ";
	else
		out += std::string(static_cast<std::size_t>(indent_level > 0 ? indent_level : 0), '\t');
	if (StormByte::Safe::holds_alternative<int>(m_store))
		out += std::format("{}", StormByte::Safe::get<int>(m_store));
	else if (StormByte::Safe::holds_alternative<double>(m_store)) {
		out += std::format("{}", StormByte::Safe::get<double>(m_store));
		if (out.find('.') == std::string::npos && out.find('e') == std::string::npos && out.find('E') == std::string::npos)
			out += ".0";
	} else if (StormByte::Safe::holds_alternative<bool>(m_store))
		out += StormByte::Safe::get<bool>(m_store) ? "true" : "false";
	else if (StormByte::Safe::holds_alternative<StormByte::Safe::String>(m_store))
		out += "\"" + static_cast<std::string>(StormByte::Safe::get<StormByte::Safe::String>(m_store)) + "\"";
	else
		out += "b\"" + static_cast<std::string>(StormByte::Base64Encode(StormByte::Safe::get<StormByte::Safe::Binary>(m_store))) + "\"";
	return StormByte::Safe::String(std::string_view(out));
}

Base::PointerType Value::Clone() const {
	return MakePointer<Value>(*this);
}

Base::PointerType Value::Move() {
	return MakePointer<Value>(std::move(*this));
}
