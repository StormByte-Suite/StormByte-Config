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

#include <StormByte/config/item/container.hxx>
#include <StormByte/safe/string.hxx>

#include <algorithm>
#include <string>
#include <string_view>

using namespace StormByte::Config::Item;

namespace {
	StormByte::Safe::String Indent(const int indent_level) {
		const int level = indent_level > 0 ? indent_level : 0;
		return StormByte::Safe::String(std::string(static_cast<std::size_t>(level), '\t'));
	}
}

Container::Container() = default;

Container::Container(const StormByte::Safe::String& name): Base(name) {}

Container::Container(StormByte::Safe::String&& name): Base(std::move(name)) {}

Container::Container(const Container& base) = default;

Container::Container(Container&& base) noexcept = default;

Container& Container::operator=(const Container& base) = default;

Container& Container::operator=(Container&& base) noexcept = default;

Container::~Container() noexcept = default;

bool Container::Equals(const Base& other) const noexcept {
	if (this->Type() != other.Type())
		return false;
	if (this->Name() != other.Name())
		return false;
	const Container& other_container = static_cast<const Container&>(other);
	if (this->ContainerType() != other_container.ContainerType())
		return false;
	if (m_items.size() != other_container.m_items.size())
		return false;
	for (std::size_t i = 0; i < m_items.size(); ++i) {
		if (*m_items[i] != *other_container.m_items[i])
			return false;
	}
	return true;
}

bool Container::operator==(const Container& container) const noexcept {
	return Equals(container);
}

Base& Container::operator[](const StormByte::Size& index) {
	return const_cast<Base&>(static_cast<const Container&>(*this)[index]);
}

const Base& Container::operator[](const StormByte::Size& index) const {
	const std::size_t raw = static_cast<std::size_t>(index);
	if (raw >= m_items.size())
		throw OutOfBounds("Index {} is out of bounds (max size is {})", raw, m_items.size());
	return *m_items[raw];
}

Base& Container::operator[](const StormByte::Safe::String& path) {
	return const_cast<Base&>(static_cast<const Container&>(*this)[path]);
}

Base& Container::Add(Base::PointerType item, const StormByte::Config::OnExistingAction& on_existing) {
	if (!item)
		throw Exception("Tried to add a null pointer to Container!");

	if (item->Type() == Type::Container) {
		auto& child = static_cast<Container&>(*item);
		child.SetOnExistingAction(on_existing);
	}

	Base::PointerType i = this->BeforeAdditionActions(item, on_existing);
	if (i)
		return *i;
	m_items.push_back(item);
	const Base::PointerType added = m_items.back();
	return *added;
}

void Container::Clear() noexcept {
	m_items.clear();
}

bool Container::Exists(const StormByte::Safe::String& path) const {
	try {
		LookUp(path);
		return true;
	} catch (const Exception&) {
		return false;
	}
}

void Container::Remove(const StormByte::Size& index) {
	const std::size_t raw = static_cast<std::size_t>(index);
	if (raw >= m_items.size())
		throw OutOfBounds("Index {} is out of bounds (max size: {})", raw, m_items.size());
	m_items.erase(m_items.begin() + static_cast<std::ptrdiff_t>(raw));
}

void Container::Remove(const StormByte::Safe::String& path) {
	if (!IsPathValid(path))
		throw InvalidPath("Invalid path '{}'", static_cast<std::string_view>(path));
	StormByte::Safe::Queue<StormByte::Safe::String> path_queue;
	if (path.Explode('/', path_queue) != StormByte::Safe::Status::Success)
		throw InvalidPath("Failed to split removal path");
	Remove(path_queue);
}

StormByte::Safe::String Container::Serialize(const int& indent_level) const noexcept {
	const auto enclosure_characters = EnclosureCharacters(ContainerType());
	std::string serial = static_cast<std::string>(Base::Serialize(indent_level));
	serial += enclosure_characters.first;
	serial += '\n';
	serial += static_cast<std::string>(ContentsToString(indent_level + 1));
	serial += static_cast<std::string>(Indent(indent_level));
	serial += enclosure_characters.second;
	return StormByte::Safe::String(std::string_view(serial));
}

StormByte::Size Container::Size() const noexcept {
	return StormByte::Size{ m_items.size() };
}

StormByte::Size Container::Count() const noexcept {
	std::size_t count = 0;
	for (const auto& item : m_items) {
		if (item->Type() == Type::Container)
			count += 1 + static_cast<std::size_t>(static_cast<const Container&>(*item).Count());
		else
			++count;
	}
	return StormByte::Size{ count };
}

StormByte::Safe::String Container::ContentsToString(const int& indent_level) const noexcept {
	std::string serial;
	for (const auto& item : m_items) {
		serial += static_cast<std::string>(item->Serialize(indent_level));
		serial += '\n';
	}
	return StormByte::Safe::String(std::string_view(serial));
}

bool Container::IsPathValid(const StormByte::Safe::String& name) noexcept {
	const std::string_view text(name);
	if (text.empty())
		return false;

	auto segment = [](unsigned char c) noexcept {
		return (c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z')
			|| (c >= '0' && c <= '9')
			|| c == '_';
	};

	bool in_segment = false;
	for (const unsigned char c : text) {
		if (c == '/') {
			if (!in_segment)
				return false;
			in_segment = false;
			continue;
		}
		if (!segment(c))
			return false;
		in_segment = true;
	}
	return in_segment;
}

const Base& Container::LookUp(const StormByte::Safe::String& path) const {
	if (!IsPathValid(path))
		throw InvalidPath("Invalid path '{}'", static_cast<std::string_view>(path));
	StormByte::Safe::Queue<StormByte::Safe::String> path_queue;
	if (path.Explode('/', path_queue) != StormByte::Safe::Status::Success)
		throw InvalidPath("Failed to split lookup path");
	return LookUp(path_queue);
}

const Base& Container::LookUp(StormByte::Safe::Queue<StormByte::Safe::String>& path) const {
	if (path.empty())
		throw InvalidPath("Empty path given for lookup");

	const StormByte::Safe::String item_path = path.front();
	path.pop();
	const std::string item_text = static_cast<std::string>(item_path);
	if (path.empty()) {
		if (item_path.IsInteger()) {
			try {
				int idx = std::stoi(item_text);
				if (idx < 0)
					throw OutOfBounds("Index {} is out of bounds", idx);
				return operator[](StormByte::Size{ static_cast<std::size_t>(idx) });
			} catch (const OutOfBounds&) {
				throw;
			} catch (const std::exception&) {
				throw OutOfBounds("Invalid or out of bounds index '{}'", item_text);
			}
		}
		const auto it = std::find_if(m_items.begin(), m_items.end(), [&item_path](const Base::PointerType& item) {
			return item->Name() == item_path;
		});
		if (it != m_items.end())
			return **it;
		throw ItemNotFound("Item '{}' not found", item_text);
	}

	StormByte::Size idx{ 0 };
	const bool is_num = item_path.IsInteger();
	if (is_num) {
		try {
			int num = std::stoi(item_text);
			if (num < 0)
				throw OutOfBounds("Index {} is out of bounds", num);
			idx = StormByte::Size{ static_cast<std::size_t>(num) };
		} catch (const OutOfBounds&) {
			throw;
		} catch (const std::exception&) {
			throw OutOfBounds("Invalid or out of bounds index '{}'", item_text);
		}
	}

	const Base& item = is_num ? operator[](idx) : operator[](item_path);
	if (item.Type() != Type::Container)
		throw Exception("Lookup path {} applied to non container item", item_text);
	return static_cast<const Container&>(item).LookUp(path);
}

void Container::Remove(StormByte::Safe::Queue<StormByte::Safe::String>& path) {
	if (path.empty())
		throw InvalidPath("Empty path given for remove");

	StormByte::Safe::String item_path = path.front();
	path.pop();
	const std::string item_text = static_cast<std::string>(item_path);
	if (path.empty()) {
		if (item_path.IsInteger()) {
			try {
				int idx = std::stoi(item_text);
				if (idx < 0)
					throw OutOfBounds("Index {} is out of bounds", idx);
				Remove(StormByte::Size{ static_cast<std::size_t>(idx) });
			} catch (const OutOfBounds&) {
				throw;
			} catch (const std::exception&) {
				throw OutOfBounds("Invalid or out of bounds index '{}'", item_text);
			}
			return;
		}
		const auto it = std::find_if(m_items.begin(), m_items.end(), [&item_path](const Base::PointerType& item) {
			return item->Name() == item_path;
		});
		if (it != m_items.end())
			m_items.erase(it);
		else
			throw ItemNotFound("Item '{}' not found", item_text);
		return;
	}

	StormByte::Size idx{ 0 };
	const bool is_num = item_path.IsInteger();
	if (is_num) {
		try {
			int num = std::stoi(item_text);
			if (num < 0)
				throw OutOfBounds("Index {} is out of bounds", num);
			idx = StormByte::Size{ static_cast<std::size_t>(num) };
		} catch (const OutOfBounds&) {
			throw;
		} catch (const std::exception&) {
			throw OutOfBounds("Invalid or out of bounds index '{}'", item_text);
		}
	}

	Base& item = is_num ? operator[](idx) : operator[](item_path);
	if (item.Type() != Type::Container)
		throw Exception("Lookup path {} applied to non container item", item_text);
	static_cast<Container&>(item).Remove(path);
}
