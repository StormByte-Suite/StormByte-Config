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

#include <StormByte/safe/clonable.hxx>
#include <StormByte/config/visibility.h>

/**
 * @brief Config module of the StormByte suite.
 */
namespace StormByte::Config {
	namespace Item {
		class Group;
	}

	/**
	 * @class ReadHook
	 * @brief Stateful hook run before/after a successful text read.
	 *
	 * Derive and construct with `MakePointer` when the hook needs state.
	 * Stateless callbacks use the function-pointer overloads on `Config`.
	 */
	class STORMBYTE_CONFIG_PUBLIC ReadHook: public StormByte::Safe::Clonable<ReadHook, StormByte::Safe::Shared<ReadHook>> {
		public:
			/**
			 * @brief Constructor
			 */
			ReadHook() noexcept = default;

			/**
			 * @brief Copy constructor
			 * @param hook	hook to copy
			 */
			ReadHook(const ReadHook& hook) noexcept = default;

			/**
			 * @brief Move constructor
			 * @param hook	hook to move
			 */
			ReadHook(ReadHook&& hook) noexcept = default;

			/**
			 * @brief Assignment operator
			 * @param hook	hook to copy
			 * @return		this
			 */
			ReadHook& operator=(const ReadHook& hook) noexcept = default;

			/**
			 * @brief Move assignment operator
			 * @param hook	hook to move
			 * @return		this
			 */
			ReadHook& operator=(ReadHook&& hook) noexcept = default;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~ReadHook() noexcept override;

			/**
			 * @brief Clone
			 * @return	pointer
			 */
			PointerType Clone() const override = 0;

			/**
			 * @brief Move
			 * @return	pointer
			 */
			PointerType Move() override = 0;

			/**
			 * @brief Invoke the hook.
			 * @param root	Document root.
			 */
			virtual void operator()(Item::Group& root) = 0;
	};

	/**
	 * @class FailureHook
	 * @brief Stateful hook run when text parse fails.
	 *
	 * Return `false` to swallow the error; `true` to keep throwing.
	 */
	class STORMBYTE_CONFIG_PUBLIC FailureHook: public StormByte::Safe::Clonable<FailureHook, StormByte::Safe::Shared<FailureHook>> {
		public:
			/**
			 * @brief Constructor
			 */
			FailureHook() noexcept = default;

			/**
			 * @brief Copy constructor
			 * @param hook	hook to copy
			 */
			FailureHook(const FailureHook& hook) noexcept = default;

			/**
			 * @brief Move constructor
			 * @param hook	hook to move
			 */
			FailureHook(FailureHook&& hook) noexcept = default;

			/**
			 * @brief Assignment operator
			 * @param hook	hook to copy
			 * @return		this
			 */
			FailureHook& operator=(const FailureHook& hook) noexcept = default;

			/**
			 * @brief Move assignment operator
			 * @param hook	hook to move
			 * @return		this
			 */
			FailureHook& operator=(FailureHook&& hook) noexcept = default;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			~FailureHook() noexcept override;

			/**
			 * @brief Clone
			 * @return	pointer
			 */
			PointerType Clone() const override = 0;

			/**
			 * @brief Move
			 * @return	pointer
			 */
			PointerType Move() override = 0;

			/**
			 * @brief Invoke the hook.
			 * @param root	Document root (possibly partial).
			 * @return		`true` to propagate the parse error.
			 */
			virtual bool operator()(const Item::Group& root) = 0;
	};

	/**
	 * @brief Function pointer for a stateless read hook.
	 */
	using ReadHookFunction = void (*)(Item::Group&);

	/**
	 * @brief Function pointer for a stateless failure hook.
	 */
	using FailureHookFunction = bool (*)(const Item::Group&);

	/**
	 * @brief Wrap a function pointer in a `ReadHook` allocated on Base's heap.
	 * @param function	stateless callback
	 * @return			owning pointer
	 */
	STORMBYTE_CONFIG_PUBLIC StormByte::Safe::Shared<ReadHook> MakeReadHook(ReadHookFunction function);

	/**
	 * @brief Wrap a function pointer in a `FailureHook` allocated on Base's heap.
	 * @param function	stateless callback
	 * @return			owning pointer
	 */
	STORMBYTE_CONFIG_PUBLIC StormByte::Safe::Shared<FailureHook> MakeFailureHook(FailureHookFunction function);
}
