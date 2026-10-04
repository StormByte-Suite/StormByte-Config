# StormByte-Config

![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey)
![C++26](https://img.shields.io/badge/C%2B%2B-26-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.28+-064F8C?logo=cmake&logoColor=white)
![License: LGPL v3 or commercial](https://img.shields.io/badge/License-LGPL_v3_or_commercial-blue.svg)
[![CI](https://github.com/StormByte-Suite/StormByte-Config/actions/workflows/ci.yml/badge.svg)](https://github.com/StormByte-Suite/StormByte-Config/actions/workflows/ci.yml)
[![Sponsor](https://img.shields.io/badge/Sponsor-StormBytePP-ea4aaa?logo=githubsponsors)](https://github.com/sponsors/StormBytePP)

This repository is **StormByte Config**: human-readable text and versioned binary documents for the StormByte C++ suite.

It depends on [StormByte Base](https://github.com/StormByte-Suite/StormByte) ≥ 2.0.0. Public headers live under `StormByte/config/` and cover the document, items (values, comments, groups, lists), `Save` / `Load`, and collision / hook policy.

The suite is split on purpose. Base, Buffer, Crypto, Database, Logger, Multimedia, Network and System are **other repositories**. This one does not implement them.

## What this module does

- **Text and binary I/O** — `Save` / `Load` with `Mode::Text` or `Mode::Binary` on any `std::ostream` / `std::istream`. Stream operators stay text-only.
- **Versioned binary** — magic `STBTCF` + format version. Older layouts load; newer ones are rejected; save always writes the current version.
- **Values** — one concrete `Item::Value` (text, integer, double, boolean, `StormByte::BinaryData`). Access is `Base::As<T>()`. Text binary form is Base64 `b"..."`; the binary document stores raw bytes.
- **Comments** — `#`, `//`, `/* */`.
- **Containers** — lists `[]` and groups `{}`. Counts and indices use `StormByte::Size`.
- **Hooks** — `AddHookBeforeRead` / `AddHookAfterRead` / `OnParseFailure`. Stateless hooks are function pointers. Stateful hooks derive from `ReadHook` / `FailureHook` and are built with `MakePointer`.
- **On existing** — `Keep`, `Overwrite`, or `ThrowException` (default).
- **Heap** — items are `Clonable<Base, Shared<Base>>`. Build them with `MakePointer` when you hold `PointerType`.

## The rest of the suite

| Module | Role | API |
| --- | --- | --- |
| [Base](https://github.com/StormByte-Suite/StormByte) | Exceptions, Expected, serialization, strings, UUID, concepts | [/StormByte](http://suite.stormbyte.org/StormByte) |
| [Buffer](https://github.com/StormByte-Suite/StormByte-Buffer) | FIFO, SharedFIFO, Ring, Producer/Consumer and multi-stage pipelines | [/StormByte-Buffer](http://suite.stormbyte.org/StormByte-Buffer) |
| **Config** | This repository | [/StormByte-Config](http://suite.stormbyte.org/StormByte-Config) |
| [Crypto](https://github.com/StormByte-Suite/StormByte-Crypto) | Hash, compress, encrypt, sign and key agreement — Crypto++ never leaves the private tree | [/StormByte-Crypto](http://suite.stormbyte.org/StormByte-Crypto) |
| [Database](https://github.com/StormByte-Suite/StormByte-Database) | One API over SQLite, PostgreSQL and MariaDB | [/StormByte-Database](http://suite.stormbyte.org/StormByte-Database) |
| [Logger](https://github.com/StormByte-Suite/StormByte-Logger) | Stream logger with levels, headers, human-readable sizes and redaction (`ThreadedLog`) | [/StormByte-Logger](http://suite.stormbyte.org/StormByte-Logger) |
| [Multimedia](https://github.com/StormByte-Suite/StormByte-Multimedia) | Decode, encode and containers without raw FFmpeg types; codecs enabled only if present | [/StormByte-Multimedia](http://suite.stormbyte.org/StormByte-Multimedia) |
| [Network](https://github.com/StormByte-Suite/StormByte-Network) | Framed packets, Client/Server, IPv4/IPv6 TCP and Buffer pipelines (compress/encrypt) | [/StormByte-Network](http://suite.stormbyte.org/StormByte-Network) |
| [System](https://github.com/StormByte-Suite/StormByte-System) | Processes, pipes and environment variables across Linux, Windows and macOS | [/StormByte-System](http://suite.stormbyte.org/StormByte-System) |

## Table of Contents

- [What this module does](#what-this-module-does)
- [The rest of the suite](#the-rest-of-the-suite)
- [Installation](#installation)
- [Usage](#usage)
  - [Load from a stream](#load-from-a-stream)
  - [Build a document](#build-a-document)
  - [As](#as)
  - [Hooks](#hooks)
  - [Binary Save / Load](#binary-save--load)
  - [Text syntax](#text-syntax)
- [Contributing](#contributing)
- [License](#license)
- [Support](#support)

## Installation

Needs a C++26 compiler, CMake 3.28 or newer, and [StormByte Base](https://github.com/StormByte-Suite/StormByte/releases/tag/2.0.0) ≥ 2.0.0.

```sh
git clone --recursive https://github.com/StormByte-Suite/StormByte-Config.git
cd StormByte-Config
cmake -S . -B build
cmake --build build
```

Shared vs static follows CMake `BUILD_SHARED_LIBS` (declared in `lib/`, default ON). A plain configure builds the shared library. `-DBUILD_SHARED_LIBS=OFF` builds a static archive; on Windows the headers then do not use `dllimport`. Vendored StormByte Base follows the same mode.

A shared build keeps this library as its own `.so` / `.dll`. Under the LGPL that is usually the simpler way to ship: the user can replace that file. A static archive is folded into your binary. The LGPL still applies to this code; you must give the recipient a way to relink your product with a different build of this library. If that does not fit how you distribute the final product, a commercial license is available from the copyright holder (see [License](#license)).

Link `StormByte-Config` (and String / Base). Include path: the public install prefix, headers as `#include <StormByte/config/….hxx>`.

## Usage

Headers are `#include <StormByte/config/….hxx>`. Namespace root is `StormByte::Config`.

### Load from a stream

```cpp
#include <StormByte/config/config.hxx>
#include <fstream>
#include <iostream>

using namespace StormByte::Config;

int main() {
	Config config;
	std::ifstream file("config.cfg");
	file >> config;

	const int timeout = config["timeout"].As<Item::Integer>();
	const auto& user = config["settings/username"].As<Item::Text>();
	std::cout << user << " " << timeout << std::endl;
}
```

Existing keys: `OnExistingAction` (`Keep`, `Overwrite`, `ThrowException`; default is throw).

### Build a document

```cpp
#include <StormByte/config/config.hxx>
#include <StormByte/binary_data.hxx>

using namespace StormByte::Config;

Config config;
config.Add(Item::Value("username", "example_user"));
config.Add(Item::Value("timeout", 30));
config.Add(Item::Value("feature_timeout", 60.5));
config.Add(Item::Value("enabled", true));
config.Add(Item::Value("payload", StormByte::BinaryData({
	std::byte{'H'}, std::byte{'i'}
})));

Item::Group& settings = config.Add(Item::Group("settings")).As<Item::Group>();
settings.Add(Item::Value("retries", 3));

Item::List& numbers = config.Add(Item::List("favorite_numbers")).As<Item::List>();
numbers.Add(Item::Value(3));
numbers.Add(Item::Value(14));
numbers.Add(Item::Value("pi constant"));

config.Add(Item::Comment<Item::CommentType::SingleLineBash>("bash comment"));
```

`Add` copies or moves the item onto the Config heap (`Shared<Base>`). After `Add`, look the node up and mutate it through `As`.

### As

`As<T>()` is the typed view of an item. `T` is either a node type or a leaf tag.

| `T` | Meaning |
| --- | --- |
| `Item::Value` | The scalar leaf. Assignment writes the payload. |
| `Item::Integer` | `int` |
| `Item::Double` | `double` (an Integer is accepted) |
| `Item::Bool` | `bool` |
| `Item::Text` | `StormByte::Safe::String` |
| `Item::Binary` | `StormByte::BinaryData` |
| `Item::Group` / `Item::List` | Containers |
| `Item::Comment<CommentType::…>` | Comment specializations |

```cpp
config["timeout"].As<Item::Integer>() = 45;
int n = config["timeout"].As<Item::Integer>();
double d = config["timeout"].As<Item::Double>();

config["username"].As<Item::Value>() = "other";
const auto& name = config["username"].As<Item::Text>();

Item::Group& settings = config["settings"].As<Item::Group>();
const int retries = settings["retries"].As<Item::Integer>();

const StormByte::BinaryData& bytes = config["payload"].As<Item::Binary>();
const auto first = numbers[StormByte::Size{0}].As<Item::Integer>();
```

Integer promotes to Double. Double does not narrow to Integer. A wrong tag throws `StormByte::Config::Exception`.

### Hooks

Hooks run only on **text** read (`operator<<` / `>>` from a stream or string). They do not run on binary `Load`.

- Before-read and after-read: `void (*)(Item::Group&)`. After-read runs only if parse succeeded.
- Parse failure: `bool (*)(const Item::Group&)`. Return `false` to swallow the error; `true` (or no hook) keeps the throw.
- Capturing lambdas and `std::function` are not accepted. A callback with no state is a function pointer. A callback with state is a class that derives from `ReadHook` or `FailureHook` and is created with `MakePointer`.

Stateless — inject a default timeout if the file omitted it, and refuse to throw on a known-bad lab fixture:

```cpp
#include <StormByte/config/config.hxx>
#include <StormByte/config/hook.hxx>
#include <sstream>

using namespace StormByte::Config;

void EnsureTimeout(Item::Group& root) {
	if (!root.Exists("timeout"))
		root.Add(Item::Value("timeout", 30));
}

bool IgnoreBrokenFixture(const Item::Group&) {
	return false;
}

void load_app_config(Config& config) {
	config.AddHookAfterRead(&EnsureTimeout);
	config.OnParseFailure(&IgnoreBrokenFixture);

	std::istringstream file("username = \"ada\"\n");
	file >> config;
	// config["timeout"] is 30 even though the file had no timeout key
}
```

Stateful — count how many documents a loader accepted and stamp the count into the tree:

```cpp
#include <StormByte/config/config.hxx>
#include <StormByte/config/hook.hxx>
#include <sstream>

using namespace StormByte::Config;

class LoadCounter final: public ReadHook {
	public:
		explicit LoadCounter(int& total) noexcept: m_total(total) {}

		LoadCounter(const LoadCounter&) noexcept = default;
		LoadCounter(LoadCounter&&) noexcept = default;
		LoadCounter& operator=(const LoadCounter&) = delete;
		LoadCounter& operator=(LoadCounter&&) = delete;
		~LoadCounter() noexcept override = default;

		PointerType Clone() const override {
			return MakePointer<LoadCounter>(*this);
		}

		PointerType Move() override {
			return MakePointer<LoadCounter>(std::move(*this));
		}

		void operator()(Item::Group& root) override {
			++m_total;
			root.Add(Item::Value("loads", m_total), OnExistingAction::Overwrite);
		}

	private:
		int& m_total;
};

void load_with_counter(Config& config, int& total) {
	config.AddHookAfterRead(ReadHook::MakePointer<LoadCounter>(total));
	std::istringstream file("username = \"ada\"\n");
	file >> config;
}
```

`MakeReadHook` / `MakeFailureHook` wrap a function pointer in the same `Shared` type when you already hold a `ReadHook::PointerType`.

### Binary Save / Load

```cpp
#include <StormByte/config/config.hxx>
#include <fstream>
#include <iostream>

using namespace StormByte::Config;

int main() {
	Config config;
	config.Add(Item::Value("username", "example_user"));
	config.Add(Item::Value("timeout", 30));

	{
		std::ofstream out("config.bin", std::ios::binary);
		config.Save(out, Mode::Binary);
	}

	std::ifstream in("config.bin", std::ios::binary);
	auto loaded = Config::Load(in, Mode::Binary);
	if (!loaded) {
		std::cerr << loaded.error()->what() << std::endl;
		return 1;
	}

	std::cout << loaded.value()["username"].As<Item::Text>() << std::endl;
	std::cout << loaded.value()["timeout"].As<Item::Integer>() << std::endl;
}
```

`Load` returns `ExpectedConfig` (`StormByte::Expected<Config, StormByte::Exception>`). A bad magic or a newer format version is an error, not a thrown parse of the payload. For callers passing data across a DLL boundary, use `Config::Binary()` / `Config::Load(const StormByte::BinaryData&)` or the `Safe::String` text overload; stream overloads remain caller-side adapters.

`Container::Items()` and `Config::Items()` return `StormByte::Safe::Vector<StormByte::Safe::Shared<Item::Base>>`. Path traversal uses Base's `StormByte::Safe::Queue` internally.

### Text syntax

```plaintext
username = "example_user"
timeout = 30
feature_timeout = 60.5
enabled = true
payload = b"SGVsbG8gV29ybGQ="
favorite_numbers = [3 14 42 "pi constant"]
settings = {
	username = "example_user"
	timeout = 30
}
# bash comment
// C++ comment
/* multiline */
```

Text values are UTF-8. Binary values are `StormByte::BinaryData`. Paths use `/`. List slots are numeric path segments (`list/0`).

## Contributing

Issues only on this repository. Fork and open a pull request against `master`.

## License

From 2.0.0, original StormByte-Config source is dual-licensed:

1. GNU Lesser General Public License version 3 or later. See [LICENSE](LICENSE) and <https://www.gnu.org/licenses/lgpl-3.0.html>.
2. A commercial license from the copyright holder (David C. Manuelda, StormBytePP).

Neither license covers other StormByte modules or third-party material shipped under `thirdparty/`. Those keep their own licenses. Neither license grants patent rights.

Static linking under the LGPL is described under [Installation](#installation).

## Support

StormByte is developed in spare time. Sponsorship is optional and does not buy features, priority or support.

- [GitHub Sponsors](https://github.com/sponsors/StormBytePP)
- [PayPal](https://paypal.me/StormBytePP)
