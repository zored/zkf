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

## Feature: OS input source awareness

The keyboard guesses the active RU/EN layout; `DO_NEXT_LANGUAGE` only toggles blindly.

- [ ] Host → keyboard over Raw HID (`RAW_ENABLE`): a small host helper sends the current input source on change; the keymap reacts (layer, mapping, LED).
- [ ] Evaluate QMK OS detection (`OS_DETECTION_ENABLE`) to pick macOS / Windows mode automatically instead of `macosMode` / `windowsMode`.

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
