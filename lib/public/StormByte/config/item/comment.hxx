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

#include <StormByte/config/item/base.hxx>
#include <StormByte/config/visibility.h>
#include <StormByte/safe/string.hxx>

#include <string_view>
#include <utility>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @namespace StormByte::Config
	 * @brief Config module of the StormByte suite.
	 */
	namespace Config {
		/**
		 * @namespace StormByte::Config::Item
		 * @brief Configuration document items.
		 */
		namespace Item {
			/**
			 * @class Comment
			 * @brief Comment item using Bash, C++ line, or C/C++ block syntax.
		 * @tparam T Comment syntax (`CommentType`).
		 */
			template<CommentType T>
			class STORMBYTE_CONFIG_PUBLIC Comment final: public Base {
				public:
					static constexpr bool CommentTag = true; ///< Identifies this as a comment item.

					/**
					 * @brief Constructs a Comment with a Base-owned string.
					 * @param comment The comment string.
				 */
					Comment(const StormByte::Safe::String& comment);

					/**
					 * @brief Constructs a Comment by moving a Base-owned string.
					 * @param comment The comment string.
				 */
					Comment(StormByte::Safe::String&& comment);

					/**
					 * @brief Constructs a Comment from a NUL-terminated string.
					 * @param comment Comment text.
					 */
					Comment(const char* comment);

					/**
					 * @brief Copy constructor.
					 * @param comment Comment to copy.
					 */
					Comment(const Comment& comment);

					/**
					 * @brief Move constructor.
					 * @param comment Comment to move.
					 */
					Comment(Comment&& comment) noexcept;

					/**
					 * @brief Copy assignment operator.
					 * @param comment Comment to copy.
					 * @return Reference to this Comment.
					 */
					Comment& operator=(const Comment& comment);

					/**
					 * @brief Move assignment operator.
					 * @param comment Comment to move.
					 * @return Reference to this Comment.
					 */
					Comment& operator=(Comment&& comment) noexcept;

					/**
					 * @brief Destructor, defined in the Config module.
				 */
					~Comment() noexcept override;

					/**
					 * @brief Polymorphic equality comparison.
					 * @param other The other item.
					 * @return True when type, name, and text match.
					 */
					bool Equals(const Base& other) const noexcept override;

					/**
					 * @brief Serializes the comment item.
					 * @param indent_level Indentation level.
					 * @return Serialized text.
					 */
					StormByte::Safe::String Serialize(const int& indent_level) const noexcept override;

					/**
					 * @brief Gets the item type.
					 * @return Item type.
					 */
					constexpr Item::Type Type() const noexcept override {
						return Item::Type::Comment;
					}

					/**
					 * @brief Gets the comment syntax.
					 * @return Comment syntax.
					 */
					constexpr StormByte::Config::Item::CommentType CommentType() const noexcept {
						return T;
					}

					/**
					 * @brief Converts comment syntax to its text representation.
					 * @return Comment syntax string.
					 */
					STORMBYTE_FORCE_INLINE constexpr std::string_view CommentTypeToString() const noexcept {
						return Item::TypeToString(T);
					}

					/**
					 * @brief Gets mutable comment text.
					 * @return Comment text.
					 */
					StormByte::Safe::String& Text() noexcept {
						return m_text;
					}

					/**
					 * @brief Gets const comment text.
					 * @return Comment text.
					 */
					const StormByte::Safe::String& Text() const noexcept {
						return m_text;
					}

					/**
					 * @brief Clones the comment on Base's heap.
					 * @return Cloned comment pointer.
					 */
					PointerType Clone() const override;

					/**
					 * @brief Moves the comment to Base's heap.
					 * @return Moved comment pointer.
					 */
					PointerType Move() override;

				private:
					StormByte::Safe::String m_text; ///< Comment text.
			};

			/**
			 * @brief Destructor for Bash single-line comments.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC Comment<CommentType::SingleLineBash>::~Comment() noexcept;

			/**
			 * @brief Destructor for C single-line comments.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC Comment<CommentType::SingleLineC>::~Comment() noexcept;

			/**
			 * @brief Destructor for C multi-line comments.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC Comment<CommentType::MultiLineC>::~Comment() noexcept;

			/**
			 * @brief Equality for Bash single-line comments.
			 * @param other The other item.
			 * @return True when both comments match.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC bool Comment<CommentType::SingleLineBash>::Equals(const Base& other) const noexcept;

			/**
			 * @brief Equality for C single-line comments.
			 * @param other The other item.
			 * @return True when both comments match.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC bool Comment<CommentType::SingleLineC>::Equals(const Base& other) const noexcept;

			/**
			 * @brief Equality for C multi-line comments.
			 * @param other The other item.
			 * @return True when both comments match.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC bool Comment<CommentType::MultiLineC>::Equals(const Base& other) const noexcept;

			/**
			 * @brief Serializes a Bash single-line comment.
			 * @param indent_level Indentation level.
			 * @return Serialized comment.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC StormByte::Safe::String Comment<CommentType::SingleLineBash>::Serialize(const int& indent_level) const noexcept;

			/**
			 * @brief Serializes a C single-line comment.
			 * @param indent_level Indentation level.
			 * @return Serialized comment.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC StormByte::Safe::String Comment<CommentType::SingleLineC>::Serialize(const int& indent_level) const noexcept;

			/**
			 * @brief Serializes a C multi-line comment.
			 * @param indent_level Indentation level.
			 * @return Serialized comment.
			 */
			template<>
			STORMBYTE_CONFIG_PUBLIC StormByte::Safe::String Comment<CommentType::MultiLineC>::Serialize(const int& indent_level) const noexcept;

			extern template class STORMBYTE_CONFIG_PUBLIC Comment<CommentType::SingleLineBash>;
			extern template class STORMBYTE_CONFIG_PUBLIC Comment<CommentType::SingleLineC>;
			extern template class STORMBYTE_CONFIG_PUBLIC Comment<CommentType::MultiLineC>;
		}
	}
}

STORMBYTE_DECLARE_MAYBE_SAFE(StormByte::Config::Item::Comment<StormByte::Config::Item::CommentType::SingleLineBash>);
STORMBYTE_DECLARE_MAYBE_SAFE(StormByte::Config::Item::Comment<StormByte::Config::Item::CommentType::SingleLineC>);
STORMBYTE_DECLARE_MAYBE_SAFE(StormByte::Config::Item::Comment<StormByte::Config::Item::CommentType::MultiLineC>);
