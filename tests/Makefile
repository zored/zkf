CXXFLAGS += -std=c++17
CXXFLAGS += -fno-exceptions -fno-rtti
CXXFLAGS += -fomit-frame-pointer -falign-functions=16 -ffunction-sections -fdata-sections -fno-common -fshort-wchar 
CXXFLAGS += -Os -w -Wall -Wundef -Werror -pedantic
CXXFLAGS += -I. -I..
CXXFLAGS += -I../qmk_firmware
CXXFLAGS += -I../qmk_firmware/tmk_core
CXXFLAGS += -I../qmk_firmware/quantum
CXXFLAGS += -I../qmk_firmware/quantum/keymap_extras
CXXFLAGS += -I../qmk_firmware/quantum/audio
CXXFLAGS += -I../qmk_firmware/quantum/process_keycode
CXXFLAGS += -I../qmk_firmware/tmk_core/common

OBJ    = parser-test.o
OUTPUT = parser-test

test: $(OUTPUT) $(OUTPUT).o $(OUTPUT).sym
	nm -C --defined-only $<.o | diff -du $<.sym -
	./$<

clean:
	$(RM) $(OUTPUT) *.o

$(OUTPUT).o: ../util/*.hpp

%.sym: | %.o
	nm -C --defined-only $| >$@
