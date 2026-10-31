# ZKF

YAML-configured keymaps for my QMK keyboards. `config.yaml` → `compiler/run.js` → QMK keymap → firmware.

- `config.yaml` keys and template features: [compiler/README.md](compiler/README.md)
- Roadmap, known issues, keyboard check order: [plans.md](plans.md)
- How to flash each board, its bootloader, its layout: [docs/keyboards/](docs/keyboards/)
- Host helper `os/host` (Rust, Raw HID: input source, volume, absolute RU/EN): [docs/host.md](docs/host.md); `./project.sh host-build`, `host-install`
- Plover steno (setup, English/Russian systems, `os/plover` plugin): [docs/plover.md](docs/plover.md); guided setup: `/steno-init`

## Build

Build locally in Docker; the Mac host stays free of toolchains (no `qmk`, `avr-gcc`, `node`).

```bash
./project.sh build ergodox_ez   # planck | ymd09; output: firmwares/<kb>.{hex,bin}
./project.sh build-all
```

- QMK is upstream `qmk/qmk_firmware`, pinned by `qmk_ref` in `project.sh` (now `0.34.7`). First run clones it with submodules into `vendor/qmk_firmware` (keeps `.git` for local core patches); to bump, change `qmk_ref`, delete `vendor/`, rebuild.
- Images: `ghcr.io/qmk/qmk_cli` (native arm64) builds firmware, `node:22-alpine` runs the compiler.
- Targets: `zsa/planck_ez/glow`, `ergodox_ez/base`, `ymdk/ymd09`. ErgoDox and YMD09 compile on `0.34.7` but are not verified on hardware yet.
- A build that changes `firmwares/<kb>.*` first copies the old file to `firmwares/previous/<timestamp>-<kb>.*` for rollback.
- Every build rewrites `example/<kb>/`; commit those diffs with the `config.yaml` change that caused them.

## Release

GitHub Actions ([release.yaml](.github/workflows/release.yaml)) builds every push and publishes firmware only for `v*` tags. A tag marks a final version; iterate with local builds. Latest: `v2.2.1`, fetched with `./project.sh download <kb>`.

## Flash

Flashing runs on the host (Docker on macOS has no USB passthrough). Target tool: QMK Toolbox for ErgoDox, Planck, YMD09. DAO44 takes a `.uf2` copy; Anne Pro 2 needs `annepro2_tools`. Per-board steps live in `docs/keyboards/<kb>.md`.

Planck: flash it yourself, don't hand the user Toolbox steps. Run `./project.sh flash planck` in the background (it waits for the STM32 DFU device, then runs the `dfu-util` bundled in QMK Toolbox), then ask the user to enter the bootloader. Report the `dfu-util` result. QMK Toolbox must be closed: it grabs the DFU device first (`LIBUSB_ERROR_ACCESS`), and the script refuses to run while it is open.

## Conventions

- Docs and code are English.
- Template code and `config.yaml` key aliases target the QMK `0.34` API (`tap_dance_state_t`, `KC_LCTL`, `UNICODE_MODE_MACOS`, `RM_*`, `MS_*`).
- `docs/` (GitHub Pages) reads `config.yaml` from `master` at runtime: a pushed `config.yaml` change updates the layout viewer.
- Passwords and other secrets stay out of git and CI artifacts: release firmware is public.
