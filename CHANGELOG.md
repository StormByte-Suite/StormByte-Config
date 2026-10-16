# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Summary]

StormByte Config is the configuration-document module of the StormByte C++ suite.

It depends on StormByte Base 2.1.0 and StormByte-BuildMaster >= 2.1.0. This repository is not Base, Buffer, Crypto, Database, Logger, Multimedia, Network or System.

Public headers under `StormByte/config/` cover text and versioned binary documents: values, groups, lists, comments, hooks and `Save` / `Load`.

If you landed here from a release link and have not read the tree:

- What this module is, how to build it, and short examples: [README.md](https://github.com/StormByte-Suite/StormByte-Config/blob/master/README.md)
- License: GNU Lesser General Public License version 3 or later, [LICENSE](https://github.com/StormByte-Suite/StormByte-Config/blob/master/LICENSE)

## [Unreleased]

### Fixed

- `Value::Serialize` writes a double with six digits after the decimal on every standard library. `ReplaceToken` uses `std::to_string`, and libc++ prints the shortest round-trip (`3.141592653589793`, `42`), so the text was not `3.141593` and `42.000000`. The digits now come from `std::to_chars` with a fixed format and are passed to `ReplaceToken` as text.

[Unreleased]: https://github.com/StormByte-Suite/StormByte-Config/compare/v2.0.2...HEAD

## [2.0.2] - 2026-10-16

### Added

- `ENABLE_PGO` and `ENABLE_LTO`. Profile-guided optimization trains in `pgo-instr` and installs this build. Link-time optimization follows `ENABLE_LTO` (default ON) for every build type. Both apply to Config. Base is not rebuilt.

### Changed

- Requires [StormByte-BuildMaster](https://github.com/StormByte-Suite/StormByte-BuildMaster) >= 2.1.0. An older install is not used; the `buildmaster` submodule supplies 2.1.0.
- Requires [StormByte Base](https://github.com/StormByte-Suite/StormByte/releases/tag/2.1.0) 2.1.0. The build downloads that release archive (`stormbyte_component_url`) and links it with `BACKEND=binary`. The Base submodule is gone. The archive matches this OS and C++ runtime: `libstdc++` or `libc++` on Linux and macOS, no runtime suffix on Windows.
- **Breaking:** `Config::Exception` no longer formats with `std::format`. A message with arguments replaces literal `{}` through `Safe::String::ReplaceToken`, the same rule as Base 2.1.0. `what()` stays `StormByte.Config: message`, and `Exception::Path` still appends a segment under `Config`.
- `Value::Serialize` writes an integer or a double with `Safe::String::ReplaceToken`, which uses `std::to_string`. A double is six digits after the decimal (`42.0` is `42.000000`), not the shorter `std::format` text. A value with no decimal point still gets `.0`.
- Bumped version to 2.0.2.

### Fixed

- `cmake/optimizations.cmake` is the same file as Base. On Release, and only when that language was not set from the outside, it appends `OPTIMIZATION_FLAGS_LINUX`, `OPTIMIZATION_FLAGS_MAC` or `OPTIMIZATION_FLAGS_WINDOWS`. Linux is `-O2 -march=x86-64-v3`, Apple is `-O2 -mcpu=apple-m1`, Windows is `/O2 /arch:AVX2`. There is no `/fp:fast` and no `/EHsc`. `cmake/pgo.cmake` is the same file as Base, so a forwarded list keeps its semicolons. MSVC PGO links with `/LTCG:PGINSTRUMENT` and `/LTCG:PGOPTIMIZE`. `/GENPROFILE` alone fails with LNK1220 because the current `link.exe` requires an `/LTCG` specification, and CMake's own `/LTCG` stays off so it does not replace the profile mode.
- The Base archive follows the C++ library this compiler actually uses. A Gentoo clang config that selects libc++ does not appear in `CMAKE_CXX_FLAGS`, so the helper used to download the libstdc++ zip and the tests failed to link. It now preprocesses one line with that compiler and reads `_LIBCPP_VERSION` or `__GLIBCXX__`.

[2.0.2]: https://github.com/StormByte-Suite/StormByte-Config/releases/tag/2.0.2

## [2.0.1] - 2026-10-13

### Changed

- Updated dependency pins.
- Bumped version to 2.0.1.

[2.0.1]: https://github.com/StormByte-Suite/StormByte-Config/releases/tag/2.0.1

## [2.0.0] - 2026-10-10

### Changed

- Config item classes are registered with Base's `MaybeSafe` trait when their Base-owned storage and out-of-line destruction satisfy the documented DLL-boundary contract. Plain exception messages accept `std::string_view` and are copied into Base-owned exception storage inside the Config module.
- Shared vs static follows CMake `BUILD_SHARED_LIBS` (declared in the project root, default ON). There is no `STORMBYTE_CONFIG_SHARED` CMake option. When the library is shared, the compile definition `STORMBYTE_CONFIG_SHARED` is still set so `visibility.h` can distinguish `dllexport` / `dllimport` / static. CI passes `-DBUILD_SHARED_LIBS=ON`.
- **Breaking: Item model and storage APIs:**
	- `Item::Value<T>` is gone. There is one concrete `Item::Value` leaf. Typed access is `Base::As<T>()`: node types (`Value`, `Group`, `List`, `Comment<...>`) or leaf tags (`Integer`, `Double`, `Bool`, `Text`, `Binary`). `item.As<Value>() = 3.65` and `int i = item.As<Integer>()` are the public getters. Integer promotes to Double; Double does not narrow to Integer.
	- Public binary payloads are `StormByte::Safe::Binary`, not `std::vector<std::byte>`. Text still uses Base64 `b"..."`; the binary writer emits `Serializable<Safe::Binary>`.
	- Counts and container indices use `StormByte::Size`. `operator[]` on `List` / `Group` is `Size`, not `size_t`.
	- `Item::Base` is `Clonable<Base, StormByte::Safe::Shared<Base>>`. `PointerType` is no longer `std::shared_ptr`. Construction goes through `MakePointer`. `Serializable::Serialize` is `Safe::Binary`; the on-disk reader buffer stays a local `std::vector<std::byte>`.
	- Configuration names, text values and comments use `StormByte::Safe::String` from Base. Text binary serialization uses Base's `Serializable<Safe::String>` codec. The scalar payload is a `Safe::Variant`.
	- Item collections use `StormByte::Safe::Vector`; path traversal uses `StormByte::Safe::Queue`. Public STL convenience adapters run on the consumer side; text, binary data and expected results use Base-owned storage across DLL boundaries.
- **Breaking: DLL-boundary lifetime and exception contracts:**
	- Item and Config destructors are out of line in this module so `catch` and `typeid` stay on one CRT across a DLL.
	- `StormByte::Config::Exception` uses its own explicit `Path` wrapper for additional segments nested below `StormByte.Config`; a custom segment can never replace `Config`. `Component` is gone, and `what()` starts with `StormByte.Config`.
	- Item classes and `Config` are registered with Base's `MaybeSafe` trait only where construction, copy/move, assignment, destruction, storage ownership, and heap-affecting clone operations meet its documented provider contract. Plain Config exception messages accept `std::string_view` and are copied inside the Config module.
- **Breaking: Read and failure hook API:** Hooks use copyable `StormByte::Safe::Function` values. Read callbacks receive a Base-owned `Safe::Shared<Item::Group>` handle and commit mutations only when the callback succeeds; failure callbacks receive a Base-owned const group handle and return whether the parse error should propagate. `MakeReadHook` / `MakeFailureHook` keep callable context cloning and release in the consumer module, including for capturing lambdas. Hook collections use Base-owned `Safe::Vector` storage. Hook objects are built with `Safe::MakeShared`.
- Names, paths, `Group`, `List` and `Value` accept `std::string_view` (a literal, a `std::string` or a `Safe::String`). `Item::Value` also takes `const char*` for a text literal, because a pointer would otherwise prefer `bool`. A custom Config exception path is an `Exception::Path` plus a plain message.
- Requires [StormByte Base](https://github.com/StormByte-Suite/StormByte/releases/tag/2.0.0) ≥ 2.0.0.

[2.0.0]: https://github.com/StormByte-Suite/StormByte-Config/releases/tag/2.0.0

## [1.1.0] - 2026-09-13

### Added

- **`Add` pointer overloads**: Added `Add(Base::PointerType)` overloads to `Config` and `Container` for direct item pointer insertion.

### Fixed

- **Container path exception safety**: Prevented stock C++ exception leaks (`std::out_of_range`, `std::invalid_argument`) when looking up or removing numeric or malformed container paths, throwing `StormByte::Config::OutOfBounds` or `StormByte::Config::InvalidPath` instead.
- **Double precision serialization**: Switched `Value<double>::Serialize` to `std::format` for full floating-point precision and ensured integer-like floating point values maintain a `.0` suffix so they are recognized as Double tokens by the parser.
- **Container null item protection**: Added null pointer check at the start of `Container::Add` to prevent null pointer dereferences when propagating container policies.

### Changed

- **Exception handling**: Ported `StormByte::Config::Exception` constructors to pass `StormByte::Component("Config")` introduced in StormByte Base 1.1.0, automatically formatting exception messages with `StormByte::Config: `.
- **Dependencies**: Requires [StormByte Base](https://github.com/StormByte-Suite/StormByte/releases/tag/1.1.0) ≥ 1.1.0.
- **Type traits**: Replaced stock C++ type traits (`std::is_same_v`, `std::is_base_of_v`) with StormByte flavor equivalents (`StormByte::Type::SameAs`, `StormByte::Type::DerivedFrom`).

[1.1.0]: https://github.com/StormByte-Suite/StormByte-Config/releases/tag/1.1.0

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

[1.0.0]: https://github.com/StormByte-Suite/StormByte-Config/releases/tag/1.0.0
