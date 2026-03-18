# Packaging with CPack

Currently supported methods of packaging include:
- MSI installer (WIX - Windows-only)
- Archive (7Z, TGZ, ZIP, etc)

The resulting package will be moved to the project root when complete.

## MSI installer

``cpack -G WIX --config build/CPackConfig.cmake``

The post-build script ``packaging/fix_wix.cmake`` will be executed to add some information to the installer that is needed for notifications to work.

## Archive

``cpack -G [7Z | TGZ | ZIP | ...] --config build/CPackConfig.cmake``