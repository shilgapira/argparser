CC ?= cc

PREFIX   := /usr
LIBDIR   := $(PREFIX)/lib
BUILDDIR := build

CFLAGS  := -std=c17 -Werror -Wextra -D_GNU_SOURCE
LDFLAGS := -O3 -fvisibility=hidden -fpic -flto \
	   -march=native -fomit-frame-pointer -funroll-loops

CP   := cp -f
ECHO := @echo

NAME        := libargparser
DESCRIPTION := C command-line argument parser
VERSION     := 0.1.1

SOURCE := src/argparser.c
HEADER := src/argparser.h
OBJECT := $(BUILDDIR)/argparser.o
STATIC := $(BUILDDIR)/libargparser.a
SHARED := $(BUILDDIR)/libargparser.so
PKGCFG := $(BUILDDIR)/libargparser.pc

all: $(SHARED) $(STATIC) $(PKGCFG)

$(BUILDDIR):
	$(RM) $(BUILDDIR)
	mkdir -p $(BUILDDIR)

$(OBJECT): $(BUILDDIR) $(SOURCE) $(HEADER)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCE) -c -o $@

$(SHARED): $(OBJECT)
	$(CC) $(LDFLAGS) $(OBJECT) -shared -o $@

$(STATIC): $(OBJECT)
	ar r $@ $(OBJECT)
	ranlib $(STATIC)

$(PKGCFG):
	$(ECHO) Name: $(NAME) > $(PKGCFG)
	$(ECHO) Description: $(DESCRIPTION) >> $(PKGCFG)
	$(ECHO) Version: $(VERSION) >> $(PKGCFG)
	$(ECHO) Libs: -L$(LIBDIR) -largparser >> $(PKGCFG)

install: $(SHARED) $(STATIC) $(PKGCFG)
	$(CP) $(PKGCFG) $(LIBDIR)/pkgconfig
	$(CP) $(SHARED) $(LIBDIR)
	$(CP) $(STATIC) $(LIBDIR)

uninstall:
	$(RM) $(LIBDIR)/$(notdir $(SHARED))
	$(RM) $(LIBDIR)/$(notdir $(STATIC))

clean:
	$(RM) -r $(BUILDDIR)
	$(RM) compile_flags.txt

compile_flags.txt:
	$(ECHO) $(CFLAGS) | tr " " "\n" > compile_flags.txt

.PHONY: install uninstall clean compile_flags.txt
