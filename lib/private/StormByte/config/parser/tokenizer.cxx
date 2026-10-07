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

#include <StormByte/config/parser/tokenizer.hxx>
#include <cctype>
#include <format>
using namespace StormByte::Config;
using namespace StormByte::Config::Parser;
Tokenizer::Tokenizer(std::istream& stream)
	: m_stream(stream) {}
Token Tokenizer::Next() {
	SkipWhitespace();
	if (m_stream.eof() || !m_stream.good()) {
		return Token{TokenType::EndOfFile, {}, CommentType::None, m_line};
	}

	char c = static_cast<char>(m_stream.peek());
	// Single-character symbols
	switch (c) {
		case '=': m_stream.get(); return {TokenType::Equal,    "=", CommentType::None, m_line};
		case '{': m_stream.get(); return {TokenType::LBrace,   "{", CommentType::None, m_line};
		case '}': m_stream.get(); return {TokenType::RBrace,   "}", CommentType::None, m_line};
		case '[': m_stream.get(); return {TokenType::LBracket, "[", CommentType::None, m_line};
		case ']': m_stream.get(); return {TokenType::RBracket, "]", CommentType::None, m_line};
	}

	// Binary data: b"..."
	if (c == 'b') {
		m_stream.get(); // consume 'b'
		if (m_stream.peek() == '"') {
			auto result = ReadBinary();
			if (!result) {
				return Token{TokenType::Unknown, result.error()->what(), CommentType::None, m_line};
			}

			return result.value();
		}

		// Not a binary literal → put the 'b' back and treat as identifier
		m_stream.unget();
	}

	// String
	if (c == '"') {
		auto result = ReadString();
		if (!result) {
			return Token{TokenType::Unknown, result.error()->what(), CommentType::None, m_line};
		}

		return result.value();
	}

	// Comments
	if (c == '#') {
		m_stream.get();
		return ReadSingleLineComment(CommentType::SingleLineBash);
	}

	if (c == '/') {
		m_stream.get();
		char next = static_cast<char>(m_stream.peek());
		if (next == '/') {
			m_stream.get();
			return ReadSingleLineComment(CommentType::SingleLineC);
		}

		if (next == '*') {
			m_stream.get();
			auto result = ReadMultiLineComment();
			if (!result) {
				return Token{TokenType::Unknown, result.error()->what(), CommentType::None, m_line};
			}

			return result.value();
		}

		// Lone '/' → treat as unknown
		return Token{TokenType::Unknown, "/", CommentType::None, m_line};
	}

	// Number
	if (c == '+' || c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
		return ReadNumber();
	}

	// Identifier / keyword (true, false, item names...)
	if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
		return ReadIdentifierOrKeyword();
	}

	// Unknown character
	m_stream.get();
	return Token{TokenType::Unknown, std::string(1, c), CommentType::None, m_line};
}

void Tokenizer::SkipWhitespace() {
	char c;
	while (m_stream.get(c)) {
		if (c == '\n') {
			++m_line;
			continue;
		}

		if (c == ' ' || c == '\t' || c == '\r')
			continue;
		m_stream.unget();
		return;
	}
}

StormByte::Expected<Token, ParseError> Tokenizer::ReadString() {
	m_stream.get(); // consume opening "
	std::string value;
	bool escaped = false;
	char c;
	while (m_stream.get(c)) {
		if (escaped) {
			switch (c) {
				case '"':  case '\\': value += c; break;
				case 'n':  value += '\n'; break;
				case 'r':  value += '\r'; break;
				case 't':  value += '\t'; break;
				default:
					return StormByte::Unexpected<ParseError>("Invalid escape sequence '\\{}' on line {}", c, m_line);
			}

			escaped = false;
			continue;
		}

		if (c == '\\') {
			escaped = true;
			continue;
		}

		if (c == '"') {
			return Token{TokenType::String, std::move(value), CommentType::None, m_line};
		}

		if (c == '\n') {
			++m_line;
		}

		value += c;
	}

	return StormByte::Unexpected<ParseError>("Unterminated string on line {}", m_line);
}

StormByte::Expected<Token, ParseError> Tokenizer::ReadBinary() {
	// We already consumed the 'b', now we expect "
	if (m_stream.peek() != '"') {
		return StormByte::Unexpected<ParseError>("Expected '\"' after 'b' for binary literal on line {}", m_line);
	}

	m_stream.get(); // consume opening "
	std::string value;
	char c;
	while (m_stream.get(c)) {
		if (c == '"') {
			return Token{TokenType::Binary, std::move(value), CommentType::None, m_line};
		}

		if (c == '\n') {
			++m_line;
		}

		value += c;
	}

	return StormByte::Unexpected<ParseError>("Unterminated binary literal on line {}", m_line);
}

Token Tokenizer::ReadNumber() {
	std::string value;
	bool is_double = false;
	char c;
	// Optional sign
	if (m_stream.peek() == '+' || m_stream.peek() == '-') {
		value += static_cast<char>(m_stream.get());
	}

	while (m_stream.get(c)) {
		if (std::isdigit(static_cast<unsigned char>(c))) {
			value += c;
			continue;
		}

		if (c == '.' && !is_double) {
			is_double = true;
			value += c;
			continue;
		}

		// Exponent (simple support)
		if ((c == 'e' || c == 'E') && !value.empty()) {
			is_double = true;
			value += c;
			if (m_stream.peek() == '+' || m_stream.peek() == '-') {
				value += static_cast<char>(m_stream.get());
			}

			continue;
		}

		m_stream.unget();
		break;
	}

	return Token{
		is_double ? TokenType::Double : TokenType::Integer,
		std::move(value),
		CommentType::None,
		m_line
	};
}

Token Tokenizer::ReadIdentifierOrKeyword() {
	std::string value;
	char c;
	while (m_stream.get(c)) {
		if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.') {
			value += c;
			continue;
		}

		m_stream.unget();
		break;
	}

	if (value == "true" || value == "false") {
		return Token{TokenType::Bool, std::move(value), CommentType::None, m_line};
	}

	return Token{TokenType::Identifier, std::move(value), CommentType::None, m_line};
}

Token Tokenizer::ReadSingleLineComment(CommentType type) {
	std::string value;
	char c;
	while (m_stream.get(c)) {
		if (c == '\n') {
			++m_line;
			break;
		}

		value += c;
	}

	return Token{TokenType::Comment, std::move(value), type, m_line};
}

StormByte::Expected<Token, ParseError> Tokenizer::ReadMultiLineComment() {
	std::string value;
	char c;
	bool closed = false;
	unsigned int start_line = m_line;
	while (m_stream.get(c)) {
		if (c == '*' && m_stream.peek() == '/') {
			m_stream.get(); // consume '/'
			closed = true;
			break;
		}

		if (c == '\n') {
			++m_line;
		}

		value += c;
	}

	if (!closed) {
		return StormByte::Unexpected<ParseError>("Unclosed multi-line comment starting on line {}", start_line);
	}

	return Token{TokenType::Comment, std::move(value), CommentType::MultiLineC, start_line};
}
