# Changelog
All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog
and this project adheres to Semantic Versioning.

## [Unreleased]

## [0.1.0] - 2025-12-19
### Added
- Basic text editor with Gtkmm 4
- Open file support
- Save, Save As, Open, New,Quit
- File menu with header bar
- Scrollable text view
- Document modification tracking
- Cancel, Save, don't save when closing app
- Editor tittle and * when unsave

### Changed
- Project structure refactored into multiple classes

### Fixed
- Application closing unexpectedly

## [0.1.1] - 2025-12-26
### Added
- File type validation (prevents opening images, videos, PDFs, etc.)
- Confirmation dialog when opening/creating new file with unsaved changes
- Visual error dialogs for file operations (read/write failures)
- File validator component (`FileValidator.hpp`)

### Changed
- Enhanced error reporting with user-friendly dialogs instead of console-only messages

### Fixed
- App crash when opening folders or non-text files
- Confirmation dialog now properly saves before proceeding with Open/New actions
- File dialog no longer stays open after saving and opening another file