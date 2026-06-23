# Tatsh NFO Viewer

<!-- WISWA-GENERATED-README:START -->

[![C++](https://img.shields.io/badge/C++-00599C?logo=c%2B%2B)](https://isocpp.org)
[![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/tnfoview)](https://github.com/Tatsh/tnfoview/tags)
[![License](https://img.shields.io/github/license/Tatsh/tnfoview)](https://github.com/Tatsh/tnfoview/blob/master/LICENSE.txt)
[![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/tnfoview/v0.1.0/master)](https://github.com/Tatsh/tnfoview/compare/v0.1.0...master)
[![Dependabot](https://img.shields.io/badge/Dependabot-enabled-blue?logo=dependabot)](https://github.com/dependabot)
[![Stargazers](https://img.shields.io/github/stars/Tatsh/tnfoview?logo=github&style=flat)](https://github.com/Tatsh/tnfoview/stargazers)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
[![CMake](https://img.shields.io/badge/CMake-6E6E6E?logo=cmake)](https://cmake.org/)
[![Prettier](https://img.shields.io/badge/Prettier-black?logo=prettier)](https://prettier.io/)
[![Tests](https://github.com/Tatsh/tnfoview/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/tnfoview/actions/workflows/tests.yml)
[![Coverage Status](https://coveralls.io/repos/github/Tatsh/tnfoview/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/tnfoview?branch=master)

[![@Tatsh](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fpublic.api.bsky.app%2Fxrpc%2Fapp.bsky.actor.getProfile%2F%3Factor=did%3Aplc%3Auq42idtvuccnmtl57nsucz72&query=%24.followersCount&label=Follow+%40Tatsh&logo=bluesky&style=social)](https://bsky.app/profile/Tatsh.bsky.social)
[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-Tatsh-black?logo=buymeacoffee)](https://buymeacoffee.com/Tatsh)
[![Libera.Chat](https://img.shields.io/badge/Libera.Chat-Tatsh-black?logo=liberadotchat)](irc://irc.libera.chat/Tatsh)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109370961877277568?domain=hostux.social&style=social)](https://hostux.social/@Tatsh)
[![Patreon](https://img.shields.io/badge/Patreon-Tatsh2-F96854?logo=patreon)](https://www.patreon.com/Tatsh2)

<!-- WISWA-GENERATED-README:STOP -->

Tatsh NFO Viewer is a small desktop application for viewing NFO files: the ASCII and "ANSI art"
text files that are traditionally distributed alongside scene releases, demos, and old software.
These files are encoded in IBM code page 437, so they look like garbage in an ordinary text editor
that assumes UTF-8 or Latin-1. Tatsh NFO Viewer decodes them correctly and renders them in a
faithful fixed-width font, so the box-drawing characters, blocks, and shaded borders line up the way
they were meant to.

## Features

- Correct IBM code page 437 (CP437) to Unicode decoding.
- A bundled fixed-width CP437 font ([unscii](http://viznut.fi/unscii/)) so block and box-drawing
  characters connect cleanly, with reduced line spacing for unbroken vertical lines.
- Open a file from the toolbar or by passing it on the command line; it can be set as the handler
  for `.nfo` files.
- Automatic window sizing that fits the width of the art on load, without a horizontal scrollbar.
- Adjustable on-screen text size (increase, decrease, and reset) without changing the saved
  settings.
- Optional word wrapping for files that contain long lines of prose.
- A choice of font in the settings: the bundled CP437 font or any monospaced family installed on
  your system.
- Copy the displayed text to the clipboard.
- Remembers its window size and position between sessions.

It runs on Linux, Windows, and macOS.

## Usage

Open Tatsh NFO Viewer and choose a file with the **Open** button, or pass a file on the command
line:

```shell
tnfoview release.nfo
```

On Linux it can be set as the default application for NFO files through your desktop environment's
file associations; on Windows the installer registers it as an option for opening `.nfo` files.

## Building from source

Requirements:

- CMake 3.25 or later
- A C++23 compiler
- Qt 6 with the Widgets module

After cloning the repository:

```shell
cmake -B build
cmake --build build
```

The viewer is then at `build/src/tnfoview`.

To build and run the tests, configure with `-DBUILD_TESTS=ON` and run them with `ctest` (Linux
only); add `-DCOVERAGE=ON` for coverage information (GCC and Clang only). To build the documentation,
configure with `-DBUILD_DOCS=ON`.
