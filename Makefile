LUAPKG = lua5.2
CFLAGS = `pkg-config $(LUAPKG) --cflags` -DLUA_COMPAT_MODULE -fPIC -O3 -Wall
LDFLAGS = -shared
INSTALL_PATH = `pkg-config $(LUAPKG) --variable=INSTALL_CMOD`

all: gettext.so

gettext.lo: lua-gettext.c
	$(CC) $(CFLAGS) -c lua-gettext.c -o gettext.lo

gettext.so: gettext.lo
	$(CC) $(LDFLAGS) gettext.lo -o gettext.so

install: gettext.so
	install -D -s gettext.so $(INSTALL_PATH)/gettext.so

clean:
	$(RM) gettext.lo gettext.so
