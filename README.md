# ZKF
Zored Keyboard Firmwares — are firmwares for my keyboards:
- Ergodox.
- Planck.

## Usage
```bash
# Clone with submodules:
git clone --recurse-submodules zored/alebastr-qmk-whitefox-keymap

# Build and flash firmwares:
./project.sh build-and-flash
```

## TODO
- Migrate from [C](./keymap.c) to [C++](./src/main.cpp):
    + Get rid of designated initializers.
- Guess OS in `project.sh`.
- Compile `teensy` on Windows.
- Replace builder image: Fedora -> Alpine.
- Simplify dark magic of `Makefile`.
- Move to separate repository: `alebastr-qmk-whitefox-keymap` -> `zfk`.

## 💖
- [EZ](https://ergodox-ez.com/).
- [QMK](https://github.com/qmk/qmk_firmware).
- [alebastr keymap](https://git.alebastr.su/alebastr/qmk-whitefox-keymap) for C++ sample.