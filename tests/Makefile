QMK_DIR  := ../vendor/qmk_firmware
CXXFLAGS += -std=c++17
CXXFLAGS += -fno-exceptions -fno-rtti
CXXFLAGS += -fomit-frame-pointer -falign-functions=16 -ffunction-sections -fdata-sections -fno-common -fshort-wchar 
CXXFLAGS += -Os -w -Wall -Wundef -Werror -pedantic
CXXFLAGS += -I../src
CXXFLAGS += -I../vendor
CXXFLAGS += -I$(QMK_DIR)
CXXFLAGS += -I$(QMK_DIR)/tmk_core
CXXFLAGS += -I$(QMK_DIR)/quantum
CXXFLAGS += -I$(QMK_DIR)/quantum/keymap_extras
CXXFLAGS += -I$(QMK_DIR)/quantum/audio
CXXFLAGS += -I$(QMK_DIR)/quantum/process_keycode
CXXFLAGS += -I$(QMK_DIR)/tmk_core/common

OBJ    = parser-test.o
OUTPUT = parser-test

test: $(OUTPUT) $(OUTPUT).o $(OUTPUT).sym
	nm -C --defined-only $<.o | diff -du $<.sym -
	./$<

clean:
	$(RM) $(OUTPUT) *.o

$(OUTPUT).o: ../src/util/*.hpp

%.sym: | %.o
	nm -C --defined-only $| >$@

%.dep: %.cpp
	$(CXX) $(CXXFLAGS) -M $<
