##
#
# AnV LZVN/LZSS kernel cache decompressor V1.0
#
# Intel 64-bit (x86_64) version
#
##

PREFIX=/usr/local
XCODEPATH=/Applications/Xcode.app
PLATFORM=MacOSX.platform
SDK=MacOSX.sdk
SDKPREFIX=$(XCODEPATH)/Contents/Developer/Platforms/$(PLATFORM)/Developer/SDKs/$(SDK)

NASM=nasm
CC=clang -I$(PREFIX)/include -F$(SDKPREFIX)/System/Library/Frameworks
AR=ar
RANLIB=ranlib
CODESIGN=codesign
CSIDENT=-
LIBS=-L. -llzvn -F$(SDKPREFIX)/System/Library/Frameworks -framework IOKit -framework CoreFoundation
INSTALL=install
ARFLAGS=cru
CFLAGS=-arch x86_64 -Os
ASFLAGS=$(CFLAGS)

all: decompkernelcache

# .asm.o:
#	$(NASM) -o $@ -f macho64 $<

# .s.o:
#	$(CC) $(ASFLAGS) -c $< -o $@

# .S.o:
#	$(CC) $(ASFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

WINSUPSRC/IOCFUnserialize.o: WINSUPSRC/IOCFUnserialize.c
lzvndec.o: lzvndec.c
lzvn_encode.o: lzvn_encode.c

liblzvn.a: lzvndec.o lzvn_encode.o
	$(AR) $(ARFLAGS) $@ lzvndec.o lzvn_encode.o
	$(RANLIB) liblzvn.a

decompkernelcache: decompkernelcache.o WINSUPSRC/IOCFUnserialize.o liblzvn.a
	$(CC) $(CFLAGS) -o $@ decompkernelcache.o WINSUPSRC/IOCFUnserialize.o $(LIBS)
	$(CODESIGN) -s "$(CSIDENT)" $@

clean:
	rm -f *.o WINSUPSRC/IOCFUnserialize.o liblzvn.a decompkernelcache

install: decompkernelcache lzvn.h liblzvn.a
	$(INSTALL) decompkernelcache $(PREFIX)/bin
	$(INSTALL) liblzvn.a $(PREFIX)/lib
	$(INSTALL) lzvn.h $(PREFIX)/include
