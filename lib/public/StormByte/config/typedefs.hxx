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

#include <StormByte/exception.hxx>
#include <StormByte/expected.hxx>

/**
 * @brief Config module of the StormByte suite.
 */
namespace StormByte::Config {
	class Config; ///< Forward declaration of the configuration document.

	/**
	 * @enum OnExistingAction
	 * @brief Specifies the action to take when a name conflict occurs.
	 */
	enum class OnExistingAction: unsigned short {
		Keep,			///< Retain the existing item.
		Overwrite,		///< Replace the existing item with the new one.
		ThrowException	///< Throw an exception to indicate the conflict.
	};

	/**
	 * @enum Mode
	 * @brief Serialization / deserialization mode for configuration documents.
	 *
	 * Selects whether a document is exchanged as human-readable text (the classic
	 * StormByte config syntax) or as the versioned binary wire format.
	 *
	 * @see Config::Save
	 * @see Config::Load
	 */
	enum class Mode: unsigned short {
		Text,   ///< Human-readable configuration syntax (UTF-8 text).
		Binary  ///< Versioned binary format (magic + format version + payload).
	};

	/**
	 * @brief Result of loading a configuration document (text or binary).
	 *
	 * On failure, the error is a @ref StormByte::Exception derivative
	 * (for example @ref ParseError or @ref StormByte::DeserializeError).
	 */
	using ExpectedConfig = StormByte::Expected<Config, StormByte::Exception>;
}
