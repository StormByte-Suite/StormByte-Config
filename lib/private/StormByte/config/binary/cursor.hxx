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

#include <StormByte/config/binary/typedefs.hxx>
#include <StormByte/config/visibility.h>
#include <StormByte/serializable.hxx>

#include <cstddef>
#include <utility>

/**
 * @namespace StormByte::Config::Binary
 * @brief Versioned binary wire format for configuration documents.
 */
namespace StormByte::Config::Binary {
	/**
	 * @class Cursor
	 * @brief Read position over one payload.
	 *
	 * Knows nothing about format versions. A versioned reader advances it
	 * with Take. The caller keeps its own "not enough bytes" message when
	 * the format had one; Take itself only forwards Serializable.
	 */
	class STORMBYTE_CONFIG_PRIVATE Cursor {
		public:
			/**
			 * @brief Start at the first byte of @p data.
			 * @param data Bytes to read. Must outlive the cursor.
			 */
			explicit Cursor(BufferView data) noexcept: m_data(data), m_offset(0) {}

			/**
			 * @brief Decode one @p T and advance past it.
			 * @tparam T Type known to Serializable.
			 * @return The value, or the error Serializable reported.
			 */
			template<typename T>
			Expected<T, DeserializeError> Take() {
				auto value = Serializable<T>::Deserialize(m_data.subspan(m_offset));
				if (!value)
					return Unexpected(value.error());

				m_offset += Serializable<T>::Size(value.value());
				return std::move(value.value());
			}

			/**
			 * @brief Bytes this cursor reads.
			 * @return The original view.
			 */
			BufferView data() const noexcept {
				return m_data;
			}

			/**
			 * @brief Current position.
			 * @return Offset from the start of @ref data.
			 */
			std::size_t offset() const noexcept {
				return m_offset;
			}

			/**
			 * @brief Move the position.
			 * @param offset New offset from the start of @ref data.
			 */
			void offset(std::size_t offset) noexcept {
				m_offset = offset;
			}

			/**
			 * @brief Bytes not yet consumed.
			 * @return `data.size() - offset`.
			 */
			std::size_t remaining() const noexcept {
				return m_data.size() - m_offset;
			}

		private:
			BufferView m_data;		///< Bytes being read.
			std::size_t m_offset;	///< Next byte to decode.
	};
}
