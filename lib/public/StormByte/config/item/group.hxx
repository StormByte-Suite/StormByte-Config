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
	 * @class Group
	 * @brief Named container (`{ ... }`).
	 */
	class STORMBYTE_CONFIG_PUBLIC Group: public Container {
		public:
			/**
			 * @brief Constructor
			 */
			Group() noexcept;

			/**
			 * @brief Constructor
			 * @param name	name of the group
			 */
			explicit Group(const StormByte::Safe::String& name);

			/**
			 * @brief Constructor
			 * @param name	name of the group
			 */
			explicit Group(StormByte::Safe::String&& name);

			/**
			 * @brief Constructor
			 * @param name	name of the group
			 */
			explicit Group(std::string_view name);

			/**
			 * @brief Copy constructor
			 * @param group	group to copy
			 */
			Group(const Group& group);

			/**
			 * @brief Move constructor
			 * @param group	group to move
			 */
			Group(Group&& group) noexcept;

			/**
			 * @brief Assignment operator
			 * @param group	group to copy
			 * @return		this
			 */
			Group& operator=(const Group& group);

			/**
			 * @brief Move assignment operator
			 * @param group	group to move
			 * @return		this
			 */
			Group& operator=(Group&& group) noexcept;

			/**
			 * @brief Destructor
			 */
			~Group() noexcept override;

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
				return Item::ContainerType::Group;
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

STORMBYTE_DECLARE_MAYBE_SAFE(StormByte::Config::Item::Group);
