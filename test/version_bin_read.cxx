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

/**************************** WARNING: *******************************/
/**** NEVER fix the comparisons on this test if the test fails    ****/
/**** because the test is intented to test backward compatibility ****/
/**** with older binary versions. If something fails fix reader   ****/
/*********************************************************************/

#include <StormByte/config/config.hxx>
#include <StormByte/test_handlers.h>

#include <fstream>
#include <iostream>

using namespace StormByte::Config;

// -------------------
// Version
// -------------------

int test_version_1_1_0_matches_text() {
	int result = 0;
	const auto dir = CurrentFileDirectory / "version";
	const auto text_path = dir / "config.cfg";
	const auto bin_path = dir / "config_v1.1.0.bin";

	std::ifstream text_in(text_path);
	if (!text_in) {
		std::cerr << "cannot open " << text_path << std::endl;
		RETURN_TEST("test_version_1_1_0_matches_text", 1);
	}

	Config from_text;
	try {
		text_in >> from_text;
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		RETURN_TEST("test_version_1_1_0_matches_text", 1);
	}

	std::ifstream bin_in(bin_path, std::ios::binary);
	if (!bin_in) {
		std::cerr << "cannot open " << bin_path << std::endl;
		RETURN_TEST("test_version_1_1_0_matches_text", 1);
	}

	auto from_bin = Config::Load(bin_in, Mode::Binary);
	if (!from_bin) {
		std::cerr << from_bin.error()->what() << std::endl;
		RETURN_TEST("test_version_1_1_0_matches_text", 1);
	}

	ASSERT_EQUAL("test_version_1_1_0_matches_text", from_text, from_bin.value());
	RETURN_TEST("test_version_1_1_0_matches_text", result);
}

int main() {
	int result = 0;

	// -------------------
	// Version
	// -------------------
	result += test_version_1_1_0_matches_text();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
