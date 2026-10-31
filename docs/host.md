# Host helper

`zkf-host` ([os/host](../os/host)) tells the keyboard the active input source and the output volume over Raw HID, and switches the input source when the keyboard asks. The keyboard uses it for absolute RU/EN switching and for RGB overlays. Boards with `hostState: true` in [config.yaml](../config.yaml) talk to it (today: Planck).

Without the helper the keyboard works as before: Caps toggles the language blindly and no layout or volume overlay shows.

## Install

macOS:

```bash
./project.sh host-build     # or put the release zkf-host-macos into firmwares/
./project.sh host-install   # LaunchAgent com.zored.zkf-host, log in ~/Library/Logs/zkf-host.log
```

Windows (Git Bash, binary from `host-build` or the release `zkf-host.exe`):

```bash
./project.sh host-install   # %LOCALAPPDATA%\zkf\zkf-host.exe + Startup shortcut, starts it now
```

`layouts=<EN>,<RU> ./project.sh host-install` overrides the input sources. Defaults: macOS `com.apple.keylayout.ABC,com.apple.keylayout.RussianWin` (`defaults read com.apple.HIToolbox AppleEnabledInputSources` lists the enabled ones), Windows `0409,0419` (language ids). The order gives the index: first = EN, second = RU. The helper exits with an error if a macOS source is not enabled.

Run by hand to watch it: `zkf-host --layouts com.apple.keylayout.ABC,com.apple.keylayout.RussianWin --verbose` prints every state change.

No permission prompt on macOS: the Raw HID interface is a vendor usage page, which needs no Input Monitoring, and the device is opened shared.

## Build

`./project.sh host-build` runs `cargo zigbuild` in `ghcr.io/rust-cross/cargo-zigbuild` and writes `firmwares/zkf-host-macos` (universal arm64 + x86_64) and `firmwares/zkf-host.exe` (x86_64, GNU). On a Mac it copies the Command Line Tools SDK into the `zkf-macos-sdk` Docker volume (Docker cannot mount `/Library`); CI uses the SDK bundled in the image. Release tags publish both binaries next to the firmware.

## Behaviour

- Polls every 100 ms: the input source (macOS: `TISCopyCurrentKeyboardInputSource`, woken at once by `kTISNotifySelectedKeyboardInputSourceChanged`; Windows: `GetKeyboardLayout` of the foreground window's thread) and the default output device's volume and mute (macOS: CoreAudio virtual main volume; Windows: `IAudioEndpointVolume`).
- Sends the state on every change and repeats it every 5 s.
- Finds the keyboard by usage page `0xFF60`, usage `0x61` (first match), retries every second, reopens it after replug, sleep or a KVM switch.
- On a select request it switches the input source: `TISSelectInputSource` on macOS, `WM_INPUTLANGCHANGEREQUEST` to the foreground window on Windows.

## Protocol

32-byte Raw HID reports `[type, version, payload…]`, version `1`; a report with another version is ignored.

| Type | Direction | Payload |
|---|---|---|
| `0x01` | host → keyboard | layout index into `--layouts`; `0xFE` = a source not in the list |
| `0x02` | host → keyboard | volume 0–100 (`0xFF` = output without volume, e.g. HDMI), mute flag 0/1 |
| `0x80` | keyboard → host | hello: sent 1 s after boot, the helper answers with the full state |
| `0x81` | keyboard → host | select layout index |

The keyboard treats a value as unknown 15 s after its last report: no overlay, and the language keys fall back to the blind toggle. The first report after a gap shows no overlay, only changes do.

## Keyboard side

- `capsDance`: tap = `doLayoutEn` (index 0), double tap = `doLayoutRu` (index 1). With a fresh layout the key sends `0x81` (nothing if already active); unknown layout → `doNextLanguage` (Cmd+Space / Alt+Shift).
- Overlays (`rgbOverlay.layout`, `rgbOverlay.volume`): see [compiler/README.md](../compiler/README.md) and [planck.md](keyboards/planck.md).
