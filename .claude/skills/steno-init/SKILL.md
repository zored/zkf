---
name: steno-init
description: Set up Plover steno on this Mac for a ZKF board (Planck, ErgoDox): firmware with the steno layer, Plover, Russian Trillo, plover-setup, verification. Use when asked to set up, repair or check stenography / Plover.
---

# steno-init

Takes the Mac from "board plugged in" to "steno types English on ABC and Russian on RussianWin". [docs/plover.md](../../../docs/plover.md) is the source of truth for what the setup writes and why; board keys live in `docs/keyboards/<kb>.md` under "Steno (Plover)". Read both before step 1.

Each step ends on its check. Run the check, report it as verified, and only then move on.

## Steps

1. **Plover.** `defaults read /Applications/Plover.app/Contents/Info.plist CFBundleShortVersionString` prints 5.1 or newer. Missing or older: ask the user to install it from openstenoproject.org (downloads are theirs to do), then re-check.
2. **Firmware.** `hidutil list --matching '{"PrimaryUsagePage":0xFF50}'` lists the board. Empty: build and flash it per `CLAUDE.md` (Planck: start `./project.sh flash planck` in the background, then ask for the bootloader), then re-check.
3. **Trillo (Russian).** `ls ~/Library/Application\ Support/plover/plugins/mac/lib/python/site-packages | grep russian_trillo` finds it, or the user declines Russian. To add it, ask the user: Plover → Plugins Manager → `plover-russian-trillo` → install, restart Plover.
4. **Config.** Ask the user to quit Plover with Cmd+Q (`pgrep -fl Plover.app` prints nothing), then run `./project.sh plover-setup`. Done when it prints `Updated …/plover.cfg`. Keep the Plover app path out of any shell command you run alongside it: the script's `pgrep -f` matches its own caller's command line.
5. **Start.** `open -a Plover`, then read the new tail of `~/Library/Application Support/plover/plover.log`. Done when it shows `starting \`zkf_layout_fix\` extension`, a `zkf_layout_fix: h=…, system=…` line whose system matches the current input source, and no `ERROR` since the start.
6. **Hands-on.** The user enters the `steno` layer (board doc), opens Tools → Paper Tape, and confirms on both input sources: the tape header is `#STKPWHRAO*EUFRPBLGTSDZ` on ABC and the Cyrillic Trillo chart on RussianWin, words appear in a text field, the front app never hides.

Finish with a per-step status line: verified, skipped by the user, or blocked with the reason.

## Symptoms

| Symptom | Cause | Fix |
|---|---|---|
| Strokes reach the paper tape, no text appears | output off on start | step 4 sets `auto_start`; meanwhile Output → Enable |
| Front app hides (`Cmd+H`) or switches while stenoing | `zkf_layout_fix` inactive, log lacks `zkf_layout_fix: h=` | step 4, restart Plover; `h=((4, 1),)` in the log means the patch missed the live class |
| `invalid implicit_hyphen_keys: some letters are not unique` | Trillo loaded without `zkf_layout_fix` | step 4 |
| `LIBUSB_ERROR_ACCESS` while flashing Planck | QMK Toolbox holds the DFU device | user quits QMK Toolbox |
| No HID device after flashing | `PLOVER_HID_ENABLE` off for the board | check its `rules` in `config.yaml` |
