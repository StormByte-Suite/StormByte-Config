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

#include <StormByte/config/hook.hxx>
#include <StormByte/config/item/group.hxx>

using namespace StormByte::Config;

ReadHook::~ReadHook() noexcept = default;

FailureHook::~FailureHook() noexcept = default;

namespace {
	class FunctionReadHook final: public ReadHook {
		public:
			explicit FunctionReadHook(ReadHookFunction function) noexcept: m_function(function) {}

			FunctionReadHook(const FunctionReadHook& hook) noexcept = default;

			FunctionReadHook(FunctionReadHook&& hook) noexcept = default;

			FunctionReadHook& operator=(const FunctionReadHook& hook) noexcept = default;

			FunctionReadHook& operator=(FunctionReadHook&& hook) noexcept = default;

			~FunctionReadHook() noexcept override = default;

			PointerType Clone() const override {
				return MakePointer<FunctionReadHook>(*this);
			}

			PointerType Move() override {
				return MakePointer<FunctionReadHook>(std::move(*this));
			}

			void operator()(Item::Group& root) override {
				if (m_function)
					m_function(root);
			}

		private:
			ReadHookFunction m_function;
	};

	class FunctionFailureHook final: public FailureHook {
		public:
			explicit FunctionFailureHook(FailureHookFunction function) noexcept: m_function(function) {}

			FunctionFailureHook(const FunctionFailureHook& hook) noexcept = default;

			FunctionFailureHook(FunctionFailureHook&& hook) noexcept = default;

			FunctionFailureHook& operator=(const FunctionFailureHook& hook) noexcept = default;

			FunctionFailureHook& operator=(FunctionFailureHook&& hook) noexcept = default;

			~FunctionFailureHook() noexcept override = default;

			PointerType Clone() const override {
				return MakePointer<FunctionFailureHook>(*this);
			}

			PointerType Move() override {
				return MakePointer<FunctionFailureHook>(std::move(*this));
			}

			bool operator()(const Item::Group& root) override {
				if (!m_function)
					return true;
				return m_function(root);
			}

		private:
			FailureHookFunction m_function;
	};
}

StormByte::Safe::Shared<ReadHook> StormByte::Config::MakeReadHook(ReadHookFunction function) {
	return ReadHook::MakePointer<FunctionReadHook>(function);
}

StormByte::Safe::Shared<FailureHook> StormByte::Config::MakeFailureHook(FailureHookFunction function) {
	return FailureHook::MakePointer<FunctionFailureHook>(function);
}
