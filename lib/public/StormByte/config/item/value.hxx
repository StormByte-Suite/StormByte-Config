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

#pragma once

#include <StormByte/config/item/base.hxx>
#include <StormByte/config/visibility.h>
#include <StormByte/safe/binary.hxx>
#include <StormByte/safe/string.hxx>
#include <StormByte/safe/variant.hxx>

#include <string_view>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @namespace StormByte::Config
	 * @brief Human-readable and versioned configuration documents.
	 */
	namespace Config {
		/**
		 * @namespace StormByte::Config::Item
		 * @brief Nodes stored in a document.
		 */
		namespace Item {
			/**
			 * @class Value
			 * @brief Scalar configuration item.
			 *
			 * The payload is a @ref StormByte::Safe::Variant. Integer, double, bool, text and bytes do not share storage by hand.
			 * A text literal binds to `const char*`. That overload is exact and beats the `bool` conversion of a pointer. `std::string` and `Safe::String` bind to `string_view`.
			 */
			class STORMBYTE_CONFIG_PUBLIC Value: public Base {
				public:
					/**
					 * @brief Integer payload.
					 * @param value Integer.
					 */
					explicit Value(int value);

					/**
					 * @brief Double payload.
					 * @param value Double.
					 */
					explicit Value(double value);

					/**
					 * @brief Bool payload.
					 * @param value Bool.
					 */
					explicit Value(bool value);

					/**
					 * @brief Text payload from a C string.
					 * @param value Source. A null pointer is empty text.
					 */
					explicit Value(const char* value);

					/**
					 * @brief Text payload from a view.
					 * @param value Source. A `std::string` or `Safe::String` binds here. Embedded NUL counts.
					 */
					explicit Value(std::string_view value);

					/**
					 * @brief Text payload.
					 * @param value Owned text. Left empty.
					 */
					explicit Value(StormByte::Safe::String&& value);

					/**
					 * @brief Byte payload.
					 * @param value Owned bytes.
					 */
					explicit Value(const StormByte::Safe::Binary& value);

					/**
					 * @brief Byte payload.
					 * @param value Owned bytes. Left empty.
					 */
					explicit Value(StormByte::Safe::Binary&& value);

					/**
					 * @brief Named integer.
					 * @param name Item name.
					 * @param value Integer.
					 */
					Value(std::string_view name, int value);

					/**
					 * @brief Named double.
					 * @param name Item name.
					 * @param value Double.
					 */
					Value(std::string_view name, double value);

					/**
					 * @brief Named bool.
					 * @param name Item name.
					 * @param value Bool.
					 */
					Value(std::string_view name, bool value);

					/**
					 * @brief Named text from a C string.
					 * @param name Item name.
					 * @param value C string. A null pointer is empty text.
					 */
					Value(std::string_view name, const char* value);

					/**
					 * @brief Named text.
					 * @param name Item name.
					 * @param value Text. A `std::string` or `Safe::String` binds here. Embedded NUL counts.
					 */
					Value(std::string_view name, std::string_view value);

					/**
					 * @brief Named bytes.
					 * @param name Item name.
					 * @param value Owned bytes.
					 */
					Value(std::string_view name, const StormByte::Safe::Binary& value);

					/**
					 * @brief Named bytes.
					 * @param name Item name.
					 * @param value Owned bytes. Left empty.
					 */
					Value(std::string_view name, StormByte::Safe::Binary&& value);

					/**
					 * @brief Copy constructor.
					 * @param value Source.
					 */
					Value(const Value& value);

					/**
					 * @brief Move constructor.
					 * @param value Source. Left as an integer zero.
					 */
					Value(Value&& value) noexcept;

					/**
					 * @brief Destructor.
					 */
					~Value() noexcept override;

					/**
					 * @brief Copy assignment.
					 * @param value Source.
					 * @return This item.
					 */
					Value& operator=(const Value& value);

					/**
					 * @brief Move assignment.
					 * @param value Source. Left as an integer zero.
					 * @return This item.
					 */
					Value& operator=(Value&& value) noexcept;

					/**
					 * @brief Payload kind.
					 * @return Active alternative as an item type.
					 */
					Item::Type Kind() const noexcept;

					/**
					 * @brief Item type.
					 * @return Same as @ref Kind.
					 */
					Item::Type Type() const noexcept override;

					/**
					 * @brief Assign an integer. The kind must already be integer.
					 * @param value Integer.
					 * @return This item.
					 * @throws Exception The payload is not an integer.
					 */
					Value& operator=(int value);

					/**
					 * @brief Assign a double. The kind must already be double.
					 * @param value Double.
					 * @return This item.
					 * @throws Exception The payload is not a double.
					 */
					Value& operator=(double value);

					/**
					 * @brief Assign a bool. The kind must already be bool.
					 * @param value Bool.
					 * @return This item.
					 * @throws Exception The payload is not a bool.
					 */
					Value& operator=(bool value);

					/**
					 * @brief Assign text from a C string. The kind must already be text.
					 * @param value C string. A null pointer is empty text.
					 * @return This item.
					 * @throws Exception The payload is not text.
					 */
					Value& operator=(const char* value);

					/**
					 * @brief Assign text. The kind must already be text.
					 * @param value Text. A `std::string` or `Safe::String` binds here.
					 * @return This item.
					 * @throws Exception The payload is not text.
					 */
					Value& operator=(std::string_view value);

					/**
					 * @brief Assign bytes. The kind must already be bytes.
					 * @param value Owned bytes.
					 * @return This item.
					 * @throws Exception The payload is not bytes.
					 */
					Value& operator=(const StormByte::Safe::Binary& value);

					/**
					 * @brief Integer reference.
					 * @return Active integer.
					 * @throws Exception The payload is not an integer.
					 */
					operator int&();

					/**
					 * @brief Integer reference.
					 * @return Active integer.
					 * @throws Exception The payload is not an integer.
					 */
					operator const int&() const;

					/**
					 * @brief Double value. An integer promotes.
					 * @return Active double, or the integer widened.
					 * @throws Exception The payload is neither double nor integer.
					 */
					operator double() const;

					/**
					 * @brief Double reference. An integer does not promote.
					 * @return Active double.
					 * @throws Exception The payload is not a double.
					 */
					operator double&();

					/**
					 * @brief Bool reference.
					 * @return Active bool.
					 * @throws Exception The payload is not a bool.
					 */
					operator bool&();

					/**
					 * @brief Bool reference.
					 * @return Active bool.
					 * @throws Exception The payload is not a bool.
					 */
					operator const bool&() const;

					/**
					 * @brief Text reference.
					 * @return Active text.
					 * @throws Exception The payload is not text.
					 */
					operator StormByte::Safe::String&();

					/**
					 * @brief Text reference.
					 * @return Active text.
					 * @throws Exception The payload is not text.
					 */
					operator const StormByte::Safe::String&() const;

					/**
					 * @brief Byte reference.
					 * @return Active bytes.
					 * @throws Exception The payload is not bytes.
					 */
					operator StormByte::Safe::Binary&();

					/**
					 * @brief Byte reference.
					 * @return Active bytes.
					 * @throws Exception The payload is not bytes.
					 */
					operator const StormByte::Safe::Binary&() const;

					/**
					 * @brief Clone onto the Config heap.
					 * @return Shared copy.
					 */
					PointerType Clone() const override;

					/**
					 * @brief Move onto the Config heap.
					 * @return Shared value. This item is left as an integer zero.
					 */
					PointerType Move() override;

					/**
					 * @brief Text form of this item.
					 * @param indent_level Tab count before the line.
					 * @return One line, without a trailing newline.
					 */
					StormByte::Safe::String Serialize(const int& indent_level) const override;

				protected:
					/**
					 * @brief Compare payloads after the base has matched the type.
					 * @param base Other item, already a @ref Value.
					 * @return Whether the payloads are equal.
					 */
					bool Equals(const Base& base) const override;

				private:
					using Payload = StormByte::Safe::Variant<int, double, bool, StormByte::Safe::String, StormByte::Safe::Binary>;	///< Active scalar.

					Payload m_store;	///< Active scalar.

					/**
					 * @brief Throw when the active alternative is not @p wanted.
					 * @param wanted Expected kind name.
					 */
					[[noreturn]] void Fail(const char* wanted) const;
			};
		}
	}
}

#include <StormByte/config/item/base.txx>

STORMBYTE_DECLARE_MAYBE_SAFE(StormByte::Config::Item::Value);
