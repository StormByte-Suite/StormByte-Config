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

#include <StormByte/config/item/container.hxx>
#include <StormByte/config/visibility.h>
#include <StormByte/safe/string.hxx>

#include <string_view>

/**
 * @brief Configuration items (values, comments, groups, lists).
 */
namespace StormByte::Config::Item {
	/**
	 * @class List
	 * @brief Unnamed container (`[ ... ]`).
	 */
	class STORMBYTE_CONFIG_PUBLIC List final: public Container {
		public:
			/**
			 * @brief Constructor
			 */
			List() noexcept;

			/**
			 * @brief Constructor
			 * @param name	name of the list
			 */
			explicit List(const StormByte::Safe::String& name);

			/**
			 * @brief Constructor
			 * @param name	name of the list
			 */
			explicit List(StormByte::Safe::String&& name);

			/**
			 * @brief Constructor
			 * @param name	name of the list
			 */
			explicit List(std::string_view name);

			/**
			 * @brief Copy constructor
			 * @param list	list to copy
			 */
			List(const List& list);

			/**
			 * @brief Move constructor
			 * @param list	list to move
			 */
			List(List&& list) noexcept;

			/**
			 * @brief Assignment operator
			 * @param list	list to copy
			 * @return		this
			 */
			List& operator=(const List& list);

			/**
			 * @brief Move assignment operator
			 * @param list	list to move
			 * @return		this
			 */
			List& operator=(List&& list) noexcept;

			/**
			 * @brief Destructor
			 */
			~List() noexcept override;

			/**
			 * @brief Clone
			 * @return	pointer
			 */
			PointerType Clone() const override;

			/**
			 * @brief Move
			 * @return	pointer
			 */
			PointerType Move() override;

			/**
			 * @brief Container type
			 * @return	type
			 */
			constexpr Item::ContainerType ContainerType() const noexcept override {
				return Item::ContainerType::List;
			}

			/**
			 * @brief Item type
			 * @return	type
			 */
			constexpr Item::Type Type() const noexcept override {
				return Item::Type::Container;
			}

		protected:
			/**
			 * @brief Actions before addition
			 * @param item			item
			 * @param onexisting	collision policy
			 * @return				pointer
			 */
			Base::PointerType BeforeAdditionActions(Base::PointerType item, const StormByte::Config::OnExistingAction onexisting) override;
	};
}

#include <StormByte/config/item/base.txx>

STORMBYTE_DECLARE_MAYBE_SAFE(StormByte::Config::Item::List);
