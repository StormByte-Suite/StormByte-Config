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

#include <StormByte/config/item/comment.hxx>
#include <StormByte/config/item/group.hxx>
#include <StormByte/config/item/list.hxx>
#include <StormByte/config/item/value.hxx>
#include <StormByte/type_traits.hxx>

#include <string_view>

namespace StormByte::Config::Item {
	namespace {
		[[noreturn]] void FailAs(const Base& item, const char* wanted) {
			throw Exception("Item {} is not {}", static_cast<std::string_view>(item.Name()), wanted);
		}

		Value& RequireValue(Base& item) {
			if (item.Type() != Item::Type::Integer && item.Type() != Item::Type::Double &&
				item.Type() != Item::Type::Bool && item.Type() != Item::Type::String &&
				item.Type() != Item::Type::Binary)
				FailAs(item, "Value");
			return static_cast<Value&>(item);
		}

		const Value& RequireValue(const Base& item) {
			if (item.Type() != Item::Type::Integer && item.Type() != Item::Type::Double &&
				item.Type() != Item::Type::Bool && item.Type() != Item::Type::String &&
				item.Type() != Item::Type::Binary)
				FailAs(item, "Value");
			return static_cast<const Value&>(item);
		}
	}

	template<class T>
	decltype(auto) Base::As() {
		if constexpr (StormByte::Type::SameAs<T, Value>) {
			return RequireValue(*this);
		} else if constexpr (StormByte::Type::SameAs<T, Container>) {
			if (this->Type() != Item::Type::Container)
				FailAs(*this, "Container");
			return static_cast<Container&>(*this);
		} else if constexpr (StormByte::Type::SameAs<T, Group>) {
			if (this->Type() != Item::Type::Container)
				FailAs(*this, "Group");
			auto& container = static_cast<Container&>(*this);
			if (container.ContainerType() != Item::ContainerType::Group)
				FailAs(*this, "Group");
			return static_cast<Group&>(*this);
		} else if constexpr (StormByte::Type::SameAs<T, List>) {
			if (this->Type() != Item::Type::Container)
				FailAs(*this, "List");
			auto& container = static_cast<Container&>(*this);
			if (container.ContainerType() != Item::ContainerType::List)
				FailAs(*this, "List");
			return static_cast<List&>(*this);
		} else if constexpr (StormByte::Type::SameAs<T, Integer>) {
			return static_cast<int&>(RequireValue(*this));
		} else if constexpr (StormByte::Type::SameAs<T, Double>) {
			return static_cast<double>(RequireValue(*this));
		} else if constexpr (StormByte::Type::SameAs<T, Bool>) {
			return static_cast<bool&>(RequireValue(*this));
		} else if constexpr (StormByte::Type::SameAs<T, Text>) {
			return static_cast<StormByte::Safe::String&>(RequireValue(*this));
		} else if constexpr (StormByte::Type::SameAs<T, Binary>) {
			return static_cast<StormByte::BinaryData&>(RequireValue(*this));
		} else if constexpr (requires { T::CommentTag; }) {
			if (this->Type() != Item::Type::Comment)
				FailAs(*this, "Comment");
			return static_cast<T&>(*this);
		} else {
			static_assert(StormByte::Type::SameAs<T, Value>, "Unsupported As<T>");
		}
	}

	template<class T>
	decltype(auto) Base::As() const {
		if constexpr (StormByte::Type::SameAs<T, Value>) {
			return RequireValue(*this);
		} else if constexpr (StormByte::Type::SameAs<T, Container>) {
			if (this->Type() != Item::Type::Container)
				FailAs(*this, "Container");
			return static_cast<const Container&>(*this);
		} else if constexpr (StormByte::Type::SameAs<T, Group>) {
			if (this->Type() != Item::Type::Container)
				FailAs(*this, "Group");
			const auto& container = static_cast<const Container&>(*this);
			if (container.ContainerType() != Item::ContainerType::Group)
				FailAs(*this, "Group");
			return static_cast<const Group&>(*this);
		} else if constexpr (StormByte::Type::SameAs<T, List>) {
			if (this->Type() != Item::Type::Container)
				FailAs(*this, "List");
			const auto& container = static_cast<const Container&>(*this);
			if (container.ContainerType() != Item::ContainerType::List)
				FailAs(*this, "List");
			return static_cast<const List&>(*this);
		} else if constexpr (StormByte::Type::SameAs<T, Integer>) {
			return static_cast<const int&>(RequireValue(*this));
		} else if constexpr (StormByte::Type::SameAs<T, Double>) {
			return static_cast<double>(RequireValue(*this));
		} else if constexpr (StormByte::Type::SameAs<T, Bool>) {
			return static_cast<const bool&>(RequireValue(*this));
		} else if constexpr (StormByte::Type::SameAs<T, Text>) {
			return static_cast<const StormByte::Safe::String&>(RequireValue(*this));
		} else if constexpr (StormByte::Type::SameAs<T, Binary>) {
			return static_cast<const StormByte::BinaryData&>(RequireValue(*this));
		} else if constexpr (requires { T::CommentTag; }) {
			if (this->Type() != Item::Type::Comment)
				FailAs(*this, "Comment");
			return static_cast<const T&>(*this);
		} else {
			static_assert(StormByte::Type::SameAs<T, Value>, "Unsupported As<T>");
		}
	}
}
