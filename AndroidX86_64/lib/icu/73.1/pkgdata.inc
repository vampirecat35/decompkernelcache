GENCCODE_ASSEMBLY_TYPE=-a gcc
SO=so
SOBJ=so
A=a
LIBPREFIX=lib
LIB_EXT_ORDER=.73.1
COMPILE=/usr/local/android-studio/plugins/android-ndk/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android31-clang -D_REENTRANT  -DU_HAVE_ELF_H=1 -DU_HAVE_STRTOD_L=1 -DU_HAVE_XLOCALE_H=1 -DU_HAVE_STRING_VIEW=1  -DU_ATTRIBUTE_DEPRECATED= -O2 -std=c11 -Wall -pedantic -Wshadow -Wpointer-arith -Wmissing-prototypes -Wwrite-strings   -c
LIBFLAGS=-I/usr/local/android-studio/plugins/android-ndk/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include -DPIC -fPIC
GENLIB=/usr/local/android-studio/plugins/android-ndk/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android31-clang -O2 -std=c11 -Wall -pedantic -Wshadow -Wpointer-arith -Wmissing-prototypes -Wwrite-strings    -shared -Wl,-Bsymbolic
LDICUDTFLAGS=-nodefaultlibs -nostdlib
LD_SONAME=-Wl,-soname -Wl,
RPATH_FLAGS=
BIR_LDFLAGS=-Wl,-Bsymbolic
AR=/usr/local/android-studio/plugins/android-ndk/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar
ARFLAGS=r
RANLIB=/usr/local/android-studio/plugins/android-ndk/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib
INSTALL_CMD=/usr/bin/install -c
