# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Summary]

StormByte Config is the configuration-document module of the StormByte C++ suite.

It depends on StormByte Base ≥ 2.0.0 and StormByte String ≥ 1.0.0. This repository is not Base, Buffer, Crypto, Database, Logger, Multimedia, Network or System.

Public headers under `StormByte/config/` cover text and versioned binary documents: values, groups, lists, comments, hooks and `Save` / `Load`.

If you landed here from a release link and have not read the tree:

- What this module is, how to build it, and short examples: [README.md](https://github.com/StormBytePP/StormByte-Config/blob/master/README.md)
- License: GNU Lesser General Public License version 3 or later, [LICENSE](https://github.com/StormBytePP/StormByte-Config/blob/master/LICENSE)

## [Unreleased]

[Unreleased]: https://github.com/StormBytePP/StormByte-Config/compare/v2.0.0...HEAD

## [2.0.0] - 2026-09-27

### Changed

- **Breaking:** `Item::Value<T>` is gone. There is one concrete `Item::Value` leaf. Typed access is `Base::As<T>()`: node types (`Value`, `Group`, `List`, `Comment<...>`) or leaf tags (`Integer`, `Double`, `Bool`, `Text`, `Binary`). `item.As<Value>() = 3.65` and `int i = item.As<Integer>()` are the public getters. Integer promotes to Double; Double does not narrow to Integer.
- **Breaking:** Public binary payloads are `StormByte::BinaryData`, not `std::vector<std::byte>`. Text still uses Base64 `b"..."`; the binary writer emits `Serializable<BinaryData>`.
- **Breaking:** Counts and container indices use `StormByte::Size`. `operator[]` on `List` / `Group` is `Size`, not `size_t`.
- **Breaking:** Item and Config destructors are out of line in this module so `catch` and `typeid` stay on one CRT across a DLL.
- **Breaking:** `StormByte::Config::Exception` takes `StormByte::Exception::Path`. `Component` is gone. `what()` is `StormByte.Config: message`. Destructors are defined in this module so `catch` matches across a DLL.
- Names, paths, `Group`, `List` and `Value` accept `std::string_view` (a literal or a `std::string`). The `StormByte::String::String` is built inside Config. String values are `StormByte::String::String`, not `std::string`. A custom exception path is a view plus a plain message; `StormByte::Exception::Path` stays protected.
- **Breaking:** `Item::Base` is `Clonable<Base, StormByte::Shared<Base>>`. `PointerType` is no longer `std::shared_ptr`. Construction goes through `MakePointer`. `Serializable::Serialize` is `BinaryData`; the on-disk reader buffer stays a local `std::vector<std::byte>`.
- **Breaking:** Read and failure hooks are no longer `std::function`. Stateless hooks are function pointers (`void (*)(Item::Group&)`, `bool (*)(const Item::Group&)`). Stateful hooks derive from `ReadHook` / `FailureHook` (`Clonable` + `Shared`, `MakePointer`). `AddHookBeforeRead` / `AddHookAfterRead` / `OnParseFailure` are out of line in this module. Capturing lambdas do not bind; wrap state in a hook class.
- Requires [StormByte Base](https://github.com/StormBytePP/StormByte/releases/tag/2.0.0) ≥ 2.0.0 and [StormByte String](https://github.com/StormBytePP/StormByte-String/releases/tag/1.0.0) ≥ 1.0.0.

[2.0.0]: https://github.com/StormBytePP/StormByte-Config/releases/tag/2.0.0

## [1.1.0] - 2026-09-13

### Added

- **`Add` pointer overloads**: Added `Add(Base::PointerType)` overloads to `Config` and `Container` for direct item pointer insertion.

### Fixed

- **Container path exception safety**: Prevented stock C++ exception leaks (`std::out_of_range`, `std::invalid_argument`) when looking up or removing numeric or malformed container paths, throwing `StormByte::Config::OutOfBounds` or `StormByte::Config::InvalidPath` instead.
- **Double precision serialization**: Switched `Value<double>::Serialize` to `std::format` for full floating-point precision and ensured integer-like floating point values maintain a `.0` suffix so they are recognized as Double tokens by the parser.
- **Container null item protection**: Added null pointer check at the start of `Container::Add` to prevent null pointer dereferences when propagating container policies.

### Changed

- **Exception handling**: Ported `StormByte::Config::Exception` constructors to pass `StormByte::Component("Config")` introduced in StormByte Base 1.1.0, automatically formatting exception messages with `StormByte::Config: `.
- **Dependencies**: Requires [StormByte Base](https://github.com/StormBytePP/StormByte/releases/tag/1.1.0) ≥ 1.1.0.
- **Type traits**: Replaced stock C++ type traits (`std::is_same_v`, `std::is_base_of_v`) with StormByte flavor equivalents (`StormByte::Type::SameAs`, `StormByte::Type::DerivedFrom`).

[1.1.0]: https://github.com/StormBytePP/StormByte-Config/releases/tag/1.1.0

## [1.0.0] - 2026-09-05

Initial public release of StormByte Config.

### Added

- `Config` document with `operator<<` / `operator>>` for text
- `Save` / `Load` with `Mode::Text` and `Mode::Binary`
- Versioned binary envelope: magic `STBTCF` + format version + payload
- Values: string, integer, double, boolean, binary (`std::vector<std::byte>`)
- Text binary values as Base64 `b"..."`; raw bytes on the binary wire
- Comments: `#`, `//`, `/* */`
- Containers: lists `[]` and groups `{}`
- Pre/post read hooks (`AddHookBeforeRead` / `AddHookAfterRead`)
- `OnExistingAction` (`Keep`, `Overwrite`, `ThrowException`; default is throw)
- Project version read from the `VERSION` file
- CMake 3.28 floor

### Notes

- Stream operators are text-only. Use `Save` / `Load` for binary.
- Older supported binary versions load fully; newer versions are rejected.
- Save always writes the current format version.
- Needs a C++26 compiler and StormByte Base ≥ 1.0.0.

[1.0.0]: https://github.com/StormBytePP/StormByte-Config/releases/tag/1.0.0
