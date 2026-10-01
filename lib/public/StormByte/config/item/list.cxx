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

#include <StormByte/config/item/list.hxx>
#include <StormByte/safe/string.hxx>

#include <algorithm>
#include <string>

using namespace StormByte::Config::Item;

List::List() noexcept = default;

List::List(const StormByte::Safe::String& name): Container(name) {}

List::List(StormByte::Safe::String&& name): Container(std::move(name)) {}

List::List(std::string_view name): Container(StormByte::Safe::String(name)) {}

List::List(const List& list) = default;

List::List(List&& list) noexcept = default;

List& List::operator=(const List& list) = default;

List& List::operator=(List&& list) noexcept = default;

List::~List() noexcept = default;

Base::PointerType List::BeforeAdditionActions(Base::PointerType item, const StormByte::Config::OnExistingAction on_existing) {
	if (!item)
		throw Exception("Tried to add a null pointer to List!");

	if (item->Type() != Item::Type::Comment) {
		if (!item->Name().empty())
			throw InvalidName("Lists do not support named items (received name '{}')", static_cast<std::string_view>(item->Name()));

		auto item_iterator = std::find_if(m_items.begin(), m_items.end(), [&item](Base::PointerType& i) {
			return *i == *item;
		});
		if (item_iterator != m_items.end()) {
			switch (on_existing) {
				case OnExistingAction::Keep:
					return *item_iterator;
				case OnExistingAction::Overwrite:
					m_items.erase(item_iterator);
					break;
				case OnExistingAction::ThrowException:
					throw ItemAlreadyExists("Config item already exists in List");
			}
		}
	}

	m_items.push_back(item);
	return m_items.back();
}
