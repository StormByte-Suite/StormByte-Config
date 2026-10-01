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

#include <StormByte/config/config.hxx>
#include <StormByte/test_handlers.h>

#include <climits>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

using namespace StormByte::Config;
using StormByte::Safe::String;

namespace {
	std::filesystem::path TempConfigFile() {
		const std::filesystem::path dir = std::filesystem::temp_directory_path();
		std::random_device device;
		for (;;) {
			const std::filesystem::path path = dir / ("config-" + std::to_string(device()) + ".conf");
			if (!std::filesystem::exists(path))
				return path;
		}
	}

	void HookAfterClear(Item::Group& root) {
		root.Clear();
	}

	bool HookSwallowFailure(const Item::Group&) {
		return false;
	}
}

// -------------------
// Comments
// -------------------

int test_comment_types() {
	int result = 0;
	Item::Comment<Item::CommentType::SingleLineBash> singleline("This is a single line comment");
	Item::Comment<Item::CommentType::MultiLineC> multiline(
		"/* This is a multi-line comment\nwhich spans multiple lines\n"
		"and ends here */"
	);
	ASSERT_EQUAL("test_comment_types", Item::TypeToString(Item::Type::Comment), singleline.TypeToString());
	ASSERT_EQUAL("test_comment_types", Item::TypeToString(Item::CommentType::SingleLineBash), singleline.CommentTypeToString());
	ASSERT_EQUAL("test_comment_types", Item::TypeToString(Item::Type::Comment), multiline.TypeToString());
	ASSERT_EQUAL("test_comment_types", Item::TypeToString(Item::CommentType::MultiLineC), multiline.CommentTypeToString());
	RETURN_TEST("test_comment_types", result);
}

int commented_config() {
	int result = 0;
	const std::filesystem::path temp_file = TempConfigFile();
	Config config;
	const std::string config_str = "# The following is a test integer\n"
		"test_integer = 666\n"
		"\n"
		"# Now a group\n"
		"test_group = { # We can have a comment here!\n"
		"\t# And also here\n"
		"\ttest_string = \"# But this is not a comment\"\n"
		"}\n"
		"# Ending comment";
	const std::string expected_str = "# The following is a test integer\n"
		"test_integer = 666\n"
		"# Now a group\n"
		"test_group = {\n"
		"\t# We can have a comment here!\n"
		"\t# And also here\n"
		"\ttest_string = \"# But this is not a comment\"\n"
		"}\n"
		"# Ending comment\n";
	config << config_str;
	std::fstream file;
	file.open(temp_file, std::ios::out);
	config >> file;
	file.close();
	const Item::Base& test_string = config["test_group/test_string"];
	ASSERT_EQUAL("commented_config", "# But this is not a comment", test_string.As<Item::Text>());
	std::ifstream temp_file_stream(temp_file);
	std::stringstream buffer;
	buffer << temp_file_stream.rdbuf();
	ASSERT_EQUAL("commented_config", expected_str, buffer.str());
	std::remove(temp_file.string().c_str());
	RETURN_TEST("commented_config", result);
}

// -------------------
// Files
// -------------------

int bad_config1() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_config1.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		result = 0;
	}

	RETURN_TEST("bad_config1", result);
}

int bad_config2() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_config2.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("bad_config2", result);
}

int bad_config3() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_config3.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		result = 0;
	}

	RETURN_TEST("bad_config3", result);
}

int good_double_conf1() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_double_conf1.conf", std::ios::in);
		cfg << file;
		file.close();
		Item::Base& lookup_double = cfg["test_double"];
		ASSERT_EQUAL("good_double_conf1", 666.666, lookup_double.As<Item::Double>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("good_double_conf1", result);
}

int good_double_conf2() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_double_conf2.conf", std::ios::in);
		cfg << file;
		file.close();
		Item::Base& lookup_test_double = cfg["test_double"];
		ASSERT_EQUAL("good_double_conf2", 19.89, lookup_test_double.As<Item::Double>());
		Item::Base& lookup_test_exp = cfg["test_exp"];
		ASSERT_EQUAL("good_double_conf2", 1.87e-6, lookup_test_exp.As<Item::Double>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("good_double_conf2", result);
}

// -------------------
// Groups
// -------------------

int test_add_remove_group() {
	int result = 0;
	Config config;
	try {
		Item::Group group("TestGroup");
		group.Add(Item::Value("GroupInt", 55));
		Item::Base& group_item = config.Add(group);
		group_item.As<Item::Group>().Remove("GroupInt");
		config["TestGroup/GroupInt"];
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		result = 0;
	}

	RETURN_TEST("test_add_remove_group", result);
}

int test_complex_config_creation() {
	int result = 0;
	const std::filesystem::path temp_file = TempConfigFile();
	Config config;
	try {
		Item::Group& group1 = config.Add(Item::Group("Group1")).As<Item::Group>();
		Item::Group& group2 = group1.Add(Item::Group("Group2")).As<Item::Group>();
		group2.Add(Item::Value("IntItem1", 123));
		group2.Add(Item::Value("StrItem1", "Nested String"));
		Item::Group& group3 = config.Add(Item::Group("Group3")).As<Item::Group>();
		group3.Add(Item::Value("IntItem2", 456));
		std::fstream file;
		file.open(temp_file, std::ios::out);
		file << config;
		file.close();
		std::ifstream temp_file_stream(temp_file);
		std::stringstream buffer;
		buffer << temp_file_stream.rdbuf();
		std::string expected_content =
			"Group1 = {\n"
			"\tGroup2 = {\n"
			"\t\tIntItem1 = 123\n"
			"\t\tStrItem1 = \"Nested String\"\n"
			"\t}\n"
			"}\n"
			"Group3 = {\n"
			"\tIntItem2 = 456\n"
			"}\n";
		ASSERT_EQUAL("test_complex_config_creation", expected_content, buffer.str());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	std::remove(temp_file.string().c_str());
	RETURN_TEST("test_complex_config_creation", result);
}

int test_nested_groups() {
	int result = 0;
	Config config;
	try {
		Item::Base& group1 = config.Add(Item::Group("Group1"));
		Item::Base& group2 = group1.As<Item::Group>().Add(Item::Group("Group2"));
		group2.As<Item::Group>().Add(Item::Value("SubTestInt", 99));
		group2.As<Item::Group>().Add(Item::Value("SubTestStr", "Sub Hello"));
		const Item::Base& lookup_int = config["Group1/Group2/SubTestInt"];
		ASSERT_EQUAL("test_nested_groups", 99, lookup_int.As<Item::Integer>());
		const Item::Base& lookup_str = config["Group1/Group2/SubTestStr"];
		ASSERT_EQUAL("test_nested_groups", "Sub Hello", lookup_str.As<Item::Text>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_nested_groups", result);
}

int test_write_nested_groups() {
	int result = 0;
	const std::filesystem::path temp_file = TempConfigFile();
	std::string config_content =
		"Group1 = {\n"
		"    Group2 = {\n"
		"        SubTestInt = 99\n"
		"        SubTestStr = \"Sub Hello\"\n"
		"    }\n"
		"}\n";
	Config config;
	try {
		config_content >> config;
		const Item::Base& lookup_int = config["Group1/Group2/SubTestInt"];
		ASSERT_EQUAL("test_write_nested_groups", 99, lookup_int.As<Item::Integer>());
		const Item::Base& lookup_str = config["Group1/Group2/SubTestStr"];
		ASSERT_EQUAL("test_write_nested_groups", "Sub Hello", lookup_str.As<Item::Text>());
		std::fstream file;
		file.open(temp_file, std::ios::out);
		file << config;
		file.close();
		file.open(temp_file, std::ios::in);
		Config config2;
		config2 << file;
		file.close();
		const Item::Base& lookup_int2 = config2["Group1/Group2/SubTestInt"];
		ASSERT_EQUAL("test_write_nested_groups", 99, lookup_int2.As<Item::Integer>());
		const Item::Base& lookup_str2 = config2["Group1/Group2/SubTestStr"];
		ASSERT_EQUAL("test_write_nested_groups", "Sub Hello", lookup_str2.As<Item::Text>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	std::remove(temp_file.string().c_str());
	RETURN_TEST("test_write_nested_groups", result);
}

// -------------------
// Values
// -------------------

int test_add_and_lookup() {
	int result = 0;
	Config config;
	config.Add(Item::Value("TestInt", 42));
	config.Add(Item::Value("TestStr", "Hello, World!"));
	try {
		const Item::Base& lookup_int = config["TestInt"];
		ASSERT_EQUAL("test_add_and_lookup", 42, lookup_int.As<Item::Integer>());
		const Item::Base& lookup_str = config["TestStr"];
		ASSERT_EQUAL("test_add_and_lookup", "Hello, World!", lookup_str.As<Item::Text>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_add_and_lookup", result);
}

int test_write_and_read() {
	int result = 0;
	const std::filesystem::path temp_file = TempConfigFile();
	std::string config_content =
		"TestInt = 42\n"
		"TestStr = \"Hello, World!\"\n";
	Config config;
	try {
		config << config_content;
		const Item::Base& int_item = config["TestInt"];
		ASSERT_EQUAL("test_write_and_read", 42, int_item.As<Item::Integer>());
		const Item::Base& str_item = config["TestStr"];
		ASSERT_EQUAL("test_write_and_read", "Hello, World!", str_item.As<Item::Text>());
		std::fstream file;
		file.open(temp_file, std::ios::out);
		file << config;
		file.close();
		file.open(temp_file, std::ios::in);
		Config config2;
		file >> config2;
		file.close();
		const Item::Base& int_item2 = config["TestInt"];
		ASSERT_EQUAL("test_write_and_read", 42, int_item2.As<Item::Integer>());
		const Item::Base& str_item2 = config["TestStr"];
		ASSERT_EQUAL("test_write_and_read", "Hello, World!", str_item2.As<Item::Text>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	std::remove(temp_file.string().c_str());
	RETURN_TEST("test_write_and_read", result);
}

int good_string_conf() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_string_conf.conf", std::ios::in);
		cfg << file;
		file.close();
		const Item::Base& lookup_string = cfg["test_string"];
		ASSERT_EQUAL("good_string_conf", "This is a test string", lookup_string.As<Item::Text>());
		const Item::Base& lookup_quoted = cfg["test_quoted"];
		ASSERT_EQUAL("good_string_conf", "This \"quote\" allows more things", lookup_quoted.As<Item::Text>());
		const Item::Base& lookup_unfinished = cfg["test_unfinished"];
		ASSERT_EQUAL("good_string_conf", "When you see a \" you might have the start of a string", lookup_unfinished.As<Item::Text>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("good_string_conf", result);
}

int test_empty_string() {
	int result = 0;
	Config config;
	config.Add(Item::Value("EmptyString", ""));
	try {
		const Item::Base& lookup_str = config["EmptyString"];
		ASSERT_EQUAL("test_empty_string", "", lookup_str.As<Item::Text>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_empty_string", result);
}

int test_integer_boundaries() {
	int result = 0;
	Config config;
	config.Add(Item::Value("MaxInt", INT_MAX));
	config.Add(Item::Value("MinInt", INT_MIN));
	try {
		const Item::Base& lookup_max_int = config["MaxInt"];
		ASSERT_EQUAL("test_integer_boundaries", INT_MAX, lookup_max_int.As<Item::Integer>());
		const Item::Base& lookup_min_int = config["MinInt"];
		ASSERT_EQUAL("test_integer_boundaries", INT_MIN, lookup_min_int.As<Item::Integer>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_integer_boundaries", result);
}

int test_special_characters_in_string() {
	int result = 0;
	Config config;
	config.Add(Item::Value("SpecialChars", "Line1\nLine2\tTabbed"));
	try {
		const Item::Base& lookup_str = config["SpecialChars"];
		ASSERT_EQUAL("test_special_characters_in_string", "Line1\nLine2\tTabbed", lookup_str.As<Item::Text>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_special_characters_in_string", result);
}

int test_deeply_nested_groups() {
	int result = 0;
	Config config;
	try {
		Item::Group& group1 = config.Add(Item::Group("Group1")).As<Item::Group>();
		Item::Group& group2 = group1.Add(Item::Group("Group2")).As<Item::Group>();
		Item::Group& group3 = group2.Add(Item::Group("Group3")).As<Item::Group>();
		Item::Group& group4 = group3.Add(Item::Group("Group4")).As<Item::Group>();
		group4.Add(Item::Value("DeepInt", 1234));
		const Item::Base& lookup_int = config["Group1/Group2/Group3/Group4/DeepInt"];
		ASSERT_EQUAL("test_deeply_nested_groups", 1234, lookup_int.As<Item::Integer>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_deeply_nested_groups", result);
}

int test_invalid_syntax() {
	int result = 0;
	Config config;
	std::string invalid_config = "Invalid = { Unclosed }";
	try {
		config << invalid_config;
		result = 1;
	} catch (const StormByte::Config::ParseError&) {
		// Expected
	}

	RETURN_TEST("test_invalid_syntax", result);
}

int test_special_characters_string() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "special_characters_conf.conf", std::ios::in);
		cfg << file;
		file.close();
		const Item::Base& lookup_special = cfg["special_string"];
		ASSERT_EQUAL("test_special_characters_string", "This is a test string with special characters: \n, \t, \\", lookup_special.As<Item::Text>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_special_characters_string", result);
}

int test_long_string() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "long_string_conf.conf", std::ios::in);
		cfg << file;
		file.close();
		const Item::Base& lookup_long = cfg["long_string"];
		const std::string expected_long(1000, 'a');
		ASSERT_EQUAL("test_long_string", String(std::string_view(expected_long)), lookup_long.As<Item::Text>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_long_string", result);
}

int test_unmatched_braces() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "unmatched_braces.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("test_unmatched_braces", result);
}

int good_boolean_config1() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_boolean_conf1.conf", std::ios::in);
		cfg << file;
		file.close();
		const Item::Base& lookup_enable_feature = cfg["settings/enable_feature"];
		ASSERT_EQUAL("good_boolean_config1", true, lookup_enable_feature.As<Item::Bool>());
		const Item::Base& lookup_enable_extra = cfg["settings/enable_extra"];
		ASSERT_EQUAL("good_boolean_config1", false, lookup_enable_extra.As<Item::Bool>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("good_boolean_config1", result);
}

int bad_boolean_config1() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_boolean_conf1.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("bad_boolean_config1", result);
}

int copy_configuration() {
	Config cfg1, cfg2;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_boolean_conf1.conf", std::ios::in);
		cfg1 << file;
		file.close();
		cfg2 = cfg1;
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		RETURN_TEST("copy_configuration", 1);
	}

	try {
		const Item::Base& lookup_enable_feature_1 = cfg1["settings/enable_feature"];
		const Item::Base& lookup_enable_feature_2 = cfg2["settings/enable_feature"];
		ASSERT_EQUAL("copy_configuration", lookup_enable_feature_1.As<Item::Bool>(), lookup_enable_feature_2.As<Item::Bool>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		RETURN_TEST("copy_configuration", 1);
	}

	RETURN_TEST("copy_configuration", 0);
}

int move_configuration() {
	int result = 0;
	Config cfg1, cfg2;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_boolean_conf1.conf", std::ios::in);
		cfg1 << file;
		file.close();
		cfg2 = std::move(cfg1);
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		RETURN_TEST("move_configuration", 1);
	}

	try {
		cfg1["settings/enable_feature"];
		RETURN_TEST("move_configuration", 1);
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	try {
		const Item::Base& lookup_enable_feature = cfg2["settings/enable_feature"];
		ASSERT_EQUAL("move_configuration", true, lookup_enable_feature.As<Item::Bool>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		RETURN_TEST("move_configuration", 1);
	}

	RETURN_TEST("move_configuration", result);
}

int duplicated_insertion() {
	int result = 0;
	Config cfg;
	try {
		cfg.Add(Item::Value("testInt", 66));
		cfg.Add(Item::Value("testInt", 66));
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("duplicated_insertion", result);
}

int on_name_clash_keep_existing() {
	int result = 0;
	Config cfg;
	cfg.OnExistingAction(OnExistingAction::Keep);
	cfg.Add(Item::Value("testItem", true));
	try {
		cfg.Add(Item::Value("testItem", 666));
		const Item::Base& item = cfg["testItem"];
		ASSERT_EQUAL("on_name_clash_keep_existing", true, item.As<Item::Bool>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("on_name_clash_keep_existing", result);
}

int on_name_clash_replace() {
	int result = 0;
	Config cfg;
	cfg.OnExistingAction(OnExistingAction::Overwrite);
	cfg.Add(Item::Value("testItem", true));
	try {
		cfg.Add(Item::Value("testItem", 66));
		const Item::Base& item = cfg["testItem"];
		ASSERT_EQUAL("on_name_clash_replace", 66, item.As<Item::Integer>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("on_name_clash_replace", result);
}

int config_to_config_output() {
	int result = 0;
	Config cfg1, cfg2;
	int value = 0;
	cfg1.Add(Item::Value("testInt", value));
	cfg2.Add(Item::Value("testString", "Hello!"));
	try {
		cfg1 << cfg2;
		const Item::Base& testInt = cfg1["testInt"];
		ASSERT_EQUAL("config_to_config_output", 0, testInt.As<Item::Integer>());
		const Item::Base& testString = cfg1["testString"];
		ASSERT_EQUAL("config_to_config_output", "Hello!", testString.As<Item::Text>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("config_to_config_output", result);
}

int config_value_reference_change() {
	int result = 0;
	Config cfg;
	cfg.Add(Item::Value("testInt", 66));
	try {
		cfg["testInt"].As<Item::Integer>() = 99;
		const Item::Base& testInt = cfg["testInt"];
		ASSERT_EQUAL("config_value_reference_change", 99, testInt.As<Item::Integer>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("config_value_reference_change", result);
}

int config_remove_full_path() {
	int result = 0;
	Config cfg;
	Item::Group& group = cfg.Add(Item::Group("testGroup")).As<Item::Group>();
	group.Add(Item::Value("testInt", 99));
	group.Add(Item::Value("testString", "Group String"));
	try {
		cfg.Remove("testGroup/testInt");
		const auto& testString = cfg["testGroup/testString"];
		ASSERT_EQUAL("config_remove_full_path", "Group String", testString.As<Item::Text>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result = 1;
	}

	try {
		cfg["testGroup/testInt"];
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("config_remove_full_path", result);
}

int config_test_add_empty_name() {
	int result = 0;
	Config cfg;
	try {
		cfg.Add(Item::Value("", 66));
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("config_test_add_empty_name", result);
}

int config_list_test() {
	Config cfg;
	cfg.Add(Item::List("testList"));
	Item::List& list = cfg["testList"].As<Item::List>();
	list.Add(Item::Comment<Item::CommentType::SingleLineBash>("List comment"));
	list.Add(Item::Value(66));
	list.Add(Item::Value("Test string"));
	cfg.Add(Item::Group("testGroup"));
	Item::Group& group = cfg["testGroup"].As<Item::Group>();
	group.Add(Item::Value("testInt", 99));
	group.Add(Item::Value("testString2", "Group String"));
	group.Add(Item::List("testList2"));
	Item::List& list2 = group["testList2"].As<Item::List>();
	list2.Add(Item::Comment<Item::CommentType::SingleLineBash>("List comment 2"));
	list2.Add(Item::Value(11));
	const std::string expected = "testList = [\n"
	"\t#List comment\n"
	"\t66\n"
	"\t\"Test string\"\n"
	"]\n"
	"testGroup = {\n"
	"\ttestInt = 99\n"
	"\ttestString2 = \"Group String\"\n"
	"\ttestList2 = [\n"
	"\t\t#List comment 2\n"
	"\t\t11\n"
	"\t]\n"
	"}\n";
	Config cfg2;
	try {
		cfg2 << cfg;
		ASSERT_EQUAL("config_list_test", expected, (std::string)cfg2);
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		RETURN_TEST("config_list_test", 1);
	}

	RETURN_TEST("config_list_test", 0);
}

int config_list_access_by_index() {
	Config cfg1;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_list_conf1.conf", std::ios::in);
		cfg1 << file;
		file.close();
		const auto& lookup_list = cfg1["testList"].As<Item::List>();
		ASSERT_EQUAL("config_list_access_by_index", 66, lookup_list[StormByte::Size{1}].As<Item::Integer>());
		const auto& lookup_list2 = cfg1["testGroup/testList2"].As<Item::List>();
		ASSERT_EQUAL("config_list_access_by_index", 11, lookup_list2[StormByte::Size{1}].As<Item::Integer>());
	} catch (const StormByte::Config::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		RETURN_TEST("config_list_access_by_index", 1);
	}

	RETURN_TEST("config_list_access_by_index", 0);
}

int complex_conf1() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "complex_conf1.conf", std::ios::in);
		cfg << file;
		file.close();
		const Item::List& lookup_testList2 = cfg["testGroup/testList2"].As<Item::List>();
		const Item::Group& group_inside_list = lookup_testList2[StormByte::Size{3}].As<Item::Group>();
		const Item::Base& lookup_testInt_inside = group_inside_list["testInt"];
		ASSERT_EQUAL("complex_conf1", 1, lookup_testInt_inside.As<Item::Integer>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("complex_conf1", result);
}

int copy_and_delete() {
	int result = 0;
	std::unique_ptr<Config> cfg(std::make_unique<Config>());
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "complex_conf1.conf", std::ios::in);
		*cfg << file;
		file.close();
		const Config cfg2(*cfg);
		cfg.reset();
		const Item::List& lookup_testList2 = cfg2["testGroup/testList2"].As<Item::List>();
		const Item::Group& group_inside_list = lookup_testList2[StormByte::Size{3}].As<Item::Group>();
		const Item::Base& lookup_testInt_inside = group_inside_list["testInt"];
		ASSERT_EQUAL("copy_and_delete", 1, lookup_testInt_inside.As<Item::Integer>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("copy_and_delete", result);
}

int complex_path_access() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "complex_conf1.conf", std::ios::in);
		cfg << file;
		file.close();
		const Item::Base& lookup_deep_into_list = cfg["testGroup/testList2/3/testList/2"];
		ASSERT_EQUAL("complex_path_access", 3, lookup_deep_into_list.As<Item::Integer>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("complex_path_access", result);
}

int good_comment_multi_conf1() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_comment_multi_conf1.conf", std::ios::in);
		cfg << file;
		file.close();
		const std::string expected =
		"# This is a comment\n"
		"testInteger = 6\n"
		"/*\n"
		" * This is a block comment\n"
		" */\n"
		"testString = \"test\"\n"
		"/* This is a comment */\n"
		"testGroup = {\n"
		"\t/**\n"
		"\t * This is a documentation comment\n"
		"\t */\n"
		"\t/* testInt = 6 */\n"
		"\ttestString = \"test2\"\n"
		"}\n";
		const std::string actual = (std::string)cfg;
		ASSERT_EQUAL("good_comment_multi_conf1", expected, actual);
	} catch (const StormByte::Config::Exception&) {
		result = 1;
	}

	RETURN_TEST("good_comment_multi_conf1", result);
}

int test_config_hooks() {
	int result = 0;
	Config cfg1;
	cfg1.AddHookAfterRead(&HookAfterClear);
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "complex_conf1.conf", std::ios::in);
		cfg1 << file;
		file.close();
		ASSERT_EQUAL("test_config_hooks", static_cast<std::size_t>(0), static_cast<std::size_t>(cfg1.Size()));
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("test_config_hooks", result);
}

int size_and_count() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "complex_conf1.conf", std::ios::in);
		cfg << file;
		file.close();
		ASSERT_EQUAL("size_and_count", static_cast<std::size_t>(4), static_cast<std::size_t>(cfg.Size()));
		ASSERT_EQUAL("size_and_count", static_cast<std::size_t>(24), static_cast<std::size_t>(cfg.Count()));
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("size_and_count", result);
}

int all_comment_types_test() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_comment_multi_conf2.conf", std::ios::in);
		cfg << file;
		file.close();
		Item::Base& number = cfg["number"];
		Item::Base& another_number = cfg["another_number"];
		ASSERT_EQUAL("all_comment_types_test", 1, number.As<Item::Integer>());
		ASSERT_EQUAL("all_comment_types_test", 2, another_number.As<Item::Integer>());
		ASSERT_EQUAL("all_comment_types_test", static_cast<std::size_t>(6), static_cast<std::size_t>(cfg.Size()));
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("all_comment_types_test", result);
}

int test_on_failure_hook() {
	int result = 0;
	Config cfg;
	cfg.OnParseFailure(&HookSwallowFailure);
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_config1.conf", std::ios::in);
		cfg << file;
		file.close();
	} catch (const StormByte::Config::Exception&) {
		result = 1;
	}

	RETURN_TEST("test_on_failure_hook", result);
}

int good_binary_simple() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_binary_simple.conf", std::ios::in);
		cfg << file;
		file.close();
		const StormByte::BinaryData& data = cfg["data"].As<Item::Binary>();
		std::string recovered(reinterpret_cast<const char*>(data.span().data()), static_cast<std::size_t>(data.size()));
		ASSERT_EQUAL("good_binary_simple", "Hello", recovered);
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("good_binary_simple", result);
}

int good_binary_mixed() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_binary_mixed.conf", std::ios::in);
		cfg << file;
		file.close();
		ASSERT_EQUAL("good_binary_mixed", "StormByte", cfg["name"].As<Item::Text>());
		ASSERT_EQUAL("good_binary_mixed", 1.2, cfg["version"].As<Item::Double>());
		ASSERT_EQUAL("good_binary_mixed", true, cfg["enabled"].As<Item::Bool>());
		const StormByte::BinaryData& payload = cfg["payload"].As<Item::Binary>();
		std::string recovered(reinterpret_cast<const char*>(payload.span().data()), static_cast<std::size_t>(payload.size()));
		ASSERT_EQUAL("good_binary_mixed", "Hello World", recovered);
		const StormByte::BinaryData& binary_key = cfg["settings/binary_key"].As<Item::Binary>();
		std::string recovered2(reinterpret_cast<const char*>(binary_key.span().data()), static_cast<std::size_t>(binary_key.size()));
		ASSERT_EQUAL("good_binary_mixed", "This is a test", recovered2);
		const auto& list = cfg["list_of_things"].As<Item::List>();
		ASSERT_EQUAL("good_binary_mixed", "first", list[StormByte::Size{0}].As<Item::Text>());
		const StormByte::BinaryData& second = list[StormByte::Size{1}].As<Item::Binary>();
		std::string recovered3(reinterpret_cast<const char*>(second.span().data()), static_cast<std::size_t>(second.size()));
		ASSERT_EQUAL("good_binary_mixed", "second", recovered3);
		ASSERT_EQUAL("good_binary_mixed", 42, list[StormByte::Size{2}].As<Item::Integer>());
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("good_binary_mixed", result);
}

int good_binary_empty() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_binary_empty.conf", std::ios::in);
		cfg << file;
		file.close();
		const StormByte::BinaryData& data = cfg["empty_data"].As<Item::Binary>();
		ASSERT_EQUAL("good_binary_empty", static_cast<std::size_t>(0), static_cast<std::size_t>(data.size()));
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("good_binary_empty", result);
}

int good_binary_nested() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "good_binary_nested.conf", std::ios::in);
		cfg << file;
		file.close();
		const StormByte::BinaryData& secret = cfg["root/level1/level2/secret"].As<Item::Binary>();
		std::string recovered(reinterpret_cast<const char*>(secret.span().data()), static_cast<std::size_t>(secret.size()));
		ASSERT_EQUAL("good_binary_nested", "SecretData", recovered);
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("good_binary_nested", result);
}

int bad_binary_invalid_base64() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_binary_invalid_base64.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("bad_binary_invalid_base64", result);
}

int bad_binary_unclosed() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_binary_unclosed.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("bad_binary_unclosed", result);
}

int bad_binary_missing_quote() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_binary_missing_quote.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("bad_binary_missing_quote", result);
}

int bad_binary_wrong_prefix() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_binary_wrong_prefix.conf", std::ios::in);
		cfg << file;
		file.close();
		const auto& data = cfg["data"].As<Item::Text>();
		ASSERT_EQUAL("bad_binary_wrong_prefix", "SGVsbG8=", data);
	} catch (const StormByte::Config::Exception& e) {
		std::cerr << e.what() << std::endl;
		result = 1;
	}

	RETURN_TEST("bad_binary_wrong_prefix", result);
}

int bad_binary_in_list_unclosed() {
	int result = 0;
	Config cfg;
	try {
		std::fstream file;
		file.open(CurrentFileDirectory / "files" / "bad_binary_in_list_unclosed.conf", std::ios::in);
		cfg << file;
		file.close();
		result = 1;
	} catch (const StormByte::Config::Exception&) {
		// Expected
	}

	RETURN_TEST("bad_binary_in_list_unclosed", result);
}

int test_list_duplicate_detection() {
	Config cfg;
	Item::List& list = cfg.Add(Item::List("mylist")).As<Item::List>();
	list.Add(Item::Value(42));
	bool caught = false;
	try {
		list.Add(Item::Value(42));
	} catch (const StormByte::Config::ItemAlreadyExists&) {
		caught = true;
	}

	if (!caught) {
		std::cerr << "test_list_duplicate_detection: expected ItemAlreadyExists\n";
		return 1;
	}

	if (list.Size() != StormByte::Size{1}) {
		std::cerr << "test_list_duplicate_detection: size after throw is " << static_cast<std::size_t>(list.Size()) << "\n";
		return 1;
	}

	list.Add(Item::Value(43));
	if (list.Size() != StormByte::Size{2}) {
		std::cerr << "test_list_duplicate_detection: final size is " << static_cast<std::size_t>(list.Size()) << "\n";
		return 1;
	}

	RETURN_TEST("test_list_duplicate_detection", 0);
}

int test_list_duplicate_keep() {
	int result = 0;
	Config cfg;
	Item::List& list = cfg.Add(Item::List("mylist")).As<Item::List>();
	list.Add(Item::Value(10), OnExistingAction::Keep);
	list.Add(Item::Value(10), OnExistingAction::Keep);
	ASSERT_EQUAL("test_list_duplicate_keep", static_cast<std::size_t>(1), static_cast<std::size_t>(list.Size()));
	ASSERT_EQUAL("test_list_duplicate_keep", 10, list[StormByte::Size{0}].As<Item::Integer>());
	RETURN_TEST("test_list_duplicate_keep", result);
}

int test_list_duplicate_overwrite() {
	int result = 0;
	Config cfg;
	Item::List& list = cfg.Add(Item::List("mylist")).As<Item::List>();
	list.Add(Item::Value(10), OnExistingAction::Overwrite);
	list.Add(Item::Value(20), OnExistingAction::Overwrite);
	ASSERT_EQUAL("test_list_duplicate_overwrite", static_cast<std::size_t>(2), static_cast<std::size_t>(list.Size()));
	RETURN_TEST("test_list_duplicate_overwrite", result);
}

int test_different_types_not_equal_in_list() {
	int result = 0;
	Config cfg;
	Item::List& list = cfg.Add(Item::List("mylist")).As<Item::List>();
	list.Add(Item::Value(42));
	list.Add(Item::Value("42"));
	ASSERT_EQUAL("test_different_types_not_equal_in_list", static_cast<std::size_t>(2), static_cast<std::size_t>(list.Size()));
	RETURN_TEST("test_different_types_not_equal_in_list", result);
}

int test_comment_types_not_equal() {
	int result = 0;
	Item::Comment<Item::CommentType::SingleLineBash> bash("same text");
	Item::Comment<Item::CommentType::SingleLineC> cxx("same text");
	Item::Comment<Item::CommentType::MultiLineC> multi("same text");
	ASSERT_TRUE("test_comment_types_not_equal", !(bash == cxx));
	ASSERT_TRUE("test_comment_types_not_equal", !(bash == multi));
	ASSERT_TRUE("test_comment_types_not_equal", !(cxx == multi));
	RETURN_TEST("test_comment_types_not_equal", result);
}

int test_on_existing_action_propagation() {
	Config cfg;
	cfg.OnExistingAction(OnExistingAction::Keep);
	Item::Group& g = cfg.Add(Item::Group("g")).As<Item::Group>();
	Item::List& l = g.Add(Item::List("l")).As<Item::List>();
	ASSERT_EQUAL("test_on_existing_action_propagation",
		static_cast<int>(OnExistingAction::Keep),
		static_cast<int>(l.GetOnExistingAction()));
	l.Add(Item::Value(42));
	l.Add(Item::Value(42));
	ASSERT_EQUAL("test_on_existing_action_propagation", static_cast<std::size_t>(1), static_cast<std::size_t>(l.Size()));
	RETURN_TEST("test_on_existing_action_propagation", 0);
}

int test_comments_never_duplicate() {
	Config cfg;
	Item::List& list = cfg.Add(Item::List("l")).As<Item::List>();
	list.Add(Item::Comment<Item::CommentType::SingleLineBash>("same"));
	list.Add(Item::Comment<Item::CommentType::SingleLineBash>("same"));
	ASSERT_EQUAL("test_comments_never_duplicate", static_cast<std::size_t>(2), static_cast<std::size_t>(list.Size()));
	RETURN_TEST("test_comments_never_duplicate", 0);
}

int test_empty_containers_equality() {
	Item::Group g1("a"), g2("a");
	Item::List l1("b"), l2("b");
	ASSERT_TRUE("test_empty_containers_equality", g1 == g2);
	ASSERT_TRUE("test_empty_containers_equality", l1 == l2);
	ASSERT_TRUE("test_empty_containers_equality", !(g1 == l1));
	RETURN_TEST("test_empty_containers_equality", 0);
}

int test_invalid_name_in_group() {
	Config cfg;
	Item::Group& g = cfg.Add(Item::Group("g")).As<Item::Group>();
	try {
		g.Add(Item::Value("1invalid", 10));
		return 1;
	} catch (const InvalidName&) {
		// Expected
	}

	RETURN_TEST("test_invalid_name_in_group", 0);
}

int test_exception_component() {
	int result = 0;
	StormByte::Config::Exception ex_default("default message");
	ASSERT_EQUAL("test_exception_component", std::string("StormByte.Config: default message"), std::string(ex_default.what()));

	StormByte::Config::InvalidName ex_derived("invalid item name");
	ASSERT_EQUAL("test_exception_component", std::string("StormByte.Config: invalid item name"), std::string(ex_derived.what()));

	StormByte::Config::Exception ex_custom("Custom", std::string("custom message"));
	ASSERT_EQUAL("test_exception_component", std::string("StormByte.Custom: custom message"), std::string(ex_custom.what()));

	RETURN_TEST("test_exception_component", result);
}

int test_invalid_path_exceptions() {
	Config cfg;
	try {
		cfg["foo//bar"];
		return 1;
	} catch (const InvalidPath&) {
		// Expected
	}

	try {
		cfg["/foo"];
		return 1;
	} catch (const InvalidPath&) {
		// Expected
	}

	RETURN_TEST("test_invalid_path_exceptions", 0);
}

int test_numeric_path_overflow_exceptions() {
	Config cfg;
	Item::List& list = cfg.Add(Item::List("mylist")).As<Item::List>();
	list.Add(Item::Value(123));
	try {
		list["9999999999999999999999"];
		return 1;
	} catch (const OutOfBounds&) {
		// Expected
	} catch (const std::exception&) {
		std::cerr << "test_numeric_path_overflow_exceptions leaked std::exception!\n";
		return 1;
	}

	RETURN_TEST("test_numeric_path_overflow_exceptions", 0);
}

int test_double_precision_serialization() {
	int result = 0;
	Item::Value v("pi", 3.141592653589793);
	std::string serialized = static_cast<std::string>(v.Serialize(0));
	ASSERT_EQUAL("test_double_precision_serialization", std::string("pi = 3.141592653589793"), serialized);

	Item::Value integer_double("val", 42.0);
	std::string serialized_int = static_cast<std::string>(integer_double.Serialize(0));
	ASSERT_EQUAL("test_double_precision_serialization", std::string("val = 42.0"), serialized_int);

	RETURN_TEST("test_double_precision_serialization", result);
}

int test_null_pointer_addition() {
	Config cfg;
	try {
		cfg.Add(Item::Base::PointerType{});
		return 1;
	} catch (const Exception&) {
		// Expected
	}

	RETURN_TEST("test_null_pointer_addition", 0);
}

int main() {
	int result = 0;

	// -------------------
	// Comments
	// -------------------
	result += test_comment_types();
	result += commented_config();
	result += test_comment_types_not_equal();
	result += good_comment_multi_conf1();
	result += all_comment_types_test();
	result += test_comments_never_duplicate();

	// -------------------
	// Files
	// -------------------
	result += bad_config1();
	result += bad_config2();
	result += bad_config3();
	result += good_double_conf1();
	result += good_double_conf2();
	result += good_string_conf();
	result += test_special_characters_string();
	result += test_long_string();
	result += test_unmatched_braces();
	result += good_boolean_config1();
	result += bad_boolean_config1();
	result += good_binary_simple();
	result += good_binary_mixed();
	result += good_binary_empty();
	result += good_binary_nested();
	result += bad_binary_invalid_base64();
	result += bad_binary_unclosed();
	result += bad_binary_missing_quote();
	result += bad_binary_wrong_prefix();
	result += bad_binary_in_list_unclosed();

	// -------------------
	// Groups
	// -------------------
	result += test_add_remove_group();
	result += test_complex_config_creation();
	result += test_nested_groups();
	result += test_write_nested_groups();
	result += test_deeply_nested_groups();
	result += test_empty_containers_equality();
	result += test_invalid_name_in_group();

	// -------------------
	// Lists
	// -------------------
	result += config_list_test();
	result += config_list_access_by_index();
	result += test_list_duplicate_detection();
	result += test_list_duplicate_keep();
	result += test_list_duplicate_overwrite();
	result += test_different_types_not_equal_in_list();

	// -------------------
	// Policy
	// -------------------
	result += duplicated_insertion();
	result += on_name_clash_keep_existing();
	result += on_name_clash_replace();
	result += test_on_existing_action_propagation();
	result += test_config_hooks();
	result += test_on_failure_hook();
	result += test_null_pointer_addition();

	// -------------------
	// Values
	// -------------------
	result += test_add_and_lookup();
	result += test_write_and_read();
	result += test_empty_string();
	result += test_integer_boundaries();
	result += test_special_characters_in_string();
	result += test_invalid_syntax();
	result += copy_configuration();
	result += move_configuration();
	result += config_to_config_output();
	result += config_value_reference_change();
	result += config_remove_full_path();
	result += config_test_add_empty_name();
	result += complex_conf1();
	result += copy_and_delete();
	result += complex_path_access();
	result += size_and_count();
	result += test_exception_component();
	result += test_invalid_path_exceptions();
	result += test_numeric_path_overflow_exceptions();
	result += test_double_precision_serialization();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
