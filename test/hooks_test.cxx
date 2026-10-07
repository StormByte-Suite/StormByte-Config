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

#include <StormByte/config/config.hxx>
#include <StormByte/config/hook.hxx>
#include <StormByte/test_handlers.h>

#include <iostream>
#include <string>

using namespace StormByte::Config;

namespace {
	int g_after = 0;
	int g_before = 0;
	int g_fail = 0;
	int g_state = 0;

	void AfterMark(StormByte::Safe::Shared<Item::Group> root) {
		++g_after;
		root->Add(Item::Value("hooked", true));
	}

	void BeforeClear(StormByte::Safe::Shared<Item::Group> root) {
		++g_before;
		root->Clear();
	}

	bool KeepFailure(StormByte::Safe::Shared<const Item::Group>) {
		++g_fail;
		return true;
	}

	bool SwallowFailure(StormByte::Safe::Shared<const Item::Group>) {
		++g_fail;
		return false;
	}
}

// -------------------
// Failure
// -------------------

int test_failure_swallow() {
	int result = 0;
	g_fail = 0;
	Config cfg;
	cfg.OnParseFailure(MakeFailureHook(&SwallowFailure));
	try {
		cfg << std::string("= broken\n");
	} catch (const Exception&) {
		result = 1;
	}
	ASSERT_EQUAL(1, g_fail);
	RETURN_TEST(result);
}

int test_failure_throw() {
	int result = 0;
	g_fail = 0;
	Config cfg;
	cfg.OnParseFailure(MakeFailureHook(&KeepFailure));
	try {
		cfg << std::string("= broken\n");
		result = 1;
	} catch (const Exception&) {
		result = 0;
	}
	ASSERT_EQUAL(1, g_fail);
	RETURN_TEST(result);
}

// -------------------
// Function
// -------------------

int test_function_before_after() {
	int result = 0;
	g_before = 0;
	g_after = 0;
	Config cfg;
	cfg.AddHookBeforeRead(MakeReadHook(&BeforeClear));
	cfg.AddHookAfterRead(MakeReadHook(&AfterMark));
	try {
		cfg << std::string("alpha = 1\n");
		ASSERT_EQUAL(1, g_before);
		ASSERT_EQUAL(1, g_after);
		ASSERT_TRUE(cfg.Exists("hooked"));
		ASSERT_TRUE(static_cast<bool>(cfg["hooked"].As<Item::Bool>()));
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}
	RETURN_TEST(result);
}

// -------------------
// Stateful
// -------------------

int test_stateful_after_read() {
	int result = 0;
	g_state = 0;
	Config cfg;
	auto callback = MakeReadHook([&counter = g_state](StormByte::Safe::Shared<Item::Group> root) {
		++counter;
		root->Add(Item::Value("stateful", 1));
	});
	ReadHook callback_copy = callback;
	ASSERT_TRUE(callback_copy.HasValue());
	auto copied_root = StormByte::Safe::MakeShared<Item::Group>();
	ASSERT_EQUAL(StormByte::Safe::Status::Success, callback_copy.Call(copied_root));
	ASSERT_TRUE(copied_root->Exists("stateful"));
	ASSERT_EQUAL(1, g_state);
	cfg.AddHookAfterRead(std::move(callback));
	try {
		cfg << std::string("alpha = 7\n");
		ASSERT_EQUAL(2, g_state);
		ASSERT_TRUE(cfg.Exists("stateful"));
		ASSERT_EQUAL(1, cfg["stateful"].As<Item::Integer>());
	} catch (const Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}
	RETURN_TEST(result);
}

int main() {
	int result = 0;

	// -------------------
	// Failure
	// -------------------
	result += test_failure_swallow();
	result += test_failure_throw();

	// -------------------
	// Function
	// -------------------
	result += test_function_before_after();

	// -------------------
	// Stateful
	// -------------------
	result += test_stateful_after_read();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
