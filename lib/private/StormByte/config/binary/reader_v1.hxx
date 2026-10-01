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

#include <StormByte/config/binary/cursor.hxx>
#include <StormByte/config/item/container.hxx>
#include <StormByte/config/item/group.hxx>
#include <StormByte/config/visibility.h>
#include <StormByte/safe/string.hxx>

#include <optional>
#include <utility>

/**
 * @namespace StormByte::Config::Binary
 * @brief Versioned binary wire format for configuration documents.
 */
namespace StormByte::Config::Binary {
	/**
	 * @class ReaderV1
	 * @brief Reads format version 1 (library 1.1.0 and the current writer).
	 *
	 * Not a base class. A later format is a new reader, selected by
	 * @ref Reader, not an override of this one.
	 */
	class STORMBYTE_CONFIG_PRIVATE ReaderV1 final {
		public:
			ReaderV1() = delete;

			/**
			 * @brief Read one version-1 payload.
			 * @param payload Bytes after the magic and the version byte.
			 * @return Collision policy and root group, or a deserialize error.
			 */
			static Expected<std::pair<OnExistingAction, Item::Group>, DeserializeError> Read(BufferView payload);

		private:
			/**
			 * @brief Read the type tag and the optional name.
			 * @param cursor Read position. Advanced past both fields.
			 * @return Type and name.
			 */
			static Expected<std::pair<Item::Type, std::optional<StormByte::Safe::String>>, DeserializeError>
			ReadBase(Cursor& cursor);

			/**
			 * @brief Assign @p name when it is present.
			 * @param item Item to name.
			 * @param name Decoded name. Empty means the item stays unnamed.
			 */
			static void ApplyName(Item::Base& item, std::optional<StormByte::Safe::String>& name);

			/**
			 * @brief Read one item, including a nested container.
			 * @param cursor Read position. Advanced past the item.
			 * @return The item.
			 */
			static Expected<StormByte::Safe::Shared<Item::Base>, DeserializeError> ReadItem(Cursor& cursor);

			/**
			 * @brief Fill @p container from its header and its children.
			 * @param cursor Read position, at the container's type tag.
			 * @param container Group or list to fill. Its kind must match the wire.
			 * @return Nothing, or a deserialize error.
			 */
			static Expected<void, DeserializeError> ReadContainerInto(Cursor& cursor, Item::Container& container);
	};
}
