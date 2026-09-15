# Brighton Rig

A guitar amp/effects plugin (JUCE, VST3/AU, AAX-ready) built around a
Vox AC30-style amp, treble-booster, Deacy-style blend amp, cabinet/mic
section, modulation, delay (including a dedicated Brighton-Rock stereo
wet/dry/wet mode) and reverb — inspired by the classic Brian May / Queen
guitar tone. No guitar modeling is included; it processes an external
guitar signal. Original branding/UI; no official endorsement implied.

This is a v1 "core tone chain" build: functional signal chain, full
parameter set, factory presets, and a usable (not yet fully animated)
control UI. Planned follow-ups: spectrum analyzer, tuner, MIDI Learn,
A/B/snapshots, animated signal-flow graphics, independent multi-amp
(AC30 A/B) routing.

## Building

```sh
cmake -B build
cmake --build build --config Release -j
```

Output:
- VST3: `build/BrightonRig_artefacts/Release/VST3/Brighton Rig.vst3`
- AU (macOS only): `build/BrightonRig_artefacts/Release/AU/Brighton Rig.component`
- Standalone: `build/BrightonRig_artefacts/Release/Standalone/`

Install VST3 to `~/.vst3` (Linux), `C:\Program Files\Common Files\VST3` (Windows),
or `~/Library/Audio/Plug-Ins/VST3` (macOS); AU to
`~/Library/Audio/Plug-Ins/Components` (macOS).

## Enabling AAX (Pro Tools)

Requires Avid's free Developer account (AAX SDK) and PACE signing — see
`-DAAX_SDK_PATH=...`. No source changes needed once the SDK is available.
Alternatively, load the VST3 in Pro Tools via a VST-to-AAX wrapper such as
MetaPlugin (universal arm64+x86_64 binaries are built for macOS to support
this regardless of whether the host is running under Rosetta).
