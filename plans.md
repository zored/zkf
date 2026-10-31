# Plans

Order matters: finish a phase before starting the next. Keyboards always go **YMD09 → Planck → ErgoDox → DAO44 → Anne Pro 2**: the simplest board first, the daily driver last, the non-QMK boards after.

Context: [zored_qmk.md](zored_qmk.md). Per-board steps: [docs/keyboards/](docs/keyboards/).

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
- [ ] Install QMK Toolbox on the host.
- [ ] Reflash the same firmware through QMK Toolbox: YMD09 → Planck → ErgoDox (keyboard check, smoke only).
- [ ] DAO44: reflash both halves via `.uf2` copy, re-pair halves.
- [ ] Remove the `wally-cli` check from `project.sh sync` (it blocks a fresh clone); drop the wally/dfu flash branches once Toolbox works for all three.
- [ ] Write `docs/keyboards/*.md` findings back (bootloader entry, Toolbox quirks).

## Phase 1: build against upstream QMK

The fork's only patch (tap-dance re-lookup) is upstream since [qmk/qmk_firmware#17935](https://github.com/qmk/qmk_firmware/pull/17935) (2022-10).

- [ ] Evaluate QMK External Userspace (`qmk_userspace`) as the target layout: keymaps outside the QMK tree, `qmk compile` in a container. Unverified: how it pins a QMK revision. Decide: userspace vs current `vendor/` clone.
- [ ] Point `project.sh sync` at upstream `qmk/qmk_firmware`, pinned to a release tag or commit.
- [ ] Replace the `qmkfm/qmk_firmware` image with the current official QMK CLI container; check for an arm64 build.
- [ ] Fork housekeeping in [zored/qmk_firmware](https://github.com/zored/qmk_firmware) (ask before deleting anything):
  - [ ] Tag `archive/zkf_stable_planck`, `archive/zkf_stable_ap2`, `archive/zored`, `archive/zkf`.
  - [ ] Sync `master` with upstream; it becomes the fork's working and default branch.
  - [ ] Delete the archived branches.
  - [ ] Future core patches: one commit each on top of `master`, rebased on upstream.

## Phase 2: migrate keymaps to the current QMK API

Per board, in order YMD09 → Planck → ErgoDox, each closed by the keyboard check.

- [ ] Template renames: `qk_tap_dance_state_t` → `tap_dance_state_t`, `KC_LCTRL`-style aliases, `UC_OSX` / `UC_M_OS` unicode modes, `LEADER_EXTERNS` / leader API, `biton32`, `DYNAMIC_MACRO_RANGE`, `layer_state_set_user` signature.
- [ ] `TAPPING_TERM_TAP_DANCE` → `TAPPING_TERM_PER_KEY` + `get_tapping_term()`.
- [ ] Compiler: Node LTS image, `js-yaml` 4 (`load` instead of `safeLoad`), current eslint.
- [ ] YMD09.
- [ ] Planck.
- [ ] ErgoDox.
- [ ] Tag a release `v3.0.0` once all three pass.

## Phase 3: docs

- [ ] `docs/keyboards/<kb>.md` updated for every board: framework, firmware source, flash tool, bootloader entry, layout link.
- [ ] README: board table pointing at those files.
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

## Known issues

- `DO_NEXT_TAB` / `DO_PREV_TAB` lack `break` and fall through into `DO_FUTURE` (`compiler/template/zored_keymap.c`).
- `DO_FUTURE` sends "back", `DO_PAST` sends "forward": names swapped.
- `code_hold_ms` ignores its argument and always holds CapsLock.
- `ZKC_BTL` is declared but never handled; `project.sh flash` still mentions it.
- `DYNAMIC_MACRO_ENABLE: 'no'`, yet the ErgoDox emoji layer has record/play macro keys.
- `KeymapFiles` compares a Buffer to a string with `!==`, so files are always rewritten (`compiler/run.js`).
- `compileKeymap` receives an unused `config` argument.
- README mentions `./project.sh provision`; the command is `provision-host`, and its `cp` arguments are reversed.
- Stale tooling: `docker-machine` in `project.sh`, `actions/checkout@v2`, `docker/setup-buildx-action@v1.1.1`, `softprops/action-gh-release@v1`.
- Leftover `console.log` in `docs/js/main.js`.
