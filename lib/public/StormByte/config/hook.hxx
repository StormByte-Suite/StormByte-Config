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

#include <StormByte/config/item/group.hxx>
#include <StormByte/safe/function.hxx>

#include <type_traits>
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
		 * @brief Safe read callback receiving a Base-owned mutable group handle.
		 */
		using ReadHook = StormByte::Safe::Function<void(StormByte::Safe::Shared<Item::Group>)>;

		/**
		 * @brief Safe parse-failure callback receiving a Base-owned read-only group handle.
		 */
		using FailureHook = StormByte::Safe::Function<bool(StormByte::Safe::Shared<const Item::Group>)>;

		/**
		 * @brief Creates a read hook whose callable context is cloned and released by its creator.
		 * @tparam Callable Copy-constructible callable accepting a shared mutable group handle.
		 * @param callable Callback invoked before/after text parsing.
		 * @return Safe callback value suitable for storing or copying across a DLL boundary.
		 */
		template<typename Callable>
		requires StormByte::Type::CopyConstructible<std::remove_cvref_t<Callable>>
		ReadHook MakeReadHook(Callable&& callable) {
			using CallableType = std::remove_cvref_t<Callable>;
			auto* context = new CallableType(std::forward<Callable>(callable));
			try {
				return ReadHook(
					context,
					[](void* state, StormByte::Safe::Shared<Item::Group> root) {
						(*static_cast<CallableType*>(state))(std::move(root));
						return StormByte::Safe::Status::Success;
					},
					[](const void* state) noexcept -> void* {
						try {
							return new CallableType(*static_cast<const CallableType*>(state));
						} catch (...) {
							return nullptr;
						}
					},
					[](void* state) noexcept {
						delete static_cast<CallableType*>(state);
					});
			} catch (...) {
				delete context;
				throw;
			}
		}

		/**
		 * @brief Creates a failure hook whose callable context is cloned and released by its creator.
		 * @tparam Callable Copy-constructible callable accepting a shared const group handle and returning bool.
		 * @param callable Callback invoked with the partially parsed group.
		 * @return Safe callback value suitable for storing or copying across a DLL boundary.
		 */
		template<typename Callable>
		requires StormByte::Type::CopyConstructible<std::remove_cvref_t<Callable>>
		FailureHook MakeFailureHook(Callable&& callable) {
			using CallableType = std::remove_cvref_t<Callable>;
			auto* context = new CallableType(std::forward<Callable>(callable));
			try {
				return FailureHook(
					context,
					[](void* state, bool* result, StormByte::Safe::Shared<const Item::Group> root) {
						*result = (*static_cast<CallableType*>(state))(std::move(root));
						return StormByte::Safe::Status::Success;
					},
					[](const void* state) noexcept -> void* {
						try {
							return new CallableType(*static_cast<const CallableType*>(state));
						} catch (...) {
							return nullptr;
						}
					},
					[](void* state) noexcept {
						delete static_cast<CallableType*>(state);
					});
			} catch (...) {
				delete context;
				throw;
			}
		}
	}
}
