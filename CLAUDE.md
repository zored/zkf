# ZKF

YAML-configured keymaps for my QMK keyboards. `config.yaml` → `compiler/run.js` → QMK keymap → firmware.

- `config.yaml` keys and template features: [compiler/README.md](compiler/README.md)
- Roadmap, known issues, keyboard check order: [plans.md](plans.md)
- How to flash each board, its bootloader, its layout: [docs/keyboards/](docs/keyboards/)

## Build

Build locally in Docker; the Mac host stays free of toolchains (no `qmk`, `avr-gcc`, `node`).

```bash
./project.sh build ergodox_ez   # planck | ymd09; output: firmwares/<kb>.{hex,bin}
./project.sh build-all
```

- First run clones the QMK fork into `vendor/qmk_firmware` and pulls `qmkfm/qmk_firmware` (amd64, emulated on arm64): slow once, then each board builds in under a minute.
- `sync` exits 1 when `wally-cli` is missing (flasher check). Install it, or rerun the build: `sync` is skipped once `vendor/` exists.
- Every build rewrites `example/<kb>/`; commit those diffs with the `config.yaml` change that caused them.
- `vendor/` is a plain copy without `.git`; delete it to re-sync the fork.

## Release

GitHub Actions ([release.yaml](.github/workflows/release.yaml)) builds every push and publishes firmware only for `v*` tags. A tag marks a final version; iterate with local builds. Latest: `v2.2.1`, fetched with `./project.sh download <kb>`.

## Flash

Flashing runs on the host (Docker on macOS has no USB passthrough). Target tool: QMK Toolbox for ErgoDox, Planck, YMD09. DAO44 takes a `.uf2` copy; Anne Pro 2 needs `annepro2_tools`. Per-board steps live in `docs/keyboards/<kb>.md`.

## Conventions

- Docs and code are English.
- Template code targets the 2020 QMK API (`qk_tap_dance_state_t`, `KC_LCTRL`, `UC_OSX`); match it until the migration in plans.md lands.
- `docs/` (GitHub Pages) reads `config.yaml` from `master` at runtime: a pushed `config.yaml` change updates the layout viewer.
- Passwords and other secrets stay out of git and CI artifacts: release firmware is public.
