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

#include <StormByte/base64.hxx>
#include <StormByte/config/item/value.hxx>
#include <StormByte/safe/string.hxx>

#include <format>
#include <new>
#include <string>
#include <string_view>
#include <utility>

using namespace StormByte::Config::Item;

namespace {
	[[noreturn]] void FailKind(const char* wanted) {
		throw StormByte::Config::Exception("Value is not {}", wanted);
	}
}

void Value::Destroy() noexcept {
	switch (m_kind) {
		case Type::String:
			m_store.text.~String();
			break;
		case Type::Binary:
			m_store.bytes.~BinaryData();
			break;
		default:
			break;
	}
}

void Value::CopyFrom(const Value& value) {
	m_kind = value.m_kind;
	switch (m_kind) {
		case Type::Integer:
			m_store.integer = value.m_store.integer;
			break;
		case Type::Double:
			m_store.floating = value.m_store.floating;
			break;
		case Type::Bool:
			m_store.boolean = value.m_store.boolean;
			break;
		case Type::String:
			new (&m_store.text) StormByte::Safe::String(value.m_store.text);
			break;
		case Type::Binary:
			new (&m_store.bytes) StormByte::BinaryData(value.m_store.bytes);
			break;
		default:
			break;
	}
}

void Value::MoveFrom(Value&& value) noexcept {
	m_kind = value.m_kind;
	switch (m_kind) {
		case Type::Integer:
			m_store.integer = value.m_store.integer;
			break;
		case Type::Double:
			m_store.floating = value.m_store.floating;
			break;
		case Type::Bool:
			m_store.boolean = value.m_store.boolean;
			break;
		case Type::String:
			new (&m_store.text) StormByte::Safe::String(std::move(value.m_store.text));
			break;
		case Type::Binary:
			new (&m_store.bytes) StormByte::BinaryData(std::move(value.m_store.bytes));
			break;
		default:
			break;
	}
}

Value::Value(int value): Base(), m_kind(Type::Integer) {
	m_store.integer = value;
}

Value::Value(double value): Base(), m_kind(Type::Double) {
	m_store.floating = value;
}

Value::Value(bool value): Base(), m_kind(Type::Bool) {
	m_store.boolean = value;
}

Value::Value(const StormByte::Safe::String& value): Base(), m_kind(Type::String) {
	new (&m_store.text) StormByte::Safe::String(value);
}

Value::Value(StormByte::Safe::String&& value): Base(), m_kind(Type::String) {
	new (&m_store.text) StormByte::Safe::String(std::move(value));
}

Value::Value(const char* value): Value(StormByte::Safe::String(value)) {}

Value::Value(std::string_view value): Value(StormByte::Safe::String(value)) {}

Value::Value(const StormByte::BinaryData& value): Base(), m_kind(Type::Binary) {
	new (&m_store.bytes) StormByte::BinaryData(value);
}

Value::Value(StormByte::BinaryData&& value): Base(), m_kind(Type::Binary) {
	new (&m_store.bytes) StormByte::BinaryData(std::move(value));
}

Value::Value(const StormByte::Safe::String& name, int value): Base(name), m_kind(Type::Integer) {
	m_store.integer = value;
}

Value::Value(const StormByte::Safe::String& name, double value): Base(name), m_kind(Type::Double) {
	m_store.floating = value;
}

Value::Value(const StormByte::Safe::String& name, bool value): Base(name), m_kind(Type::Bool) {
	m_store.boolean = value;
}

Value::Value(const StormByte::Safe::String& name, const char* value)
	: Value(name, StormByte::Safe::String(value)) {}

Value::Value(const StormByte::Safe::String& name, const StormByte::Safe::String& value): Base(name), m_kind(Type::String) {
	new (&m_store.text) StormByte::Safe::String(value);
}

Value::Value(std::string_view name, int value): Value(StormByte::Safe::String(name), value) {}

Value::Value(std::string_view name, double value): Value(StormByte::Safe::String(name), value) {}

Value::Value(std::string_view name, bool value): Value(StormByte::Safe::String(name), value) {}

Value::Value(std::string_view name, std::string_view value)
	: Value(StormByte::Safe::String(name), StormByte::Safe::String(value)) {}

Value::Value(std::string_view name, const char* value)
	: Value(StormByte::Safe::String(name), StormByte::Safe::String(value)) {}

Value::Value(const StormByte::Safe::String& name, const StormByte::BinaryData& value): Base(name), m_kind(Type::Binary) {
	new (&m_store.bytes) StormByte::BinaryData(value);
}

Value::Value(std::string_view name, const StormByte::BinaryData& value)
	: Value(StormByte::Safe::String(name), value) {}

Value::Value(const Value& value): Base(value), m_kind(Type::Integer) {
	CopyFrom(value);
}

Value::Value(Value&& value) noexcept: Base(std::move(value)), m_kind(Type::Integer) {
	MoveFrom(std::move(value));
}

Value& Value::operator=(const Value& value) {
	if (this == &value)
		return *this;
	Base::operator=(value);
	Destroy();
	CopyFrom(value);
	return *this;
}

Value& Value::operator=(Value&& value) noexcept {
	if (this == &value)
		return *this;
	Base::operator=(std::move(value));
	Destroy();
	MoveFrom(std::move(value));
	return *this;
}

Value::~Value() noexcept {
	Destroy();
}

Type Value::Kind() const noexcept {
	return m_kind;
}

Type Value::Type() const noexcept {
	return m_kind;
}

bool Value::Equals(const Base& base) const {
	const auto& value = static_cast<const Value&>(base);
	if (m_kind != value.m_kind)
		return false;
	switch (m_kind) {
		case Type::Integer:
			return m_store.integer == value.m_store.integer;
		case Type::Double:
			return m_store.floating == value.m_store.floating;
		case Type::Bool:
			return m_store.boolean == value.m_store.boolean;
		case Type::String:
			return m_store.text == value.m_store.text;
		case Type::Binary:
			return m_store.bytes == value.m_store.bytes;
		default:
			return false;
	}
}

Value& Value::operator=(int value) {
	if (m_kind != Type::Integer)
		FailKind("Integer");
	m_store.integer = value;
	return *this;
}

Value& Value::operator=(double value) {
	if (m_kind != Type::Double)
		FailKind("Double");
	m_store.floating = value;
	return *this;
}

Value& Value::operator=(bool value) {
	if (m_kind != Type::Bool)
		FailKind("Bool");
	m_store.boolean = value;
	return *this;
}

Value& Value::operator=(const StormByte::Safe::String& value) {
	if (m_kind != Type::String)
		FailKind("String");
	m_store.text = value;
	return *this;
}

Value& Value::operator=(const char* value) {
	if (m_kind != Type::String)
		FailKind("String");
	m_store.text = StormByte::Safe::String(value);
	return *this;
}

Value& Value::operator=(const StormByte::BinaryData& value) {
	if (m_kind != Type::Binary)
		FailKind("Binary");
	m_store.bytes = value;
	return *this;
}

Value::operator int&() {
	if (m_kind != Type::Integer)
		FailKind("Integer");
	return m_store.integer;
}

Value::operator const int&() const {
	if (m_kind != Type::Integer)
		FailKind("Integer");
	return m_store.integer;
}

Value::operator double() const {
	if (m_kind == Type::Double)
		return m_store.floating;
	if (m_kind == Type::Integer)
		return static_cast<double>(m_store.integer);
	FailKind("Double");
}

Value::operator double&() {
	if (m_kind != Type::Double)
		FailKind("Double");
	return m_store.floating;
}

Value::operator bool&() {
	if (m_kind != Type::Bool)
		FailKind("Bool");
	return m_store.boolean;
}

Value::operator const bool&() const {
	if (m_kind != Type::Bool)
		FailKind("Bool");
	return m_store.boolean;
}

Value::operator StormByte::Safe::String&() {
	if (m_kind != Type::String)
		FailKind("String");
	return m_store.text;
}

Value::operator const StormByte::Safe::String&() const {
	if (m_kind != Type::String)
		FailKind("String");
	return m_store.text;
}

Value::operator StormByte::BinaryData&() {
	if (m_kind != Type::Binary)
		FailKind("Binary");
	return m_store.bytes;
}

Value::operator const StormByte::BinaryData&() const {
	if (m_kind != Type::Binary)
		FailKind("Binary");
	return m_store.bytes;
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
	switch (m_kind) {
		case Type::Integer:
			out += std::format("{}", m_store.integer);
			break;
		case Type::Double:
			out += std::format("{}", m_store.floating);
			if (out.find('.') == std::string::npos && out.find('e') == std::string::npos && out.find('E') == std::string::npos)
				out += ".0";
			break;
		case Type::Bool:
			out += m_store.boolean ? "true" : "false";
			break;
		case Type::String:
			out += "\"" + static_cast<std::string>(m_store.text) + "\"";
			break;
		case Type::Binary:
			out += "b\"" + static_cast<std::string>(StormByte::Base64Encode(m_store.bytes)) + "\"";
			break;
		default:
			break;
	}
	return StormByte::Safe::String(std::string_view(out));
}

Base::PointerType Value::Clone() const {
	return MakePointer<Value>(*this);
}

Base::PointerType Value::Move() {
	return MakePointer<Value>(std::move(*this));
}
