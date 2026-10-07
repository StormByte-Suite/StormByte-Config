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

/**
 * @namespace StormByte::Config::Binary
 * @brief Versioned binary wire format for configuration documents.
 */
namespace StormByte::Config::Binary {
	/**
	 * @class Writer
	 * @brief Serializes a @ref Config to the current binary format.
	 *
	 * Always writes @ref Magic and @ref CurrentVersion. Friend of @ref Config
	 * to access document internals without expanding the public API.
	 */
	class STORMBYTE_CONFIG_PRIVATE Writer {
		public:
			/**
			 * @brief Construct a writer for @p config.
			 * @param config Document to serialize (must outlive the writer).
			 */
			explicit Writer(const Config& config) noexcept;

			Writer(const Writer&) = delete;
			Writer(Writer&&) noexcept = delete;
			~Writer() noexcept = default;

			Writer& operator=(const Writer&) = delete;
			Writer& operator=(Writer&&) noexcept = delete;

			/**
			 * @brief Emit magic + CurrentVersion + payload.
			 * @return Binary buffer.
			 */
			Buffer Serialize() const noexcept;

		private:
			const Config& m_config; ///< Document being serialized.
	};
}
