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

#include <StormByte/binary_data.hxx>
#include <StormByte/config/item/base.hxx>
#include <StormByte/config/visibility.h>
#include <StormByte/safe/string.hxx>

#include <string_view>

/**
 * @namespace Item
 * @brief All the classes for item configuration
 */
namespace StormByte::Config::Item {
	/**
	 * @class Value
	 * @brief Scalar configuration item
	 */
	class STORMBYTE_CONFIG_PUBLIC Value: public Base {
		public:
			/**
			 * @brief Constructor
			 * @param value integer
			 */
			explicit Value(int value);

			/**
			 * @brief Constructor
			 * @param value double
			 */
			explicit Value(double value);

			/**
			 * @brief Constructor
			 * @param value bool
			 */
			explicit Value(bool value);

			/**
			 * @brief Constructor
			 * @param value string
			 */
			explicit Value(const StormByte::Safe::String& value);

			/**
			 * @brief Constructor
			 * @param value string
			 */
			explicit Value(StormByte::Safe::String&& value);

			/**
			 * @brief Constructor
			 * @param value string
			 */
			explicit Value(const char* value);

			/**
			 * @brief Constructor
			 * @param value string
			 */
			explicit Value(std::string_view value);

			/**
			 * @brief Constructor
			 * @param value bytes
			 */
			explicit Value(const StormByte::BinaryData& value);

			/**
			 * @brief Constructor
			 * @param value bytes
			 */
			explicit Value(StormByte::BinaryData&& value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value integer
			 */
			Value(const StormByte::Safe::String& name, int value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value double
			 */
			Value(const StormByte::Safe::String& name, double value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value bool
			 */
			Value(const StormByte::Safe::String& name, bool value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value string
			 */
			Value(const StormByte::Safe::String& name, const StormByte::Safe::String& value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value string
			 */
			Value(const StormByte::Safe::String& name, const char* value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value bytes
			 */
			Value(const StormByte::Safe::String& name, const StormByte::BinaryData& value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value integer
			 */
			Value(std::string_view name, int value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value double
			 */
			Value(std::string_view name, double value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value bool
			 */
			Value(std::string_view name, bool value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value string
			 */
			Value(std::string_view name, std::string_view value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value string
			 */
			Value(std::string_view name, const char* value);

			/**
			 * @brief Constructor
			 * @param name name
			 * @param value bytes
			 */
			Value(std::string_view name, const StormByte::BinaryData& value);

			/**
			 * @brief Copy constructor
			 * @param value item
			 */
			Value(const Value& value);

			/**
			 * @brief Move constructor
			 * @param value item
			 */
			Value(Value&& value) noexcept;

			/**
			 * @brief Assignment operator
			 * @param value item
			 * @return this
			 */
			Value& operator=(const Value& value);

			/**
			 * @brief Move assignment operator
			 * @param value item
			 * @return this
			 */
			Value& operator=(Value&& value) noexcept;

			/**
			 * @brief Destructor
			 */
			~Value() noexcept override;

			/**
			 * @brief Payload kind
			 * @return item type
			 */
			Item::Type Kind() const noexcept;

			/**
			 * @brief Item type
			 * @return item type
			 */
			Item::Type Type() const noexcept override;

			/**
			 * @brief Assign integer
			 * @param value integer
			 * @return this
			 */
			Value& operator=(int value);

			/**
			 * @brief Assign double
			 * @param value double
			 * @return this
			 */
			Value& operator=(double value);

			/**
			 * @brief Assign bool
			 * @param value bool
			 * @return this
			 */
			Value& operator=(bool value);

			/**
			 * @brief Assign string
			 * @param value string
			 * @return this
			 */
			Value& operator=(const StormByte::Safe::String& value);

			/**
			 * @brief Assign string
			 * @param value string
			 * @return this
			 */
			Value& operator=(const char* value);

			/**
			 * @brief Assign bytes
			 * @param value bytes
			 * @return this
			 */
			Value& operator=(const StormByte::BinaryData& value);

			/**
			 * @brief Convert to integer
			 */
			operator int&();

			/**
			 * @brief Convert to integer
			 */
			operator const int&() const;

			/**
			 * @brief Convert to double (Integer promotes)
			 */
			operator double() const;

			/**
			 * @brief Convert to double reference
			 */
			operator double&();

			/**
			 * @brief Convert to bool
			 */
			operator bool&();

			/**
			 * @brief Convert to bool
			 */
			operator const bool&() const;

			/**
			 * @brief Convert to string
			 */
			operator StormByte::Safe::String&();

			/**
			 * @brief Convert to string
			 */
			operator const StormByte::Safe::String&() const;

			/**
			 * @brief Convert to bytes
			 */
			operator StormByte::BinaryData&();

			/**
			 * @brief Convert to bytes
			 */
			operator const StormByte::BinaryData&() const;

			/**
			 * @brief Clone
			 * @return pointer
			 */
			PointerType Clone() const override;

			/**
			 * @brief Move
			 * @return pointer
			 */
			PointerType Move() override;

			/**
			 * @brief Serialize
			 * @param indent_level indent
			 * @return text
			 */
			StormByte::Safe::String Serialize(const int& indent_level) const override;

		protected:
			/**
			 * @brief Equals
			 * @param base item
			 * @return bool
			 */
			bool Equals(const Base& base) const override;

		private:
			Item::Type m_kind; ///< Discriminator
			union Storage {
				int integer;
				double floating;
				bool boolean;
				StormByte::Safe::String text;
				StormByte::BinaryData bytes;
				Storage() noexcept {}
				~Storage() noexcept {}
			} m_store; ///< Active payload

			void Destroy() noexcept;
			void CopyFrom(const Value& value);
			void MoveFrom(Value&& value) noexcept;

			/**
			 * @brief Throw on kind mismatch
			 * @param wanted expected kind name
			 */
			[[noreturn]] void Fail(const char* wanted) const;
	};
}

#include <StormByte/config/item/base.txx>

STORMBYTE_DECLARE_MAYBE_SAFE(StormByte::Config::Item::Value);
