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

#include <StormByte/binary_data.hxx>
#include <StormByte/config/binary/reader_v1.hxx>
#include <StormByte/config/item/comment.hxx>
#include <StormByte/config/item/group.hxx>
#include <StormByte/config/item/list.hxx>
#include <StormByte/config/item/value.hxx>
#include <StormByte/serializable.hxx>
#include <StormByte/serializable.hxx>
#include <StormByte/safe/string.hxx>

#include <cstdint>
#include <optional>
#include <utility>

using namespace StormByte;
using namespace StormByte::Config;
using namespace StormByte::Config::Binary;

Expected<std::pair<Item::Type, std::optional<StormByte::Safe::String>>, DeserializeError>
ReaderV1::ReadBase(Cursor& cursor) {
	if (cursor.offset() >= cursor.data().size())
		return Unexpected<DeserializeError>("Insufficient data for item type");

	auto type = cursor.Take<Item::Type>();
	if (!type)
		return Unexpected(type.error());

	if (cursor.offset() >= cursor.data().size())
		return Unexpected<DeserializeError>("Insufficient data for item name");

	auto name = cursor.Take<std::optional<StormByte::Safe::String>>();
	if (!name)
		return Unexpected(name.error());

	return std::make_pair(type.value(), std::move(name.value()));
}

void ReaderV1::ApplyName(Item::Base& item, std::optional<StormByte::Safe::String>& name) {
	if (name)
		item.Name(*name);
}

Expected<void, DeserializeError> ReaderV1::ReadContainerInto(Cursor& cursor, Item::Container& container) {
	auto basic = ReadBase(cursor);
	if (!basic)
		return Unexpected(basic.error());

	auto [type, name] = std::move(basic.value());
	if (type != Item::Type::Container)
		return Unexpected<DeserializeError>("Expected container item");
	ApplyName(container, name);

	if (cursor.offset() >= cursor.data().size())
		return Unexpected<DeserializeError>("Insufficient data for container type");

	auto ctype = cursor.Take<Item::ContainerType>();
	if (!ctype)
		return Unexpected(ctype.error());
	if (ctype.value() != container.ContainerType())
		return Unexpected<DeserializeError>("Container type mismatch");

	if (cursor.offset() >= cursor.data().size())
		return Unexpected<DeserializeError>("Insufficient data for container size");

	auto count = cursor.Take<std::uint64_t>();
	if (!count)
		return Unexpected(count.error());
	if (count.value() > static_cast<std::uint64_t>(cursor.remaining()))
		return Unexpected<DeserializeError>("Claimed container size exceeds buffer");

	for (std::uint64_t i = 0; i < count.value(); ++i) {
		auto child = ReadItem(cursor);
		if (!child)
			return Unexpected(child.error());
		container.Add(std::move(child.value()), container.GetOnExistingAction());
	}

	return {};
}

Expected<StormByte::Safe::Shared<Item::Base>, DeserializeError> ReaderV1::ReadItem(Cursor& cursor) {
	const std::size_t start = cursor.offset();

	auto basic = ReadBase(cursor);
	if (!basic)
		return Unexpected(basic.error());

	auto [type, name] = std::move(basic.value());

	switch (type) {
		case Item::Type::String: {
			auto value = cursor.Take<StormByte::Safe::String>();
			if (!value)
				return Unexpected(value.error());
			auto item = Item::Base::MakePointer<Item::Value>(std::move(value.value()));
			ApplyName(*item, name);
			return item;
		}
		case Item::Type::Integer: {
			auto value = cursor.Take<int>();
			if (!value)
				return Unexpected(value.error());
			auto item = Item::Base::MakePointer<Item::Value>(value.value());
			ApplyName(*item, name);
			return item;
		}
		case Item::Type::Double: {
			auto value = cursor.Take<double>();
			if (!value)
				return Unexpected(value.error());
			auto item = Item::Base::MakePointer<Item::Value>(value.value());
			ApplyName(*item, name);
			return item;
		}
		case Item::Type::Bool: {
			auto value = cursor.Take<bool>();
			if (!value)
				return Unexpected(value.error());
			auto item = Item::Base::MakePointer<Item::Value>(value.value());
			ApplyName(*item, name);
			return item;
		}
		case Item::Type::Binary: {
			auto value = cursor.Take<BinaryData>();
			if (!value)
				return Unexpected(value.error());
			auto item = Item::Base::MakePointer<Item::Value>(std::move(value.value()));
			ApplyName(*item, name);
			return item;
		}
		case Item::Type::Comment: {
			auto ct = cursor.Take<Item::CommentType>();
			if (!ct)
				return Unexpected(ct.error());

			auto text = cursor.Take<StormByte::Safe::String>();
			if (!text)
				return Unexpected(text.error());

			StormByte::Safe::Shared<Item::Base> item;
			switch (ct.value()) {
				case Item::CommentType::SingleLineBash:
					item = Item::Base::MakePointer<Item::Comment<Item::CommentType::SingleLineBash>>(std::move(text.value()));
					break;
				case Item::CommentType::SingleLineC:
					item = Item::Base::MakePointer<Item::Comment<Item::CommentType::SingleLineC>>(std::move(text.value()));
					break;
				case Item::CommentType::MultiLineC:
					item = Item::Base::MakePointer<Item::Comment<Item::CommentType::MultiLineC>>(std::move(text.value()));
					break;
				default:
					return Unexpected<DeserializeError>("Unknown comment type");
			}

			ApplyName(*item, name);
			return item;
		}
		case Item::Type::Container: {
			cursor.offset(start);
			Cursor peek = cursor;
			auto peek_basic = ReadBase(peek);
			if (!peek_basic)
				return Unexpected(peek_basic.error());
			if (peek.offset() >= peek.data().size())
				return Unexpected<DeserializeError>("Insufficient data for container type");

			auto ctype = peek.Take<Item::ContainerType>();
			if (!ctype)
				return Unexpected(ctype.error());

			StormByte::Safe::Shared<Item::Base> container;
			if (ctype.value() == Item::ContainerType::Group)
				container = Item::Base::MakePointer<Item::Group>();
			else if (ctype.value() == Item::ContainerType::List)
				container = Item::Base::MakePointer<Item::List>();
			else
				return Unexpected<DeserializeError>("Unknown container type");

			Expected<void, DeserializeError> filled;
			if (ctype.value() == Item::ContainerType::List)
				filled = ReadContainerInto(cursor, container->As<Item::List>());
			else
				filled = ReadContainerInto(cursor, container->As<Item::Group>());
			if (!filled)
				return Unexpected(filled.error());
			return container;
		}
		default:
			return Unexpected<DeserializeError>("Unhandled item type");
	}
}

Expected<std::pair<OnExistingAction, Item::Group>, DeserializeError> ReaderV1::Read(BufferView payload) {
	Cursor cursor(payload);

	if (cursor.offset() >= cursor.data().size())
		return Unexpected<DeserializeError>("Insufficient data for on_existing_action");

	auto policy = cursor.Take<std::optional<OnExistingAction>>();
	if (!policy)
		return Unexpected(policy.error());

	OnExistingAction resolved = OnExistingAction::ThrowException;
	if (policy.value())
		resolved = *policy.value();

	Item::Group root;
	root.SetOnExistingAction(resolved);
	auto root_read = ReadContainerInto(cursor, root);
	if (!root_read)
		return Unexpected(root_read.error());

	return std::make_pair(resolved, std::move(root));
}
