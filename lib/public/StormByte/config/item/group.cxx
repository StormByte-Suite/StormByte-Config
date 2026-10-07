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

#include <StormByte/config/item/group.hxx>
#include <StormByte/safe/string.hxx>

#include <algorithm>
#include <string>

using namespace StormByte::Config::Item;

Group::Group() noexcept = default;

Group::Group(const StormByte::Safe::String& name): Container(name) {}

Group::Group(StormByte::Safe::String&& name): Container(std::move(name)) {}

Group::Group(std::string_view name): Group(StormByte::Safe::String(name)) {}

Group::Group(const Group& group) = default;

Group::Group(Group&& group) noexcept = default;

Group& Group::operator=(const Group& group) = default;

Group& Group::operator=(Group&& group) noexcept = default;

Group::~Group() noexcept = default;

Base::PointerType Group::Clone() const {
	return MakePointer<Group>(*this);
}

Base::PointerType Group::Move() {
	return MakePointer<Group>(std::move(*this));
}

Base::PointerType Group::BeforeAdditionActions(Base::PointerType item, const StormByte::Config::OnExistingAction onexisting) {
	if (!item)
		throw Exception("Tried to add a null pointer to Group!");

	if (item->Type() != Item::Type::Comment) {
		if (item->Name().empty())
			throw InvalidName("Groups require every item to have a name");
		if (!Base::IsNameValid(item->Name()))
			throw InvalidName("Invalid item name '{}' for Group (must start with a letter and contain only letters, digits or underscores)", static_cast<std::string_view>(item->Name()));

		const auto it = std::find_if(m_items.begin(), m_items.end(), [&item](const Base::PointerType& i) {
			return i->Type() != Item::Type::Comment && i->Name() == item->Name();
		});
		if (it != m_items.end()) {
			switch (onexisting) {
				case OnExistingAction::Keep:
					return *it;
				case OnExistingAction::Overwrite:
					m_items.erase(it);
					break;
				case OnExistingAction::ThrowException:
					throw ItemNameAlreadyExists(std::string(static_cast<std::string_view>(item->Name())));
			}
		}
	}

	m_items.push_back(item);
	return m_items.back();
}
