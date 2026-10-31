# Plans

Order matters: finish a phase before starting the next. Keyboards always go **YMD09 → Planck → ErgoDox → DAO44 → Anne Pro 2**: the simplest board first, the daily driver last, the non-QMK boards after.

Per-board steps: [docs/keyboards/](docs/keyboards/). Config and template reference: [compiler/README.md](compiler/README.md).

## Keyboard check (reused by every phase)

For one board at a time:

- [ ] **Rollback ready**: the previous firmware file sits in `firmwares/` (`./project.sh download <kb>` for `v2.2.1`).
- [ ] **Built**: new firmware file in `firmwares/`.
- [ ] **Flashed** with the board's tool from `docs/keyboards/<kb>.md`.
- [ ] **Smoke**: letters, every layer, dances (tap and hold), RU/EN switch, macOS/Windows mode, unicode/emoji. Where the board has them: combos, leader, LEDs, RGB, audio.
- [ ] **Fast typing regression**: hold a layer dance and press the next key before the tapping term ends; the next key comes from the new layer. Same for home-row mods typed fast.
- [ ] **Daily use**: work on it until it feels the same as before.

Done = every box ticked. Broken = flash the rollback file, write the problem under the phase.

## Phase 0: baseline on current firmware

Prove the flashing workflow on firmware already known to work, before changing anything.

- [ ] Collect current firmware in `firmwares/`:
  - [ ] `./project.sh download ymd09`, `planck`, `ergodox_ez` (`v2.2.1` = current master code).
  - [ ] DAO44: build `dao_left` / `dao_right` `.uf2` from [zored/dao-zmk-config@zored](https://github.com/zored/dao-zmk-config/tree/zored) in Docker (`./run.sh image flow`), copy into `firmwares/`.
- [x] Install QMK Toolbox on the host: `brew install --cask qmk-toolbox`.
- [ ] Reflash the same firmware through QMK Toolbox (keyboard check, smoke only):
  - [ ] YMD09.
  - [x] Planck.
  - [ ] ErgoDox.
- [ ] DAO44: reflash both halves via `.uf2` copy, re-pair halves.
- [ ] Remove the `wally-cli` check from `project.sh sync` (it blocks a fresh clone); drop the wally/dfu flash branches once Toolbox works for all three.
- [ ] Write `docs/keyboards/*.md` findings back (bootloader entry, Toolbox quirks).

## Phase 1: build against upstream QMK

The fork branch `zkf_stable_planck` is upstream `ae46e6a` (2020-02) plus one patch: after an interrupted tap dance switches layers, `process_record_quantum()` re-reads the keycode, so a key typed fast after a layer dance comes from the new layer. Upstream has the same fix since [qmk/qmk_firmware#17935](https://github.com/qmk/qmk_firmware/pull/17935) (2022-10), with tests in `tests/tap_dance/tap_dance_layers/`. Other fork branches hold nothing still needed: `ACTION_TAP_DANCE_DOUBLE_TIME` maps to `TAPPING_TERM_PER_KEY`, Anne Pro 2 is upstream.

- [x] Evaluate QMK External Userspace: rejected, local core patches are likely. `vendor/qmk_firmware` stays a clone, now with `.git`.
- [x] Point `project.sh sync` at upstream `qmk/qmk_firmware`, pinned by `qmk_ref` (`0.34.7`).
- [x] Replace the `qmkfm/qmk_firmware` image with `ghcr.io/qmk/qmk_cli` (native arm64).
- [ ] CI: `release.yaml` still uses old actions; check `./project.sh ci` on the new images.
- [ ] Fork housekeeping in [zored/qmk_firmware](https://github.com/zored/qmk_firmware) (ask before deleting anything):
  - [x] Tag `archive/zkf_stable_planck`, `archive/zkf_stable_ap2`, `archive/zored`, `archive/zkf`.
  - [x] Sync `master` with upstream; it becomes the fork's working and default branch.
  - [ ] Delete the archived branches.
  - [ ] Future core patches: one commit each on top of `master`, rebased on upstream.

## Phase 2: migrate keymaps to the current QMK API

Per board, in order YMD09 → Planck → ErgoDox, each closed by the keyboard check.

- [x] Template renames: `tap_dance_state_t`, `KC_LCTL`-style names, `UNICODE_MODE_*`, `leader_end_user()`, `get_highest_layer`, `layer_state_t`, `UM()`, `RM_*` / `MS_*` keycodes, `NKRO_DEFAULT_ON`.
- [x] `TAPPING_TERM_TAP_DANCE` → `TAPPING_TERM_PER_KEY` + `get_tapping_term()`.
- [x] Compiler: `node:22-alpine`, `js-yaml` 4 (`load`).
- [ ] Compiler: current eslint.
- [ ] YMD09: compiles on `0.34.7`; pending: board not at hand.
- [ ] Planck: smoke, fast typing, dance enemies, RGB, audio, mouse pass; daily use pending.
- [ ] ErgoDox: compiles on `0.34.7` (`ergodox_ez/base`; confirm the board variant); pending: board not at hand.
- [ ] Tag a release `v3.0.0` once all three pass.

## Phase 3: docs

- [ ] `docs/keyboards/<kb>.md` updated for every board: framework, firmware source, flash tool, bootloader entry, layout link.
- [ ] README: board table pointing at those files.
- [ ] README: host setup (`brew install --cask qmk-toolbox`) and a build → Toolbox flash example instead of `build-and-flash` (`wally-cli`).
- [ ] `docs/workflow.htm`: move "later" items into "now" as they land.
- [ ] Idea: show DAO44 in the `docs/` layout viewer.

## Phase 4: DAO44 on current ZMK

- [ ] Move from `ftc/zmk@mouse-ftc` to upstream ZMK if mouse keys are upstream by then (unverified).
- [ ] Keyboard check on both halves.

## Phase 5: Anne Pro 2

Board is not at hand yet.

- [ ] Find the board, read its revision (C15 or C18) from the PCB or stock firmware.
- [ ] Build upstream `annepro2/c15` or `annepro2/c18` with ZKF.
- [ ] Get a flasher on the host (the one host-install exception): `nix run github:OpenAnnePro/AnnePro2-Tools`, or [prifak/annepro2_flasher_c](https://github.com/prifak/annepro2_flasher_c). QMK Toolbox does not support the HT32 bootloader.
- [ ] Update Shine (LED MCU firmware) from [OpenAnnePro/AnnePro2-Shine](https://github.com/OpenAnnePro/AnnePro2-Shine): `annepro2_tools --boot -t led <shine>.bin`.
- [ ] Flash QMK: hold Esc while plugging in (IAP), `annepro2_tools <fw>.bin`.
- [ ] Keyboard check, plus Bluetooth and LEDs.

## Feature: password key

Threat model accepted: the keyboard stays at home; anyone holding it can dump flash/EEPROM, and the key types into whatever has focus. Use it for low-value passwords only.

- [ ] **Compile-time**: `secrets.h` (gitignored), included via `__has_include`; CI builds a stub, so release artifacts stay clean.
- [ ] **Runtime change**: a record mode that stores a new password in EEPROM, overriding the compiled one; survives reflash until EEPROM is reset.
- [ ] Guard against accidental typing: leader sequence or combo, never a single key.

## Feature: emoji without a special macOS input source

Unicode keys on macOS (`UC_OSX`) type only while the "Unicode Hex Input" input source is active, a third source next to RU/EN.

- [ ] Compare options: macOS-only build sending emoji another way (e.g. a host-side helper or text replacement), a separate OS-specific firmware binary, or dropping emoji from firmware.
- [ ] Decide whether a second binary per board (macOS / Windows) is worth it over the runtime `macosMode` / `windowsMode` switch.

## Feature: host awareness (OS, input source, volume)

The keyboard knows which OS it is plugged into, the active input source and the output volume, and reacts: OS-specific shortcuts, absolute RU/EN switching, RGB overlays ([RGB status overlays](#feature-rgb-status-overlays)). Hosts: macOS and Windows; the boards switch between them through a KVM. Phases go in order A → B → C.

Every part is a per-board flag in `config.yaml`, and the compiler emits no code for a disabled part: `osDetection`, `hostState` (also gates `RAW_ENABLE`), `rgbOverlay`, `layoutAwareAscii`. Each item records its flash/RAM growth from `qmk compile`. Budget: at most ~1.5 KB for all parts on AVR boards (ErgoDox `0.34.7` build: 28.7 KB of ~32 KB); Planck (61 KB of 256 KB) is not limited.

Facts (QMK `0.34.7` sources): `RAW_ENABLE` fits the USB endpoints on Planck (5 of 7), YMD09 (5 of 6, LUFA) and ErgoDox (6 of 6: no room for `CONSOLE` after it). Raw HID: usage page `0xFF60`, usage `0x61`, 32-byte reports, `raw_hid_receive` / `raw_hid_send`; the keyboard can push. A vendor-page interface opens on macOS without the Input Monitoring permission. No QMK feature knows host volume or input source.

### Phase A: OS detection (no host helper)

- [x] Planck: `OS_DETECTION_ENABLE` with `OS_DETECTION_KEYBOARD_RESET` (the KVM may keep USB powered, so the board resets to re-detect) and `OS_DETECTION_SINGLE_REPORT` (ARM Macs fire the callback again minutes later). Size, `rgbOverlay` included (`DEFERRED_EXEC`, connect overlay, flags toggle): +1208 B flash, +4 B RAM; 62936 B of 256 KB.
- [x] Verify on hardware through the KVM: switching to the Mac and to the Windows machine each detects the right OS. If the KVM emulates a keyboard instead of passing USB through, detection and Raw HID cannot work: record that and stop the phase.
- [x] `process_detected_host_os_user` sets the mode: macOS and iOS → macOS; Windows and Linux → Windows; unsure → keep the mode saved in EEPROM. The mode drives everything `zored_os` drives today (`DO_*` shortcuts, `DO_NEXT_LANGUAGE`, emoji panel) plus the unicode input mode.
- [x] `macosMode` / `windowsMode` stay as a manual override until the next connect.
- [x] Connect overlay ([RGB status overlays](#feature-rgb-status-overlays)): whole board white for macOS, blue for Windows, 1 s.
- [x] ErgoDox and YMD09: same logic behind `osDetection`, size recorded; hardware check when the boards are at hand.
  - ErgoDox: +758 B flash, +22 B RAM; 29598 of 32256 B (HalfKay), 2658 B free.
  - YMD09: +796 B flash, +22 B RAM; 28018 of 28672 B (`atmel-dfu` takes 4 KB), 654 B free.
  - OS detection alone takes half of the ~1.5 KB AVR budget; YMD09 has no room for phase B as is.
- [x] Docs: `docs/keyboards/planck.md` (OS detection, override keys), `compiler/README.md` (flags).

### Phase B: host helper

One helper in Rust (`os/host`) for macOS and Windows; [qmk-hid-host](https://github.com/zzeneg/qmk-hid-host) is a code reference only (it has no mute, sends layouts as indexes, opens the device exclusively on macOS, ships unsigned rolling builds).

- [ ] Spike: build a Rust binary that links IOKit, CoreAudio and Carbon in Docker with `cargo-zigbuild`, mounting the SDK of the installed Command Line Tools (`/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk`); run it on the Mac. `x86_64-pc-windows-gnu` in the same image. If the macOS build fails: Swift file built by the CLT `swiftc` plus a PowerShell 5.1 `.ps1` with `Add-Type` C#.
- [ ] Protocol, 32-byte reports `[type, version, payload…]`:
  - host → keyboard: `0x01` layout index (`0xFE` = a source not in the list), `0x02` volume 0–100 plus mute flag (`0xFF` = device without volume, e.g. HDMI).
  - keyboard → host: `0x80` hello (asks for the full state on connect), `0x81` select layout index.
  - The helper repeats the state every 5 s; after 15 s without a report the keyboard treats the state as unknown: no overlays, no layout-aware output. No guessing from the keyboard's own key presses.
- [ ] Layout list from arguments, index = position: macOS `--layouts com.apple.keylayout.ABC,com.apple.keylayout.RussianWin`, Windows `0409,0419`.
- [ ] macOS: input source from `kTISNotifySelectedKeyboardInputSourceChanged`, selection with `TISSelectInputSource`; volume and mute from CoreAudio listeners on the default output device, plus default-device changes. Reopen the device after sleep/wake and replug.
- [ ] Windows: layout by polling `GetKeyboardLayout` of the foreground window's thread (console windows may report wrong), volume and mute from `IAudioEndpointVolume` callbacks, selection on `0x81`.
- [ ] Absolute switching: Caps tap = EN, Caps double-tap = RU (replaces the blind `DO_NEXT_LANGUAGE` toggle). macOS goes through the helper (`0x81`); Windows uses the native "Input language hot keys" (Ctrl+Shift+digit per language) if they work reliably on the machine, otherwise the helper. Unknown state or no helper: fall back to today's toggle.
- [ ] Install: `./project.sh host-install` sets up a LaunchAgent (macOS) or a Startup-folder shortcut (Windows). Release binaries come from CI next to the firmware.
- [ ] Planck: `hostState` and overlays enabled; layout and volume overlays checked live on both hosts.
- [ ] Docs: helper setup, protocol, `config.yaml` keys.

### Phase C: layout-aware ASCII (experiments behind `layoutAwareAscii`)

With RU active, firmware ASCII output (symbol layer, `SEND_STRING` macros like `doArrow`, combos) types the wrong characters: `.` → `ю`, `,` → `б`, `->` → `-Ю`.

- [ ] C1: for characters that exist in RussianWin, send the RussianWin key instead (`.` → `/`, `,` → `Shift+/`, `/` → `Shift+\`, `;` → `Shift+4`, `:` → `Shift+6`, `?` → `Shift+7`, `"` → `Shift+2`). The compiler generates the table. Unknown state: today's behaviour.
- [ ] C2: characters missing from RussianWin (`[ ] { } < > ' @ # $ ^ & ~` and backtick): select EN through the helper, wait for the layout report, type, select RU back. Separate flag; measure the delay.

### Ideas

- System-wide mic mute from the helper with a key and an overlay. Risks: the built-in Mac mic often has no settable mute, Zoom and Electron AGC raise the input volume back, the call app keeps showing "unmuted". No API found for the VK WorkSpace in-call mute state.

## Feature: RGB status overlays

Transient overlays on top of the running RGB matrix animation; nothing is persistent. Colours and durations live in `config.yaml`.

- [x] Draw overlays in `rgb_matrix_indicators_advanced_user`, expire them with `defer_exec` (`DEFERRED_EXEC_ENABLE`).
- [x] `rgbToggle`: switch the animation off with `rgb_matrix_set_flags(LED_FLAG_NONE)` (as ZSA's stock code does) instead of `RM_TOGG`; `RM_TOGG` is intercepted (it acts on release), and a matrix disabled by older firmware is converted to `LED_FLAG_NONE` on boot. QMK runs no indicator callbacks while the matrix is disabled, suspended, timed out or in mode `NONE`, so with `RM_TOGG` off no overlay would show. Verified on Planck: the connect overlay shows with `LED_FLAG_NONE` and the board goes dark after it.
- [ ] Overlays (Planck, 47 LEDs, indexes 0–35 rows 0–2, 36–46 bottom row, 2u space = 41):
  - [x] OS on connect: whole board, macOS white, Windows blue, 1 s (phase A).
  - [ ] Input source change: whole board, EN light blue, RU orange, other purple, 0.6 s (phase B).
  - [ ] Volume change: top row (12 keys) as a green bar, 1.5 s; muted: whole row red; no volume: grey (phase B).
- [ ] The 2 Planck layer LEDs keep showing layers.
- [ ] YMD09 (RGB matrix, 9 LEDs): same overlays on the whole board; volume = number of lit keys.
- [ ] ErgoDox base (3 single-colour LEDs, used for layers): no overlays.

## Feature: Plover steno

- [x] NKRO is active: `NKRO_DEFAULT_ON: 'true'` sets it in `eeconfig_init()`, which runs on the first 0.34.7 boot because the EEPROM magic changed (`0xFEEC` → `0xFEE3`). Checked in QMK sources, not on hardware.
- [x] Protocol: Plover HID (`PLOVER_HID_ENABLE`, +440 bytes on ErgoDox). GeminiPR costs +1.3 KB and a serial port; the "Keyboard" machine needs Plover toggled in sync with the layer.
- [x] `steno` layer on ErgoDox (top inner key of the left half on `navigation`) and Planck (bottom-left key on `navigation`); YMD09 keeps `PLOVER_HID_ENABLE: 'no'`.
- [x] Planck flashed: macOS sees the Plover HID interface (usage page `0xFF50`, usage `0x4C56`).
- [x] Planck: strokes verified in Plover 5.4.2 with the ABC input source; non-steno keys are `KC_NO` (transparent keys fell through to `capsDance`, which taps `Cmd+Space`).
- [ ] ErgoDox: flash and verify, including `X1`..`X3`.
- [x] Any macOS input source: `zkf_layout_fix` Plover extension ([os/plover](os/plover)) drops Cmd/Ctrl combinations from Plover's layout table (with RussianWin `h` was `Cmd+H`). Checked against RussianWin and ABC tables with Plover's Python.
- [x] Russian: `plover-russian-trillo` 0.0.2 (Tréal chart); `X1`..`X3` added to both `steno` layers, Trillo → Plover HID keymap and switch strokes (`R*UFS` / `ХЦ`) written by `./project.sh plover-setup`. Docs: [docs/plover.md](docs/plover.md).
- [x] Planck: `steno` entered with `;` + bottom-left key and left with the bottom-left key (was `;` + bottom-right, both on the right hand).
- [x] Planck: flashed; steno on RussianWin and Trillo strokes work (Plover 5.4.2). The packaged app keeps `KeyboardLayout` in `plover.oslayer.keyboardlayout`, so the extension patches the class of the live emulation object, not the `osx` module.
- [x] Trillo on Plover 5: the extension drops `Н-` from Trillo's implicit hyphen keys (Plover 5 requires unique letters there) and loads its 67 `*Ё` prefix/suffix keys as `Ё*`.
- [x] System follows the macOS input source (ASCII-capable → English, else Trillo), via the same extension. Checked live on ABC and RussianWin.
- [ ] Upstream: report the Cmd-layer lookup to Plover (`plover/oslayer/osx/keyboardlayout.py`).

## Known issues

- `DO_FUTURE` sends "back", `DO_PAST` sends "forward": names swapped. Layouts disagree on order (ErgoDox `doPast, doFuture`; Planck history layer `doFuture, doPast`), so fixing needs a decision.
- Stale actions in `release.yaml`: `actions/checkout@v2`, `docker/setup-buildx-action@v1.1.1`, `softprops/action-gh-release@v1`.
