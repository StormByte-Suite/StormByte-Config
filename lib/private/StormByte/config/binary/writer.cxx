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

#include <StormByte/binary_data.hxx>
#include <StormByte/config/binary/writer.hxx>
#include <StormByte/config/item/comment.hxx>
#include <StormByte/config/item/group.hxx>
#include <StormByte/config/item/list.hxx>
#include <StormByte/config/item/value.hxx>
#include <StormByte/helpers.hxx>
#include <StormByte/serializable.hxx>
#include <StormByte/serializable.hxx>
#include <StormByte/safe/string.hxx>

#include <optional>
#include <typeinfo>

using namespace StormByte::Config;
using namespace StormByte::Config::Binary;
using namespace StormByte::Config::Item;

namespace {
	void Append(Buffer& out, const StormByte::BinaryData& bytes) noexcept {
		const auto view = bytes.span();
		out.insert(out.end(), view.begin(), view.end());
	}

	void WriteBase(Buffer& out, const Base& item) {
		Append(out, StormByte::Serializable<Type>(item.Type()).Serialize());
		std::optional<StormByte::Safe::String> name;
		if (!item.Name().empty())
			name = item.Name();
		Append(out, StormByte::Serializable<std::optional<StormByte::Safe::String>>(name).Serialize());
	}

	void WriteItem(Buffer& out, const Base& item, std::uint8_t version);
	void WriteContainer(Buffer& out, const Container& container, std::uint8_t version);

	void WriteContainer(Buffer& out, const Container& container, std::uint8_t version) {
		WriteBase(out, container);
		Append(out, StormByte::Serializable<ContainerType>(container.ContainerType()).Serialize());
		const auto count = static_cast<std::uint64_t>(static_cast<std::size_t>(container.Size()));
		Append(out, StormByte::Serializable<std::uint64_t>(count).Serialize());
		for (const auto& child : container.Items())
			WriteItem(out, *child, version);
	}

	CommentType CommentKind(const Base& item) {
		if (typeid(item) == typeid(Comment<CommentType::SingleLineBash>))
			return CommentType::SingleLineBash;
		if (typeid(item) == typeid(Comment<CommentType::SingleLineC>))
			return CommentType::SingleLineC;
		if (typeid(item) == typeid(Comment<CommentType::MultiLineC>))
			return CommentType::MultiLineC;
		throw StormByte::Config::Exception("Unknown comment specialization");
	}

	void WriteItem(Buffer& out, const Base& item, std::uint8_t version) {
		switch (item.Type()) {
			case Type::String:
				WriteBase(out, item);
				Append(out, StormByte::Serializable<StormByte::Safe::String>(item.As<Text>()).Serialize());
				break;
			case Type::Integer:
				WriteBase(out, item);
				Append(out, StormByte::Serializable<int>(item.As<Integer>()).Serialize());
				break;
			case Type::Double:
				WriteBase(out, item);
				Append(out, StormByte::Serializable<double>(item.As<Double>()).Serialize());
				break;
			case Type::Bool:
				WriteBase(out, item);
				Append(out, StormByte::Serializable<bool>(item.As<Bool>()).Serialize());
				break;
			case Type::Binary:
				WriteBase(out, item);
				Append(out, StormByte::Serializable<StormByte::BinaryData>(item.As<Item::Binary>()).Serialize());
				break;
			case Type::Comment: {
				WriteBase(out, item);
				const CommentType ct = CommentKind(item);
				Append(out, StormByte::Serializable<CommentType>(ct).Serialize());
				switch (ct) {
					case CommentType::SingleLineBash:
						Append(out, StormByte::Serializable<StormByte::Safe::String>(static_cast<const Comment<CommentType::SingleLineBash>&>(item).Text()).Serialize());
						break;
					case CommentType::SingleLineC:
						Append(out, StormByte::Serializable<StormByte::Safe::String>(static_cast<const Comment<CommentType::SingleLineC>&>(item).Text()).Serialize());
						break;
					case CommentType::MultiLineC:
						Append(out, StormByte::Serializable<StormByte::Safe::String>(static_cast<const Comment<CommentType::MultiLineC>&>(item).Text()).Serialize());
						break;
				}
				break;
			}
			case Type::Container:
				WriteContainer(out, static_cast<const Container&>(item), version);
				break;
			case Type::Value:
				break;
		}
	}

	void WriteConfig(Buffer& out, OnExistingAction policy, const Group& root, std::uint8_t version) {
		std::optional<OnExistingAction> pol = policy;
		Append(out, StormByte::Serializable<std::optional<OnExistingAction>>(pol).Serialize());
		WriteContainer(out, root, version);
	}
}

Writer::Writer(const Config& config) noexcept: m_config(config) {}

Buffer Writer::Serialize() const noexcept {
	Buffer out;
	out.reserve(HeaderSize + 64);
	out.insert(out.end(), Magic.begin(), Magic.end());
	Append(out, StormByte::Serializable<std::uint8_t>(CurrentVersion).Serialize());

	const OnExistingAction policy = m_config.m_on_existing_action;
	const Group& root = m_config.m_root;
	WriteConfig(out, policy, root, CurrentVersion);
	return out;
}
