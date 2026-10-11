# Qt notice

Sparkle Launcher uses dynamically linked Qt libraries and plugins. Their LGPLv3 rights apply separately from Sparkle's MIT license. The included LGPLv3 and GPLv3 texts describe those rights. `ThirdPartyNotices.txt` retains Qt Base/SVG third-party attributions and license texts from the matching 6.11.1 sources and publisher SBOMs, plus Qt's Mesa attribution. Microsoft binaries still need their own terms; this directory alone is not the complete package notice bundle.

## Corresponding source

`QtRuntimeVersion.txt` beside this notice records the linked Qt version. The following source archives are qualified for **6.11.1 only**. If that file names another version, these archives do not establish corresponding source for that build; identify matching source before distribution.

- Qt Base 6.11.1: https://download.qt.io/archive/qt/6.11/6.11.1/submodules/qtbase-everywhere-src-6.11.1.zip
  SHA-256: `3529cc37297a5a7aae4486843b9fd41c30df1d79a770f85e240b537dcc327ca5`
- Qt SVG 6.11.1: https://download.qt.io/archive/qt/6.11/6.11.1/submodules/qtsvg-everywhere-src-6.11.1.zip
  SHA-256: `767730188d4610a89bf8da502f87acf1c8881a3ac54f1e0eb167ab1e08b03a75`

Publish these exact source companions alongside any product using the qualified vendor DLLs. Publisher links identify the inputs; they do not replace Sparkle's source-delivery obligation. If the deployed Qt version or modules change, requalify and update this notice before release.

## Library replacement

You may modify Qt and replace its libraries for your own use, including reverse engineering needed to debug those modifications. Sparkle imposes no restriction on those LGPL rights.

Close Launcher, preserve a copy of the extraction, and replace its Qt DLLs and plugin DLLs with a compatible x64 release build using the same Qt ABI and compiler runtime. Keep the plugin subdirectories. NVIDIA components retain their separate terms. The package must allow this route without an enforced vendor-hash or signature lock. Actual replacement and launch must be tested before package acceptance.
