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

#pragma once

#include <StormByte/safe/clonable.hxx>
#include <StormByte/config/exception.hxx>
#include <StormByte/config/item/type.hxx>
#include <StormByte/config/visibility.h>
#include <StormByte/safe/string.hxx>

#include <string_view>

/**
 * @namespace Item
 * @brief All the classes for item configuration
 */
namespace StormByte::Config::Item {
	class Container;
	class Group;
	class List;
	class Value;
	template<CommentType T> class Comment;

	/**
	 * @class Base
	 * @brief Base class for all configuration items
	 */
	class STORMBYTE_CONFIG_PUBLIC Base: public StormByte::Safe::Clonable<Base, StormByte::Safe::Shared<Base>> {
		public:
			/**
			 * @brief Constructor
			 */
			Base() noexcept;

			/**
			 * @brief Constructor
			 * @param name	name of the item
			 */
			Base(const StormByte::Safe::String& name);

			/**
			 * @brief Copy constructor
			 * @param base	item to copy
			 */
			Base(const Base& base) noexcept = default;

			/**
			 * @brief Move constructor
			 * @param base	item to move
			 */
			Base(Base&& base) noexcept = default;

			/**
			 * @brief Assignment operator
			 * @param base	item to copy
			 * @return		copied item
			 */
			Base& operator=(const Base& base) noexcept = default;

			/**
			 * @brief Move assignment operator
			 * @param base	item to move
			 * @return		moved item
			 */
			Base& operator=(Base&& base) noexcept = default;

			/**
			 * @brief Destructor
			 */
			~Base() noexcept override;

			/**
			 * @brief Compares two items
			 * @param base	item to compare
			 * @return		bool
			 */
			bool operator==(const Base& base) const noexcept;

			/**
			 * @brief Compares two items
			 * @param base	item to compare
			 * @return		bool
			 */
			bool operator!=(const Base& base) const noexcept;

			/**
			 * @brief Gets the name of the item
			 * @return	name
			 */
			constexpr const StormByte::Safe::String& Name() const noexcept {
				return m_name;
			}

			/**
			 * @brief Sets the name of the item
			 * @param name	name
			 */
			constexpr void Name(const StormByte::Safe::String& name) {
				m_name = name;
			}

			/**
			 * @brief Sets the name of the item
			 * @param name	name
			 */
			void Name(std::string_view name);

			/**
			 * @brief Gets the type of the item
			 * @return	type
			 */
			constexpr virtual Item::Type Type() const noexcept = 0;

			/**
			 * @brief Gets the type as string
			 * @return	type as string
			 */
			constexpr std::string_view TypeToString() const noexcept {
				return Item::TypeToString(this->Type());
			}

			/**
			 * @brief Checks if name is valid
			 * @param name	name
			 * @return		bool
			 */
			static bool IsNameValid(const StormByte::Safe::String& name) noexcept;

			/**
			 * @brief Checks if name is valid
			 * @param name	name
			 * @return		bool
			 */
			static bool IsNameValid(std::string_view name) noexcept;

			/**
			 * @brief Typed view of this item
			 * @tparam T	node type, leaf tag or comment specialization
			 * @return		reference or converted value depending on T
			 */
			template<class T> decltype(auto) As();

			/**
			 * @brief Typed view of this item
			 * @tparam T	node type, leaf tag or comment specialization
			 * @return		reference or converted value depending on T
			 */
			template<class T> decltype(auto) As() const;

			/**
			 * @brief Serializes the item as text
			 * @param indent_level	indent
			 * @return				serialized item
			 */
			virtual StormByte::Safe::String Serialize(const int& indent_level = 0) const;

		protected:
			StormByte::Safe::String m_name; ///< Name of the item

			/**
			 * @brief Equals
			 * @param base	item
			 * @return		bool
			 */
			virtual bool Equals(const Base& base) const = 0;
	};
}
