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
#include <StormByte/safe/string.hxx>
#include <StormByte/test_handlers.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

using namespace StormByte::Config;
using StormByte::Safe::String;

namespace {
	ExpectedConfig LoadBinaryFile(const std::filesystem::path& path) {
		std::ifstream in(path, std::ios::binary);
		if (!in) {
			std::cerr << "cannot open " << path << std::endl;
			return StormByte::Unexpected<StormByte::DeserializeError>("cannot open file");
		}

		return Config::Load(in, Mode::Binary);
	}
}

// -------------------
// Golden
// -------------------

int test_binary_accepts_golden() {
	const auto path = CurrentFileDirectory / "files" / "config_example.bin";
	auto loaded = LoadBinaryFile(path);
	if (!loaded) {
		std::cerr << loaded.error()->what() << std::endl;
		RETURN_TEST(1);
	}

	const auto text_path = CurrentFileDirectory / "files" / "complex_conf1.conf";
	std::ifstream text_in(text_path);
	if (!text_in) {
		std::cerr << "cannot open " << text_path << std::endl;
		RETURN_TEST(1);
	}

	Config from_text;
	text_in >> from_text;
	ASSERT_EQUAL(loaded.value(), from_text);
	RETURN_TEST(0);
}

// -------------------
// Rejection
// -------------------

int test_binary_rejects_bad_magic() {
	const auto path = CurrentFileDirectory / "files" / "config_example_bad_magic.bin";
	auto loaded = LoadBinaryFile(path);
	ASSERT_FALSE(static_cast<bool>(loaded));
	RETURN_TEST(0);
}

int test_binary_rejects_bad_version() {
	const auto path = CurrentFileDirectory / "files" / "config_example_bad_version.bin";
	auto loaded = LoadBinaryFile(path);
	ASSERT_FALSE(static_cast<bool>(loaded));
	RETURN_TEST(0);
}

int test_binary_rejects_legacy_payload() {
	const auto path = CurrentFileDirectory / "files" / "config_example_legacy.bin";
	auto loaded = LoadBinaryFile(path);
	ASSERT_FALSE(static_cast<bool>(loaded));
	RETURN_TEST(0);
}

// -------------------
// Roundtrip
// -------------------

int test_binary_save_load_roundtrip_stream() {
	Config original;
	original.Add(Item::Value("name", "StormByte"));
	original.Add(Item::Value("n", 7));
	const auto tmp = CurrentFileDirectory / "files" / "config_roundtrip_tmp.bin";
	{
		std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
		if (!out) {
			std::cerr << "cannot write " << tmp << std::endl;
			RETURN_TEST(1);
		}

		original.Save(out, Mode::Binary);
	}

	std::ifstream in(tmp, std::ios::binary);
	if (!in) {
		std::cerr << "cannot read " << tmp << std::endl;
		RETURN_TEST(1);
	}

	in.seekg(1, std::ios::beg);
	auto loaded = Config::Load(in, Mode::Binary);
	if (!loaded) {
		std::cerr << loaded.error()->what() << std::endl;
		std::remove(tmp.string().c_str());
		RETURN_TEST(1);
	}

	ASSERT_TRUE(original == loaded.value());
	const int n = loaded.value()["n"].As<Item::Integer>();
	ASSERT_EQUAL(7, n);

	std::ifstream after_eof(tmp, std::ios::binary);
	after_eof.seekg(0, std::ios::end);
	char last_byte = 0;
	after_eof.get(last_byte);
	ASSERT_TRUE(after_eof.fail());
	auto recovered = Config::Load(after_eof, Mode::Binary);
	if (!recovered) {
		std::cerr << recovered.error()->what() << std::endl;
		std::remove(tmp.string().c_str());
		RETURN_TEST(1);
	}
	ASSERT_TRUE(original == recovered.value());
	std::remove(tmp.string().c_str());
	RETURN_TEST(0);
}

int main() {
	int result = 0;

	// -------------------
	// Golden
	// -------------------
	result += test_binary_accepts_golden();

	// -------------------
	// Rejection
	// -------------------
	result += test_binary_rejects_bad_magic();
	result += test_binary_rejects_bad_version();
	result += test_binary_rejects_legacy_payload();

	// -------------------
	// Roundtrip
	// -------------------
	result += test_binary_save_load_roundtrip_stream();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
