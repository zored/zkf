# based on:
# https://github.com/jackhumbert/qmk_firmware_embeded

KEYBOARD := whitefox
KEYMAP :=
TARGET ?= whitefox-alebastr-$(QMK_VERSION)+$(KEYMAP_VERSION)

TOP_DIR := qmk_firmware

.DEFAULT_GOAL := all

all: $(TOP_DIR)/quantum/version.h

docker-build:
	docker run --rm -v $(CURDIR):/qmk_firmware qmkfm/qmk_firmware make

# Generate the version.h file
KEYMAP_VERSION := $(shell git describe --abbrev=6 --always 2>/dev/null)
QMK_VERSION := $(shell cd $(TOP_DIR) && git describe --abbrev=6 --always --tags 2>/dev/null)
BUILD_DATE := $(shell date +"%Y-%m-%d-%H:%M:%S")

$(TOP_DIR)/quantum/version.h:
	echo '#define QMK_VERSION "$(QMK_VERSION)"' > $@
	echo '#define QMK_BUILDDATE "$(BUILD_DATE)"' >> $@

# Apply embedded qmk_firmware patches
PATCHES := \
	patches/0001-makefile-changes-to-support-a-parent-directory.patch \
	patches/0002-add-top_dir-in-the-correct-place.patch               \
	patches/0003-fix-tests.patch                                      \
	patches/0004-More-TOP_DIR-fixes.patch                             \
	patches/0005-Allow-overriding-keymap-file-with-KEYMAP_C-value-in-.patch

apply-patch = patch -d $(TOP_DIR) --forward --no-backup-if-mismatch -r- -p1 -i $(CURDIR)/$(1)
q.mk: $(PATCHES)
	git -C $(TOP_DIR) reset --hard >&2
	$(foreach patch, $(PATCHES), $(call apply-patch,$(patch));)
	echo "include qmk_firmware/build_keyboard.mk" > $@

# The dark magic of GNU make.
# Presence of q.mk means that the target was executed and patches were applied
include q.mk
