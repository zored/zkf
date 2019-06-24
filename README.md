# ZKF
Zored Keyboard Firmwares — are firmwares for my keyboards:
- Ergodox.
- Planck.

## Usage
```bash
# Clone with submodules:
git clone --recurse-submodules zored/zkf

# Build and flash firmwares:
./project.sh build-and-flash
```

## TODO
- Guess OS in `project.sh`.;
- Compile `teensy` on Windows.
- Replace builder image: Fedora -> Alpine.
- Simplify dark magic of `Makefile`.

## 💖
- [EZ](https://ergodox-ez.com/).
- [QMK](https://github.com/qmk/qmk_firmware).
- [alebastr keymap](https://git.alebastr.su/alebastr/qmk-whitefox-keymap) for C++ sample.