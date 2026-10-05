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

#include <StormByte/config/alias.hxx>
#include <StormByte/config/item/comment.hxx>
#include <StormByte/config/item/group.hxx>
#include <StormByte/config/item/list.hxx>
#include <StormByte/config/item/value.hxx>
#include <StormByte/config/typedefs.hxx>
#include <StormByte/platform.h>
#include <StormByte/size.hxx>
#include <StormByte/safe/string.hxx>

#include <istream>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Config module of the StormByte suite.
 */
namespace StormByte::Config {
	namespace Binary {
		class Reader;
		class Writer;
	}

	/**
	 * @class Config
	 * @brief Configuration document (text or versioned binary).
	 *
	 * A document holds:
	 * - Boolean, double, integer, string and binary values
	 * - Comments (single-line or multi-line)
	 * - Groups and lists
	 */
	class STORMBYTE_CONFIG_PUBLIC Config {
		friend class Binary::Reader;
		friend class Binary::Writer;
		public:
			/**
			 * @name Construction
			 * @{
			 */
			/**
			 * @brief Default constructor.
			 */
			Config();

			/**
			 * @brief Copy constructor.
			 * @param config	Configuration to copy.
			 */
			Config(const Config& config);

			/**
			 * @brief Move constructor.
			 * @param config	Configuration to move.
			 */
			Config(Config&& config) noexcept;

			/**
			 * @brief Copy assignment operator.
			 * @param config	Configuration to assign.
			 * @return			Reference to this Config.
			 */
			Config& operator=(const Config& config);

			/**
			 * @brief Move assignment operator.
			 * @param config	Configuration to move.
			 * @return			Reference to this Config.
			 */
			Config& operator=(Config&& config) noexcept;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			virtual ~Config() noexcept;
			/** @} */

			/**
			 * @name Access
			 * @{
			 */
			/**
			 * @brief Gets a reference to an item by path.
			 * @param path	Path to the item.
			 * @return		Item reference.
			 */
			inline Item::Base& operator[](const StormByte::Safe::String& path) {
				return m_root.operator[](path);
			}

			/**
			 * @brief Gets a const reference to an item by path.
			 * @param path	Path to the item.
			 * @return		Item const reference.
			 */
			inline const Item::Base& operator[](const StormByte::Safe::String& path) const {
				return m_root.operator[](path);
			}

			/**
			 * @brief Gets a reference to an item by a path view.
			 * @param path	Path to the item.
			 * @return		Item reference.
			 */
			STORMBYTE_FORCE_INLINE Item::Base& operator[](std::string_view path) {
				return m_root.operator[](path);
			}

			/**
			 * @brief Gets a const reference to an item by a path view.
			 * @param path	Path to the item.
			 * @return		Item const reference.
			 */
			STORMBYTE_FORCE_INLINE const Item::Base& operator[](std::string_view path) const {
				return m_root.operator[](path);
			}

			/**
			 * @brief Gets a reference to an item by index.
			 * @param index	Index of the item.
			 * @throw OutOfBounds if index is out of bounds.
			 * @return		Item reference.
			 */
			Item::Base& operator[](const StormByte::Size& index) {
				return m_root[index];
			}

			/**
			 * @brief Gets a const reference to an item by index.
			 * @param index	Index of the item.
			 * @throw OutOfBounds if index is out of bounds.
			 * @return		Item const reference.
			 */
			const Item::Base& operator[](const StormByte::Size& index) const {
				return m_root[index];
			}

			/**
			 * @brief Equality operator.
			 * @param config	Configuration to compare.
			 * @return			true if equal.
			 */
			inline bool operator==(const Config& config) const noexcept {
				return m_root == config.m_root;
			}

			/**
			 * @brief Inequality operator.
			 * @param config	Configuration to compare.
			 * @return			true if not equal.
			 */
			inline bool operator!=(const Config& config) const noexcept {
				return !operator==(config);
			}
			/** @} */

			/**
			 * @name Input
			 * @{
			 */
			/**
			 * @brief Import data from another configuration.
			 * @param source	Source configuration to import.
			 * @return			Reference to this configuration.
			 */
			Config& operator<<(const Config& source);

			/**
			 * @brief Initialize configuration from an input stream (text mode).
			 * @param istream	Input stream.
			 */
			STORMBYTE_FORCE_INLINE void operator<<(std::istream& istream) {
				std::string text;
			char chunk[4096];
				while (istream.read(chunk, sizeof(chunk)) || istream.gcount() > 0)
					text.append(chunk, static_cast<std::size_t>(istream.gcount()));
				operator<<(StormByte::Safe::String(std::string_view(text)));
			}

			/**
			 * @brief Initialize configuration from a string (text mode).
			 * @param str	Input text. Copied into the parser; not stored as `std::string`.
			 */
			void operator<<(const StormByte::Safe::String& str);

			/**
			 * @brief Initialize configuration from a caller-owned `std::string` (text mode).
			 * @param str	Input text.
			 */
			STORMBYTE_FORCE_INLINE void operator<<(const std::string& str) {
				operator<<(StormByte::Safe::String(std::string_view(str)));
			}

			/**
			 * @brief Initializes configuration when istream is on the left-hand side.
			 * @param istream	Input stream.
			 * @param file		Config to put data into.
			 * @return			Reference to the Config.
			 */
			friend Config& operator>>(std::istream& istream, Config& file);

			/**
			 * @brief Initializes configuration when String is on the left-hand side.
			 * @param str	Input text.
			 * @param file	Config to put data into.
			 * @return		Reference to the Config.
			 */
			friend Config& operator>>(const StormByte::Safe::String& str, Config& file);

			/**
			 * @brief Initializes configuration when string is on the left-hand side.
			 * @param str	Input text.
			 * @param file	Config to put data into.
			 * @return		Reference to the Config.
			 */
			friend Config& operator>>(const std::string& str, Config& file);
			/** @} */

			/**
			 * @name Output
			 * @{
			 */
			/**
			 * @brief Output current configuration into another configuration.
			 * @param dest	Destination configuration.
			 * @return		Reference to the destination configuration.
			 */
			Config& operator>>(Config& dest) const;

			/**
			 * @brief Output configuration serialized to an output stream (text).
			 * @param ostream	Output stream.
			 * @return			Reference to the output stream.
			 */
			STORMBYTE_FORCE_INLINE std::ostream& operator>>(std::ostream& ostream) const {
				Save(ostream, Mode::Text);
				return ostream;
			}

			/**
			 * @brief Append serialized text to a caller-owned string.
			 * @param str	Output string.
			 * @return		Reference to the string.
			 */
			STORMBYTE_FORCE_INLINE std::string& operator>>(std::string& str) const {
				str += static_cast<std::string>(Text());
				return str;
			}

			/**
			 * @brief Output configuration when ostream is on the left-hand side.
			 * @param ostream	Output stream.
			 * @param file		Config to get data from.
			 * @return			Reference to the output stream.
			 */
			friend std::ostream& operator<<(std::ostream& ostream, const Config& file);

			/**
			 * @brief Output configuration when string is on the left-hand side.
			 * @param str	Output string.
			 * @param file	Config to get data from.
			 * @return		Reference to the string.
			 */
			friend std::string& operator<<(std::string& str, const Config& file);

			/**
			 * @brief Serialized document as StormByte text.
			 * @return	Owned UTF-8 text.
			 */
			StormByte::Safe::String Text() const;

			/**
			 * @brief Converts the current configuration to a string (text form).
			 * @return	Serialized configuration text on the caller heap.
			 */
			STORMBYTE_FORCE_INLINE explicit operator std::string() const {
				return static_cast<std::string>(Text());
			}

			/**
			 * @brief Serialize the document to Base-owned binary bytes.
			 * @return Versioned binary document.
			 */
			StormByte::BinaryData Binary() const;

			/**
			 * @brief Write this document to an output stream.
			 * @param stream	Destination (file, stringstream, etc.).
			 * @param mode		Text (default): config syntax; Binary: versioned wire format.
			 */
			STORMBYTE_FORCE_INLINE void Save(std::ostream& stream, Mode mode = Mode::Text) const {
				if (mode == Mode::Text) {
					stream << static_cast<std::string>(Text());
					return;
				}

				const auto data = Binary();
				const auto size = static_cast<std::size_t>(data.size());
				if (size > 0)
					stream.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(size));
			}

			/**
			 * @brief Load a document from Base-owned UTF-8 text.
			 * @param text Configuration document text.
			 * @return Config on success, or a StormByte::Exception derivative on failure.
			 */
			static ExpectedConfig Load(const StormByte::Safe::String& text);

			/**
			 * @brief Load a document from Base-owned binary bytes.
			 * @param data Versioned binary document.
			 * @return Config on success, or a StormByte::Exception derivative on failure.
			 */
			static ExpectedConfig Load(const StormByte::BinaryData& data);

			/**
			 * @brief Read a document from a caller-owned stream.
			 * @param stream Source stream.
			 * @param mode Text (default) or versioned binary format.
			 * @return Config on success, or a StormByte::Exception derivative on failure.
			 */
			STORMBYTE_FORCE_INLINE static ExpectedConfig Load(std::istream& stream, Mode mode = Mode::Text) {
				if (mode == Mode::Text) {
					std::string text;
					char chunk[4096];
					while (stream.read(chunk, sizeof(chunk)) || stream.gcount() > 0)
						text.append(chunk, static_cast<std::size_t>(stream.gcount()));
					return Load(StormByte::Safe::String(std::string_view(text)));
				}

				if (stream.bad())
					return StormByte::Unexpected<StormByte::DeserializeError>("Failed to read binary config stream");
				stream.clear();
				stream.seekg(0, std::ios::end);
				const std::streamsize size = stream.tellg();
				stream.seekg(0, std::ios::beg);
				std::vector<std::byte> bytes;
				if (size < 0) {
					stream.clear();
					char chunk[4096];
					while (stream.read(chunk, sizeof(chunk)) || stream.gcount() > 0) {
						const auto count = static_cast<std::size_t>(stream.gcount());
						const auto* begin = reinterpret_cast<const std::byte*>(chunk);
						bytes.insert(bytes.end(), begin, begin + count);
					}
				} else {
					bytes.resize(static_cast<std::size_t>(size));
					if (size > 0 && !stream.read(reinterpret_cast<char*>(bytes.data()), size))
						return StormByte::Unexpected<StormByte::DeserializeError>("Failed to read binary config stream");
				}
				return Load(StormByte::BinaryData(bytes));
			}
			/** @} */

			/**
			 * @name Items
			 * @{
			 */
			/**
			 * @brief Adds an item to the configuration.
			 * @param item	The item to add.
			 * @throw ItemNameAlreadyExists if the item's name already exists.
			 * @return		A reference to the added item.
			 */
			inline Item::Base& Add(const Item::Base& item) {
				return m_root.Add(item.Clone(), m_on_existing_action);
			}

			/**
			 * @brief Adds an item to the configuration (move).
			 * @param item	Item to add.
			 * @throw ItemNameAlreadyExists if item name already exists.
			 * @return		Reference to the added item.
			 */
			Item::Base& Add(Item::Base&& item) {
				return m_root.Add(std::move(item.Move()), m_on_existing_action);
			}

			/**
			 * @brief Adds an item pointer to the configuration.
			 * @param item	Item pointer to add.
			 * @throw ItemNameAlreadyExists if item name already exists.
			 * @return		Reference to the added item.
			 */
			inline Item::Base& Add(Item::Base::PointerType item) {
				return m_root.Add(item, m_on_existing_action);
			}

			/**
			 * @brief Adds an item pointer using an explicit policy.
			 * @param item			Item pointer to add.
			 * @param on_existing	Action to take if the item already exists.
			 * @return				Reference to the added item.
			 */
			inline Item::Base& Add(Item::Base::PointerType item, const StormByte::Config::OnExistingAction& on_existing) {
				return m_root.Add(item, on_existing);
			}

			/**
			 * @brief Clears all configuration items.
			 */
			inline void Clear() noexcept {
				m_root.Clear();
			}

			/**
			 * @brief Checks if an item exists by path.
			 * @param path	Path to the item.
			 * @return		true if the item exists.
			 */
			inline bool Exists(const StormByte::Safe::String& path) const {
				return m_root.Exists(path);
			}

			/**
			 * @brief Checks if an item exists by a path view.
			 * @param path	Path to the item.
			 * @return		true if the item exists.
			 */
			STORMBYTE_FORCE_INLINE bool Exists(std::string_view path) const {
				return m_root.Exists(path);
			}

			/**
			 * @brief Removes an item by path.
			 * @param path	Item path.
			 * @throw ItemNotFound if item is not found.
			 */
			inline void Remove(const StormByte::Safe::String& path) {
				m_root.Remove(path);
			}

			/**
			 * @brief Removes an item by a path view.
			 * @param path	Item path.
			 */
			STORMBYTE_FORCE_INLINE void Remove(std::string_view path) {
				m_root.Remove(path);
			}

			/**
			 * @brief Removes an item by index.
			 * @param index	Index of the item.
			 * @throw OutOfBounds if index is out of bounds.
			 */
			inline void Remove(const StormByte::Size& index) {
				m_root.Remove(index);
			}

			/**
			 * @brief Gets the number of items in the current level.
			 * @return	Number of items.
			 */
			StormByte::Size Size() const noexcept {
				return m_root.Size();
			}

			/**
			 * @brief Gets the full number of items (including nested).
			 * @return	Total number of items.
			 */
			StormByte::Size Count() const noexcept {
				return m_root.Count();
			}

			/**
			 * @brief Items in the current level. The span cannot reseat or grow the store; each pointer's item is mutable.
			 * @return	Span of item pointers.
			 */
			const StormByte::Safe::Vector<Item::Base::PointerType>& Items() noexcept {
				return m_root.Items();
			}

			/**
			 * @brief Items in the current level.
			 * @return	Span of item pointers.
			 */
			const StormByte::Safe::Vector<Item::Base::PointerType>& Items() const noexcept {
				return m_root.Items();
			}
			/** @} */

			/**
			 * @name Policy
			 * @{
			 */
			/**
			 * @brief Sets the action to take when an item name/identity collision occurs.
			 * The policy is applied to the root container and will be inherited by all nested containers.
			 * @param on_existing	The policy to use.
			 */
			void OnExistingAction(const StormByte::Config::OnExistingAction& on_existing);

			/**
			 * @brief Sets a safe function to execute on parse failure.
			 * @param hook	Callback receiving a Base-owned const group handle. Return false to swallow the error.
			 */
			void OnParseFailure(FailureHook hook);

			/**
			 * @brief Adds a safe callback executed before reading starts.
			 * @param hook	Callback receiving a Base-owned mutable group handle.
			 */
			void AddHookBeforeRead(ReadHook hook);

			/**
			 * @brief Adds a safe callback executed after a successful read.
			 * @param hook	Callback receiving a Base-owned mutable group handle.
			 */
			void AddHookAfterRead(ReadHook hook);
			/** @} */

		protected:
			Item::Group m_root;	///< Root group

			HookFunctions m_before_read_hooks;	///< Hooks executed before reading
			HookFunctions m_after_read_hooks;	///< Hooks executed after successful reading
			OptionalFailureHook m_on_parse_failure_hook; ///< Callback executed on failure, empty if unset.
			StormByte::Config::OnExistingAction m_on_existing_action;	///< Collision policy
	};

	/**
	 * @brief Initializes configuration when istream is on the left-hand side.
	 * @param istream	Input stream.
	 * @param file		Config to put data into.
	 * @return			Reference to the Config.
	 */
	STORMBYTE_FORCE_INLINE Config& operator>>(std::istream& istream, Config& file) {
		file << istream;
		return file;
	}

	/**
	 * @brief Initializes configuration when String is on the left-hand side.
	 * @param str	Input text.
	 * @param file	Config to put data into.
	 * @return		Reference to the Config.
	 */
	STORMBYTE_FORCE_INLINE Config& operator>>(const StormByte::Safe::String& str, Config& file) {
		file << str;
		return file;
	}

	/**
	 * @brief Initializes configuration when string is on the left-hand side.
	 * @param str	Input text.
	 * @param file	Config to put data into.
	 * @return		Reference to the Config.
	 */
	STORMBYTE_FORCE_INLINE Config& operator>>(const std::string& str, Config& file) {
		file << str;
		return file;
	}

	/**
	 * @brief Output configuration when ostream is on the left-hand side.
	 * @param ostream	Output stream.
	 * @param file		Config to get data from.
	 * @return			Reference to the output stream.
	 */
	STORMBYTE_FORCE_INLINE std::ostream& operator<<(std::ostream& ostream, const Config& file) {
		file.Save(ostream, Mode::Text);
		return ostream;
	}

	/**
	 * @brief Output configuration when string is on the left-hand side.
	 * @param str	Output string.
	 * @param file	Config to get data from.
	 * @return		Reference to the string.
	 */
	STORMBYTE_FORCE_INLINE std::string& operator<<(std::string& str, const Config& file) {
		return file.operator>>(str);
	}
}

STORMBYTE_DECLARE_MAYBE_SAFE(StormByte::Config::Config);
