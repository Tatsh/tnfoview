<!-- markdownlint-configure-file {"MD024": { "siblings_only": true } } -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project
adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

### Added

- Settings dialog to choose the font (monospaced families plus Iosevka), the font size, whether to
  wrap content (word wrap), and whether to auto-resize the window to fit the content.
- Toolbar with open, copy, increase text size, decrease text size, reset text size, wrap, settings,
  and about actions. The toolbar uses the desktop theme icons and is locked in place.
- <kbd>Ctrl</kbd>+<kbd>0</kbd> resets the text size. Zooming is transient and does not change the
  saved font size.
- Optional command-line argument naming a file to open; when none is given, the application prompts
  for a file on start.
- Bundled CP437 font (unscii) so box-drawing and block art render without relying on an installed
  font.

### Changed

- Ported the application to Qt 6.
- Reimplemented CP437 decoding so box-drawing and block art now render correctly, and fixed the
  line height so adjacent lines connect.
- The open dialog now filters to `*.nfo` files.
- The window remembers its geometry and auto-fits its width to the content when a file loads.

## [0.0.1] - 2026-00-00

First version.

[unreleased]: https://github.com/Tatsh/tnfoview/compare/v0.1.0...HEAD
[0.0.1]: https://github.com/Tatsh/tnfoview/releases/tag/v0.1.0
