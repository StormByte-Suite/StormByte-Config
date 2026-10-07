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

#include <StormByte/base64.hxx>
#include <StormByte/binary_data.hxx>
#include <StormByte/config/item/comment.hxx>
#include <StormByte/config/item/group.hxx>
#include <StormByte/config/item/list.hxx>
#include <StormByte/config/item/value.hxx>
#include <StormByte/config/parser/parser.hxx>
#include <StormByte/safe/string.hxx>

#include <sstream>
#include <string_view>

using namespace StormByte;
using namespace StormByte::Config;
using StormByte::Config::Parser::Token;
using StormByte::Config::Parser::TokenType;
using StormByte::Config::Parser::Tokenizer;
using StormByte::Config::Parser::Mode;
using StormByte::Config::Parser::CommentType;

Parser::Parser::Parser(Tokenizer& tokenizer, const OnExistingAction& action)
	: m_tokenizer(tokenizer), c_on_existing_action(action) {}

Expected<void, ParseError> Parser::Parser::Parse(
	std::istream& stream,
	Item::Group& root,
	const OnExistingAction& action,
	const HookFunctions& before,
	const HookFunctions& after,
	const OptionalFailureHook& on_failure) {
	Tokenizer tokenizer(stream);
	Parser parser(tokenizer, action);
	for (const auto& hook : before) {
		if (!hook || !hook->HasValue())
			continue;
		auto safe_root = Safe::Heap::MakeShared<Item::Group>(root);
		if (hook->Call(safe_root) != Safe::Status::Success)
			throw Exception("Read callback failed before parsing");
		root = *safe_root;
	}
	auto res = parser.Parse(root, Mode::Named);
	if (!res) {
		bool should_throw = true;
		if (on_failure && on_failure->HasValue()) {
			const auto safe_root = Safe::Heap::MakeShared<Item::Group>(root);
			const StormByte::Safe::Shared<const Item::Group> const_root = safe_root;
			if (on_failure->Call(should_throw, const_root) != Safe::Status::Success)
				throw Exception("Parse-failure callback failed");
		}
		if (should_throw)
			return Unexpected(std::move(res.error()));
		return {};
	}

	for (const auto& hook : after) {
		if (!hook || !hook->HasValue())
			continue;
		auto safe_root = Safe::Heap::MakeShared<Item::Group>(root);
		if (hook->Call(safe_root) != Safe::Status::Success)
			throw Exception("Read callback failed after parsing");
		root = *safe_root;
	}
	return {};
}

Expected<void, ParseError> Parser::Parser::Parse(
	const std::string& string,
	Item::Group& root,
	const OnExistingAction& action,
	const HookFunctions& before,
	const HookFunctions& after,
	const OptionalFailureHook& on_failure) {
	std::istringstream iss(string);
	return Parse(iss, root, action, before, after, on_failure);
}

Expected<void, ParseError> Parser::Parser::Parse(Item::Container& container, Mode mode) {
	while (true) {
		Token token = m_tokenizer.Next();
		if (token.type == TokenType::RBrace || token.type == TokenType::RBracket) {
			if (m_container_level == 0)
				return Unexpected<ParseError>("Unexpected container end on line {}", token.line);
			--m_container_level;
			return {};
		}

		if (token.type == TokenType::EndOfFile) {
			if (m_container_level > 0)
				return Unexpected<ParseError>("Unexpected EOF (unclosed container)");
			return {};
		}

		if (token.type == TokenType::Comment) {
			container.Add(MakeComment(token), c_on_existing_action);
			continue;
		}

		std::string name;
		if (mode == Mode::Named) {
			if (token.type != TokenType::Identifier)
				return Unexpected<ParseError>("Expected item name on line {}, got something else", token.line);
			name = std::move(token.value);
			auto eq = Expect(TokenType::Equal);
			if (!eq)
				return Unexpected(std::move(eq.error()));
			token = m_tokenizer.Next();
		}

		Expected<Item::Base::PointerType, ParseError> item_res;
		switch (token.type) {
			case TokenType::String:
				item_res = Item::Base::MakePointer<Item::Value>(StormByte::Safe::String(std::string_view(token.value)));
				break;
			case TokenType::Binary: {
				try {
					BinaryData bytes = Base64Decode(token.value);
					item_res = Item::Base::MakePointer<Item::Value>(std::move(bytes));
				} catch (const Base64Error& ex) {
					return Unexpected<ParseError>("Invalid Base64 data on line {}: {}", token.line, ex.what());
				}
				break;
			}
			case TokenType::Integer: {
				try {
					int v = std::stoi(token.value);
					item_res = Item::Base::MakePointer<Item::Value>(v);
				} catch (...) {
					return Unexpected<ParseError>("Invalid integer '{}' on line {}", token.value, token.line);
				}
				break;
			}
			case TokenType::Double: {
				try {
					double v = std::stod(token.value);
					item_res = Item::Base::MakePointer<Item::Value>(v);
				} catch (...) {
					return Unexpected<ParseError>("Invalid double '{}' on line {}", token.value, token.line);
				}
				break;
			}
			case TokenType::Bool:
				item_res = Item::Base::MakePointer<Item::Value>(token.value == "true");
				break;
			case TokenType::LBrace: {
				++m_container_level;
				Item::Group group;
				auto res = Parse(group, Mode::Named);
				if (!res)
					return Unexpected(std::move(res.error()));
				item_res = group.Move();
				break;
			}
			case TokenType::LBracket: {
				++m_container_level;
				Item::List list;
				auto res = Parse(list, Mode::Unnamed);
				if (!res)
					return Unexpected(std::move(res.error()));
				item_res = list.Move();
				break;
			}
			case TokenType::Comment:
				item_res = MakeComment(token);
				break;
			default:
				return Unexpected<ParseError>("Unexpected token on line {}", token.line);
		}

		if (!item_res)
			return Unexpected(std::move(item_res.error()));
		auto item = std::move(item_res.value());
		if (mode == Mode::Named)
			item->Name(StormByte::Safe::String(std::string_view(name)));
		container.Add(item, c_on_existing_action);
	}
}

Expected<Token, ParseError> Parser::Parser::Expect(TokenType expected) {
	Token token = m_tokenizer.Next();
	if (token.type != expected)
		return Unexpected<ParseError>("Unexpected token on line {}", token.line);
	return token;
}

Item::Base::PointerType Parser::Parser::MakeComment(const Token& token) {
	const StormByte::Safe::String text(std::string_view(token.value));
	switch (token.comment_type) {
		case CommentType::SingleLineBash:
			return Item::Base::MakePointer<Item::Comment<Item::CommentType::SingleLineBash>>(text);
		case CommentType::SingleLineC:
			return Item::Base::MakePointer<Item::Comment<Item::CommentType::SingleLineC>>(text);
		case CommentType::MultiLineC:
			return Item::Base::MakePointer<Item::Comment<Item::CommentType::MultiLineC>>(text);
		default:
			return Item::Base::MakePointer<Item::Comment<Item::CommentType::SingleLineBash>>(text);
	}
}

namespace StormByte::Config::Parser {
	Expected<void, ParseError> Parse(
		std::istream& stream,
		Item::Group& root,
		const OnExistingAction& action,
		const HookFunctions& before,
		const HookFunctions& after,
		const OptionalFailureHook& on_failure) {
		return Parser::Parse(stream, root, action, before, after, on_failure);
	}

	Expected<void, ParseError> Parse(
		const std::string& string,
		Item::Group& root,
		const OnExistingAction& action,
		const HookFunctions& before,
		const HookFunctions& after,
		const OptionalFailureHook& on_failure) {
		return Parser::Parse(string, root, action, before, after, on_failure);
	}
}
