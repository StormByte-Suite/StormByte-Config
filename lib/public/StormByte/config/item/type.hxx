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

#include <StormByte/config/exception.hxx>
#include <StormByte/config/visibility.h>
#include <StormByte/safe/string.hxx>

#include <string_view>

/**
 * @brief Configuration items (values, comments, groups, lists).
 */
namespace StormByte::Config::Item {
	/**
	 * @enum Type
	 * @brief Node kind in the document tree.
	 */
	enum class Type: char {
		Bool,       ///< Boolean leaf.
		Comment,    ///< Comment node.
		Container,  ///< Group or list.
		Double,     ///< Double leaf.
		Integer,    ///< Integer leaf.
		String,     ///< Text leaf (`StormByte::Safe::String`).
		Binary,     ///< Binary leaf (`StormByte::BinaryData`).
		Value       ///< Any leaf (`Value` node). Not used on the wire.
	};

	/**
	 * @brief Converts a Type to its string representation.
	 * @param t The Type to convert.
	 * @return Literal name. Lives for the program.
	 */
	static STORMBYTE_FORCE_INLINE constexpr std::string_view TypeToString(const Type& t) noexcept {
		switch(t) {
			case Type::String:		return "String";
			case Type::Integer:		return "Integer";
			case Type::Double:		return "Double";
			case Type::Comment:		return "Comment";
			case Type::Bool:		return "Bool";
			case Type::Container:	return "Container";
			case Type::Binary:		return "Binary";
			case Type::Value:		return "Value";
			default:				return "Unknown";
		}
	}

	/**
	 * @enum CommentType
	 * @brief Comment syntax used in the text document.
	 */
	enum class STORMBYTE_CONFIG_PUBLIC CommentType: char {
		SingleLineBash = 2,		///< `#` until end of line
		SingleLineC = 5,		///< `//` until end of line
		MultiLineC = 8			///< `/*` … `*/`
	};

	/**
	 * @brief Converts a CommentType to a human-readable string.
	 * @param t Comment type to convert.
	 * @return Literal description. Lives for the program.
	 */
	STORMBYTE_FORCE_INLINE constexpr std::string_view TypeToString(const CommentType& t) noexcept {
		switch(t) {
			case CommentType::SingleLineBash:	return "Bash like single line comment";
			case CommentType::SingleLineC:		return "C++ like single line comment";
			case CommentType::MultiLineC:		return "C/C++ like multi line comment";
			default:							return "Unknown";
		}
	}

	/**
	 * @enum ContainerType
	 * @brief Group (`{}`) or list (`[]`).
	 */
	enum class STORMBYTE_CONFIG_PUBLIC ContainerType: char {
		Group,	///< Group of named items
		List	///< List of unnamed items
	};

	/**
	 * @brief Converts a ContainerType to a string.
	 * @param t Container type to convert.
	 * @return Literal name. Lives for the program.
	 */
	STORMBYTE_FORCE_INLINE constexpr std::string_view TypeToString(const ContainerType& t) noexcept {
		switch(t) {
			case ContainerType::Group:	return "Group";
			case ContainerType::List:	return "List";
			default:					return "Unknown";
		}
	}

	/**
	 * @brief Resolves a container type from its opening character.
	 * @param start `{` or `[`.
	 * @return Container type.
	 * @throw Exception if the character is not a container opener.
	 */
	constexpr STORMBYTE_CONFIG_PUBLIC ContainerType TypeFromStartCharacter(const char& start) {
		switch(start) {
			case '{':	return ContainerType::Group;
			case '[':	return ContainerType::List;
			default:	throw Exception("Unknown start character {} for container", start);
		}
	}

	struct Integer {};	///< Tag for `As<Integer>()` → `int`.
	struct Double {};	///< Tag for `As<Double>()` → `double` (Integer promotes).
	struct Bool {};		///< Tag for `As<Bool>()` → `bool`.
	struct Text {};		///< Tag for `As<Text>()` → `StormByte::Safe::String`.
	struct Binary {};	///< Tag for `As<Binary>()` → `StormByte::BinaryData`.
}
