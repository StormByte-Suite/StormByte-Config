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

#include <StormByte/config/exception.hxx>
#include <StormByte/config/parser/type.hxx>
#include <StormByte/expected.hxx>

#include <istream>
#include <string>
#include <vector>

/**
 * @brief Text parser internals.
 */
namespace StormByte::Config::Parser {
	/**
	 * @enum TokenType
	 * @brief Types of tokens produced by the Tokenizer.
	 */
	enum class TokenType {
		Identifier,	///< Item name (left side of `=`)
		String,		///< Quoted string value
		Binary,		///< Base64 payload with `b"..."` prefix
		Integer,	///< Integer number
		Double,		///< Floating-point number
		Bool,		///< `true` / `false`
		Comment,	///< Any comment
		Equal,		///< `=`
		LBrace,		///< `{`
		RBrace,		///< `}`
		LBracket,	///< `[`
		RBracket,	///< `]`
		EndOfFile,	///< End of input
		Unknown		///< Unrecognized character
	};

	/**
	 * @struct Token
	 * @brief A single lexical token.
	 */
	struct Token {
		TokenType type = TokenType::Unknown;			///< Token type
		std::string value;								///< Lexeme / content
		CommentType comment_type = CommentType::None;	///< Valid when type == Comment
		unsigned int line = 1;							///< Line where the token starts
	};

	/**
	 * @class Tokenizer
	 * @brief Turns an input stream into a token sequence.
	 */
	class STORMBYTE_CONFIG_PRIVATE Tokenizer {
		public:
			/**
			 * @brief Constructs a tokenizer for the given input stream.
			 * @param stream Input stream to tokenize.
			 */
			explicit Tokenizer(std::istream& stream);

			Tokenizer(const Tokenizer&) = delete;
			Tokenizer(Tokenizer&&) = default;
			Tokenizer& operator=(const Tokenizer&) = delete;
			Tokenizer& operator=(Tokenizer&&) = delete;
			~Tokenizer() = default;

			/**
			 * @brief Returns the next token from the stream.
			 * @return Next token. `type == EndOfFile` when finished.
			 */
			Token Next();

			/**
			 * @brief Returns the current line number (1-based).
			 * @return Current line number.
			 */
			unsigned int CurrentLine() const noexcept { return m_line; }

		private:
			std::istream& m_stream;		///< Underlying input stream
			unsigned int m_line = 1;	///< Current line number

			/**
			 * @brief Skips whitespace and updates the line counter.
			 */
			void SkipWhitespace();

			/**
			 * @brief Reads a quoted string (handles escapes).
			 * @return Token of type String, or a parse error.
			 */
			Expected<Token, ParseError> ReadString();

			/**
			 * @brief Reads a binary literal (`b"..."`).
			 * The quoted payload is Base64; decoding happens in the parser.
			 * @return Token of type Binary, or a parse error.
			 */
			Expected<Token, ParseError> ReadBinary();

			/**
			 * @brief Reads a number (integer or double).
			 * @return Token of type Integer or Double.
			 */
			Token ReadNumber();

			/**
			 * @brief Reads an identifier or keyword (`true`/`false`).
			 * @return Token of type Identifier or Bool.
			 */
			Token ReadIdentifierOrKeyword();

			/**
			 * @brief Reads a single-line comment (`#` or `//`).
			 * @param type Comment subtype.
			 * @return Token of type Comment.
			 */
			Token ReadSingleLineComment(CommentType type);

			/**
				 * @brief Reads a multi-line C-style block comment.
			 * @return Token of type Comment, or a parse error if unclosed.
			 */
			Expected<Token, ParseError> ReadMultiLineComment();
	};
}
