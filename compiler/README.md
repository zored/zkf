# ZKF compiler
- Compiles keymap files:
    + `keymap.c` with basic behavor (from [template](template/zored_keymap.c)).
    + `config.h` is header file with code definitions.
    + `rules.mk` is Makefile with parameters.
- Puts these files in corresponding QMK directories.

Entrypoint is [here](run.js).