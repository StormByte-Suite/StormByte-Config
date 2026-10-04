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

#include <StormByte/exception.hxx>
#include <StormByte/config/visibility.h>

#include <string>
#include <string_view>
#include <utility>

/**
 * @brief Config module of the StormByte suite.
 */
namespace StormByte::Config {
	/**
	 * @class Exception
	 * @brief Root exception for Config. `what()` is `StormByte.Config: message`.
	 */
	class STORMBYTE_CONFIG_PUBLIC Exception: public StormByte::Exception {
		public:
			/**
			 * @brief Format under `StormByte.Config`.
			 * @tparam Args Format argument types.
			 * @param fmt Format string.
			 * @param args Format arguments.
			 */
			template <typename... Args>
			explicit Exception(std::format_string<Args...> fmt, Args&&... args)
				: StormByte::Exception(StormByte::Exception::Path{"Config"}, fmt, std::forward<Args>(args)...) {}

			/**
			 * @brief Format a plain message under `StormByte.Config`.
			 * @param message Exception text. Not a format string.
			 */
			STORMBYTE_FORCE_INLINE explicit Exception(std::string message)
				: StormByte::Exception(StormByte::Exception::Path{"Config"}, "{}", std::move(message)) {}

			/**
			 * @brief Plain message under an explicit path.
			 * @param path Segments under `StormByte`. Not a format string.
			 * @param message Exception text. Not a format string.
			 *
			 * `StormByte::Exception::Path` is protected, so callers pass the
			 * segments as a view. The view only needs to live for this call.
			 */
			STORMBYTE_FORCE_INLINE Exception(std::string_view path, std::string message)
				: StormByte::Exception(StormByte::Exception::Path{path}, "{}", std::move(message)) {}

			/**
			 * @brief Copy constructor.
			 * @param other Exception to copy.
			 */
			Exception(const Exception& other) = default;

			/**
			 * @brief Move constructor.
			 * @param other Exception to move.
			 */
			Exception(Exception&& other) noexcept = default;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~Exception() noexcept override;

			/**
			 * @brief Copy assignment operator.
			 * @param other Exception to copy.
			 * @return Reference to this Exception.
			 */
			Exception& operator=(const Exception& other) = default;

			/**
			 * @brief Move assignment operator.
			 * @param other Exception to move.
			 * @return Reference to this Exception.
			 */
			Exception& operator=(Exception&& other) noexcept = default;
	};

	/**
	 * @class WrongValueTypeConversion
	 * @brief Thrown when a value is converted to an incorrect type.
	 */
	class STORMBYTE_CONFIG_PUBLIC WrongValueTypeConversion final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~WrongValueTypeConversion() noexcept override;
	};

	/**
	 * @class ValueFailure
	 * @brief Thrown when setting a value fails due to type mismatch.
	 */
	class STORMBYTE_CONFIG_PUBLIC ValueFailure final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~ValueFailure() noexcept override;
	};

	/**
	 * @class InvalidName
	 * @brief Thrown when an invalid name is used for an item.
	 */
	class STORMBYTE_CONFIG_PUBLIC InvalidName final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~InvalidName() noexcept override;
	};

	/**
	 * @class InvalidPath
	 * @brief Thrown when an invalid path is used for an item.
	 */
	class STORMBYTE_CONFIG_PUBLIC InvalidPath final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~InvalidPath() noexcept override;
	};

	/**
	 * @class ParseError
	 * @brief Thrown when a parsing error occurs in a configuration file.
	 */
	class STORMBYTE_CONFIG_PUBLIC ParseError final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~ParseError() noexcept override;
	};

	/**
	 * @class ItemNotFound
	 * @brief Thrown when an item cannot be found by its path or name.
	 */
	class STORMBYTE_CONFIG_PUBLIC ItemNotFound final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~ItemNotFound() noexcept override;
	};

	/**
	 * @class ItemAlreadyExists
	 * @brief Thrown when attempting to add an item that already exists.
	 */
	class STORMBYTE_CONFIG_PUBLIC ItemAlreadyExists final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~ItemAlreadyExists() noexcept override;
	};

	/**
	 * @class ItemNameAlreadyExists
	 * @brief Thrown when an item's name conflicts with an existing name.
	 */
	class STORMBYTE_CONFIG_PUBLIC ItemNameAlreadyExists final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~ItemNameAlreadyExists() noexcept override;
	};

	/**
	 * @class OutOfBounds
	 * @brief Thrown when an index is out of bounds.
	 */
	class STORMBYTE_CONFIG_PUBLIC OutOfBounds final: public Exception {
		public:
			using Exception::Exception;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~OutOfBounds() noexcept override;
	};
}
