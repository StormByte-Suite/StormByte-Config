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

#include <StormByte/config/item/comment.hxx>
#include <StormByte/safe/string.hxx>

#include <sstream>
#include <string>
#include <string_view>
#include <typeinfo>

using namespace StormByte::Config::Item;

template<CommentType T>
Comment<T>::Comment(const StormByte::Safe::String& comment): Base(), m_text(comment) {}

template<CommentType T>
Comment<T>::Comment(StormByte::Safe::String&& comment): Base(), m_text(std::move(comment)) {}

template<CommentType T>
Comment<T>::Comment(const char* comment): Base(), m_text(StormByte::Safe::String(comment)) {}

template<CommentType T>
Comment<T>::Comment(const Comment& base) = default;

template<CommentType T>
Comment<T>::Comment(Comment&& base) noexcept = default;

template<CommentType T>
Comment<T>& Comment<T>::operator=(const Comment& base) = default;

template<CommentType T>
Comment<T>& Comment<T>::operator=(Comment&& base) noexcept = default;

template<CommentType T>
typename Comment<T>::PointerType Comment<T>::Clone() const {
	return MakePointer<Comment<T>>(*this);
}

template<CommentType T>
typename Comment<T>::PointerType Comment<T>::Move() {
	return MakePointer<Comment<T>>(std::move(*this));
}

namespace {
	StormByte::Safe::String Indent(const int indent_level) {
		const int level = indent_level > 0 ? indent_level : 0;
		return StormByte::Safe::String(std::string(static_cast<std::size_t>(level), '\t'));
	}

	template<CommentType T>
	bool CommentEquals(const Comment<T>& self, const Base& other) noexcept {
		if (typeid(other) != typeid(Comment<T>))
			return false;
		if (self.Name() != other.Name())
			return false;
		return self.Text() == static_cast<const Comment<T>&>(other).Text();
	}
}

namespace StormByte::Config::Item {
	template<>
	Comment<CommentType::SingleLineBash>::~Comment() noexcept = default;

	template<>
	Comment<CommentType::SingleLineC>::~Comment() noexcept = default;

	template<>
	Comment<CommentType::MultiLineC>::~Comment() noexcept = default;

	template<>
	bool Comment<CommentType::SingleLineBash>::Equals(const Base& other) const noexcept {
		return CommentEquals(*this, other);
	}

	template<>
	bool Comment<CommentType::SingleLineC>::Equals(const Base& other) const noexcept {
		return CommentEquals(*this, other);
	}

	template<>
	bool Comment<CommentType::MultiLineC>::Equals(const Base& other) const noexcept {
		return CommentEquals(*this, other);
	}

	template<>
	StormByte::Safe::String Comment<CommentType::SingleLineBash>::Serialize(const int& indent_level) const noexcept {
		std::string out = static_cast<std::string>(Indent(indent_level));
		out += '#';
		out += static_cast<std::string_view>(m_text);
		return StormByte::Safe::String(std::string_view(out));
	}

	template<>
	StormByte::Safe::String Comment<CommentType::SingleLineC>::Serialize(const int& indent_level) const noexcept {
		std::string out = static_cast<std::string>(Indent(indent_level));
		out += "//";
		out += static_cast<std::string_view>(m_text);
		return StormByte::Safe::String(std::string_view(out));
	}

	template<>
	StormByte::Safe::String Comment<CommentType::MultiLineC>::Serialize(const int& indent_level) const noexcept {
		std::stringstream ss{ std::string(static_cast<std::string_view>(m_text)) };
		std::string item;
		std::string serial = static_cast<std::string>(Indent(indent_level));
		serial += "/*";
		std::getline(ss, item);
		serial += item;
		if (!ss.eof()) {
			serial += "\n";
			while (std::getline(ss, item)) {
				serial += item;
				if (!ss.eof())
					serial += "\n";
			}
		}
		serial += "*/";
		return StormByte::Safe::String(std::string_view(serial));
	}

	template class STORMBYTE_CONFIG_INSTANTIATE Comment<CommentType::SingleLineBash>;
	template class STORMBYTE_CONFIG_INSTANTIATE Comment<CommentType::SingleLineC>;
	template class STORMBYTE_CONFIG_INSTANTIATE Comment<CommentType::MultiLineC>;
}
