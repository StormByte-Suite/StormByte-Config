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
#include <StormByte/config/item/base.hxx>
#include <StormByte/config/typedefs.hxx>
#include <StormByte/size.hxx>
#include <StormByte/safe/string.hxx>

#include <queue>
#include <span>
#include <string_view>
#include <vector>

/**
 * @brief Configuration items (values, comments, groups, lists).
 */
namespace StormByte::Config::Item {
	/**
	 * @class Container
	 * @brief Holds child items. `Group` and `List` derive from this.
	 */
	class STORMBYTE_CONFIG_PUBLIC Container: public Base {
		public:
			/**
			 * @name Construction
			 * @{
			 */
			/**
			 * @brief Constructs an empty Container.
			 */
			Container();

			/**
			 * @brief Constructs a Container with the given name.
			 * @param name	The name of the container.
			 */
			Container(const StormByte::Safe::String& name);

			/**
			 * @brief Constructs a Container with the given name (move).
			 * @param name	The name of the container.
			 */
			Container(StormByte::Safe::String&& name);

			/**
			 * @brief Copy constructor.
			 * @param base	Container to copy.
			 */
			Container(const Container& base);

			/**
			 * @brief Move constructor.
			 * @param base	Container to move.
			 */
			Container(Container&& base) noexcept;

			/**
			 * @brief Copy assignment operator.
			 * @param base	Container to copy.
			 * @return		Reference to this Container.
			 */
			Container& operator=(const Container& base);

			/**
			 * @brief Move assignment operator.
			 * @param base	Container to move.
			 * @return		Reference to this Container.
			 */
			Container& operator=(Container&& base) noexcept;

			/**
			 * @brief Destructor. Defined in this module so `catch` matches across a DLL.
			 */
			virtual ~Container() noexcept override;
			/** @} */

			/**
			 * @name Access
			 * @{
			 */
			/**
			 * @brief Polymorphic equality comparison.
			 * @param other	The other item to compare against.
			 * @return		true if both containers have the same type, name and contents.
			 */
			bool Equals(const Base& other) const noexcept override;

			/**
			 * @brief Gets a reference to an item by index.
			 * @param index	Index of the item.
			 * @throw OutOfBounds if index is out of bounds.
			 * @return		Reference to the item.
			 */
			Base& operator[](const StormByte::Size& index);

			/**
			 * @brief Gets a const reference to an item by index.
			 * @param index	Index of the item.
			 * @throw OutOfBounds if index is out of bounds.
			 * @return		Const reference to the item.
			 */
			const Base& operator[](const StormByte::Size& index) const;

			/**
			 * @brief Gets a reference to an item by path.
			 * @param path	Path to the item.
			 * @throw InvalidPath if path is invalid.
			 * @throw ItemNotFound if item is not found.
			 * @return		Reference to the item.
			 */
			Base& operator[](const StormByte::Safe::String& path);

			/**
			 * @brief Gets a const reference to an item by path.
			 * @param path	Path to the item.
			 * @throw InvalidPath if path is invalid.
			 * @throw ItemNotFound if item is not found.
			 * @return		Const reference to the item.
			 */
			inline const Base& operator[](const StormByte::Safe::String& path) const {
				return LookUp(path);
			}

			/**
			 * @brief Gets a reference to an item by a path view.
			 * @param path	Path to the item.
			 * @return		Reference to the item.
			 */
			inline Base& operator[](std::string_view path) {
				return operator[](StormByte::Safe::String(path));
			}

			/**
			 * @brief Gets a const reference to an item by a path view.
			 * @param path	Path to the item.
			 * @return		Const reference to the item.
			 */
			inline const Base& operator[](std::string_view path) const {
				return operator[](StormByte::Safe::String(path));
			}

			/**
			 * @brief Equality operator.
			 * @param container	Container to compare.
			 * @return			true if equal.
			 */
			bool operator==(const Container& container) const noexcept;

			/**
			 * @brief Inequality operator.
			 * @param container	Container to compare.
			 * @return			true if not equal.
			 */
			inline bool operator!=(const Container& container) const noexcept {
				return !operator==(container);
			}

			/**
			 * @brief Serializes the container to a string.
			 * @param indent_level	Indentation level.
			 * @return				Serialized text.
			 */
			StormByte::Safe::String Serialize(const int& indent_level) const noexcept override;

			/**
			 * @brief Returns the enclosure characters for a given container type.
			 * @param type	Container type.
			 * @return		Pair of opening and closing characters.
			 */
			static constexpr std::pair<const char, const char> EnclosureCharacters(const ContainerType& type) noexcept {
				switch (type) {
					case ContainerType::Group: return {'{', '}'};
					case ContainerType::List:  return {'[', ']'};
					default:                   return {'\0', '\0'};
				}
			}

			/**
			 * @brief Returns the closing character for a given container type.
			 * @param type	Container type.
			 * @return		Closing character.
			 */
			static constexpr const char EndCharacter(const ContainerType& type) noexcept {
				switch (type) {
					case ContainerType::Group: return '}';
					case ContainerType::List:  return ']';
					default:                   return '\0';
				}
			}

			/**
			 * @brief Gets the container type.
			 * @return	Container type.
			 */
			constexpr virtual Item::ContainerType ContainerType() const noexcept = 0;

			/**
			 * @brief Gets the container type as string.
			 * @return	Container type as string.
			 */
			constexpr std::string_view ContainerTypeToString() const noexcept {
				return Item::TypeToString(this->ContainerType());
			}

			/**
			 * @brief Gets the item type.
			 * @return	Item type.
			 */
			constexpr Item::Type Type() const noexcept override {
				return Item::Type::Container;
			}
			/** @} */

			/**
			 * @name Items
			 * @{
			 */
			/**
			 * @brief Adds an item (const reference) using an explicit policy.
			 * @param item			Item to add.
			 * @param on_existing	Action to take if the item already exists.
			 * @return				Reference to the added item.
			 */
			inline Base& Add(const Base& item, const StormByte::Config::OnExistingAction& on_existing) {
				return Add(std::move(*item.Clone()), on_existing);
			}

			/**
			 * @brief Adds an item (rvalue) using an explicit policy.
			 * @param item			Item to add.
			 * @param on_existing	Action to take if the item already exists.
			 * @return				Reference to the added item.
			 */
			inline Base& Add(Base&& item, const StormByte::Config::OnExistingAction& on_existing) {
				return Add(item.Move(), on_existing);
			}

			/**
			 * @brief Adds an item using the container's current OnExistingAction policy.
			 * @param item	Item to add.
			 * @return		Reference to the added item.
			 */
			inline Base& Add(const Base& item) {
				return Add(std::move(*item.Clone()), m_on_existing_action);
			}

			/**
			 * @brief Adds an item using the container's current OnExistingAction policy (move).
			 * @param item	Item to add.
			 * @return		Reference to the added item.
			 */
			inline Base& Add(Base&& item) {
				return Add(item.Move(), m_on_existing_action);
			}

			/**
			 * @brief Adds an item to the container using current policy.
			 * @param item	The item to add.
			 * @return		Reference to the added item.
			 */
			inline Base& Add(Base::PointerType item) {
				return Add(item, m_on_existing_action);
			}

			/**
			 * @brief Adds an item to the container.
			 * @param item			The item to add.
			 * @param on_existing	The action to take if the item already exists.
			 * @return				A reference to the added item.
			 */
			Base& Add(Base::PointerType item, const StormByte::Config::OnExistingAction& on_existing);

			/**
			 * @brief Clears all items from the container.
			 */
			void Clear() noexcept;

			/**
			 * @brief Checks if an item exists by path.
			 * @param path	Path to the item.
			 * @return		true if the item exists.
			 */
			bool Exists(const StormByte::Safe::String& path) const;

			/**
			 * @brief Checks if an item exists by a path view.
			 * @param path	Path to the item.
			 * @return		true if the item exists.
			 */
			inline bool Exists(std::string_view path) const {
				return Exists(StormByte::Safe::String(path));
			}

			/**
			 * @brief Removes an item by index.
			 * @param index	Index of the item to remove.
			 * @throw OutOfBounds if index is out of bounds.
			 */
			void Remove(const StormByte::Size& index);

			/**
			 * @brief Removes an item by path.
			 * @param path	Path to the item to remove.
			 * @throw InvalidPath if path is invalid.
			 * @throw ItemNotFound if item is not found.
			 */
			void Remove(const StormByte::Safe::String& path);

			/**
			 * @brief Removes an item by a path view.
			 * @param path	Path to the item to remove.
			 */
			inline void Remove(std::string_view path) {
				Remove(StormByte::Safe::String(path));
			}

			/**
			 * @brief Items in this level. The span cannot reseat or grow the store; each pointer's item is mutable.
			 * @return	Span of item pointers.
			 */
			constexpr std::span<const Base::PointerType> Items() noexcept {
				return std::span<const Base::PointerType>(m_items);
			}

			/**
			 * @brief Items in this level.
			 * @return	Span of item pointers.
			 */
			constexpr std::span<const Base::PointerType> Items() const noexcept {
				return std::span<const Base::PointerType>(m_items);
			}

			/**
			 * @brief Gets the number of items in the current level.
			 * @return	Number of items.
			 */
			StormByte::Size Size() const noexcept;

			/**
			 * @brief Gets the total number of items including nested ones.
			 * @return	Total number of items.
			 */
			StormByte::Size Count() const noexcept;
			/** @} */

			/**
			 * @name Policy
			 * @{
			 */
			/**
			 * @brief Sets the action to take when an item with the same identity already exists.
			 * @param action	The policy to apply on name/identity collision.
			 */
			void SetOnExistingAction(StormByte::Config::OnExistingAction action) noexcept {
				m_on_existing_action = action;
			}

			/**
			 * @brief Gets the current OnExistingAction policy.
			 * @return	The current policy.
			 */
			StormByte::Config::OnExistingAction GetOnExistingAction() const noexcept {
				return m_on_existing_action;
			}
			/** @} */

		protected:
			std::vector<Base::PointerType> m_items;	///< Items stored in the container
			StormByte::Config::OnExistingAction m_on_existing_action = StormByte::Config::OnExistingAction::ThrowException;	///< Collision policy

			/**
			 * @brief Actions performed before adding an item.
			 * @param item			Item to check.
			 * @param onexisting	Action to take if the item already exists.
			 * @return				Pointer to the existing item if kept, otherwise nullptr.
			 */
			virtual Base::PointerType BeforeAdditionActions(Base::PointerType item, const StormByte::Config::OnExistingAction onexisting) = 0;

		private:
			/**
			 * @brief Internal helper that serializes the contents of the container.
			 * @param level	Indentation level.
			 * @return		Serialized contents.
			 */
			virtual StormByte::Safe::String ContentsToString(const int& level) const noexcept;

			/**
			 * @brief Checks whether a path is syntactically valid.
			 * @param name	Path to validate.
			 * @return		true if the path is valid.
			 */
			static bool IsPathValid(const StormByte::Safe::String& name) noexcept;

			/**
			 * @brief Looks up a child item by path.
			 * @param path	Path to the child.
			 * @return		Const reference to the found item.
			 */
			const Base& LookUp(const StormByte::Safe::String& path) const;

			/**
			 * @brief Looks up a child item by path (queue version).
			 * @param path	Path components.
			 * @return		Const reference to the found item.
			 */
			const Base& LookUp(std::queue<StormByte::Safe::String>& path) const;

			/**
			 * @brief Removes an item by path (queue version).
			 * @param path	Path components.
			 */
			void Remove(std::queue<StormByte::Safe::String>& path);
	};
}
