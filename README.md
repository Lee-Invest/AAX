# SimpleVerb

A basic reverb audio plugin built with [JUCE](https://juce.com), targeting VST3
(and AU on macOS) now, with AAX wired in as an optional target for later.

## Parameters
- Room Size, Damping, Wet Level, Dry Level, Width, Freeze Mode

## Building (VST3 / Standalone)

Requirements: CMake 3.22+, a C++17 compiler, and (on Linux) the usual JUCE
dependency packages (ALSA/X11/etc. dev headers).

```sh
cmake -B build
cmake --build build --config Release -j
```

The build downloads JUCE automatically via CMake FetchContent (requires
internet access on first configure). Build output:
- VST3: `build/SimpleVerb_artefacts/Release/VST3/SimpleVerb.vst3`
- Standalone app: `build/SimpleVerb_artefacts/Release/Standalone/`
- AU (macOS only): `build/SimpleVerb_artefacts/Release/AU/SimpleVerb.component`

**Important:** a plugin built on Linux produces a Linux VST3, one built on
Windows produces a Windows VST3, and one built on macOS produces a macOS
VST3/AU. To use it in your DAW, build on (or for) the same OS you run your
DAW on.

To install: copy the `.vst3` file into your system's VST3 folder
(`~/.vst3` on Linux, `C:\Program Files\Common Files\VST3` on Windows,
`~/Library/Audio/Plug-Ins/VST3` on macOS), then rescan plugins in your DAW.

## Enabling AAX (Pro Tools)

AAX requires two things this project can't include:
1. **Avid AAX SDK** — free with an Avid Developer account at
   https://developer.avid.com. Download it, then configure this project with:
   ```sh
   cmake -B build -DAAX_SDK_PATH=/path/to/AAX_SDK
   ```
   AAX will then be built alongside VST3 automatically.
2. **PACE signing** — Pro Tools refuses to load unsigned AAX plugins. After
   building, sign the AAX bundle with `wraptool` using your PACE Anti-Piracy
   developer credentials before copying it into
   `/Library/Application Support/Avid/Audio/Plug-Ins` (macOS) or
   `C:\Program Files\Common Files\Avid\Audio\Plug-Ins` (Windows).

No source code changes are needed to go from VST3-only to VST3+AAX — just
supply the SDK path and sign the result.
