ifeq ($(OS),)
OS=$(shell uname)
endif

ifeq ($(OS),Windows_NT)
DLLEXT=dll
else
DLLEXT=so
endif

LUA_VERSION?=5.3

PREFIX?=/usr
INSTALL_LUA=$(PREFIX)/share/lua/$(LUA_VERSION)
INSTALL_BIN=$(PREFIX)/lib/lua/$(LUA_VERSION)
CPPFLAGS=-Wall -Wextra -Werror -O2
CFLAGS=-fvisibility=hidden

ifeq ($(OS),Linux)
CPPFLAGS+=-I/usr/include/libusb-1.0 "-DLUAUSB_API=__attribute__((visibility(\"default\")))"
CFLAGS+=-fPIC
LDLIBS+=-lusb-1.0
endif
ifeq ($(OS),Windows_NT)
ifneq ($(LUA_VERSION),5.1)
$(error only Lua 5.1 is supported on Windows)
endif
ARCH=32
LIBUSB_DIR=./libusbx-1.0.14-win
LUABIN_DIR=./lua5_1_4_Win$(ARCH)_dll8_lib
CPPFLAGS+=-I$(LUABIN_DIR)/include -I$(LIBUSB_DIR)/include/libusbx-1.0 "-DLUAUSB_API=__declspec(dllexport)"
LDLIBS+=-l:$(LUABIN_DIR)/lua5.1.dll -l:$(LIBUSB_DIR)/MinGW$(ARCH)/dll/libusb-1.0.dll
endif

build:usb.$(DLLEXT)

clean:
	rm -f usb.$(DLLEXT) *.o

cleandist:clean
	rm -f enums.c enums.h structs.c structs.h

install:build
	install -d $(INSTALL_BIN)
	install *.$(DLLEXT) $(INSTALL_BIN)

usb.$(DLLEXT): CPPFLAGS+=-Dluaopen_module=luaopen_usb

%.$(DLLEXT): %.c
	$(LINK.c) -shared $^ $(LOADLIBES) $(LDLIBS) -o $@

usb.$(DLLEXT):enums.o structs.o compat.o

usb.c:enums.h structs.h

structs.c enums.c enums.h structs.h:gen.lua
	lua gen.lua

.PHONY:build clean cleandist pureinstall install

# Copyright (c) 2010 Jérôme Vuarand
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
