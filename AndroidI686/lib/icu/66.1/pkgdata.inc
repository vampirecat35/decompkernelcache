GENCCODE_ASSEMBLY_TYPE=-a gcc
SO=so
SOBJ=so
A=a
LIBPREFIX=lib
LIB_EXT_ORDER=.66.1
COMPILE=/Users/andyvand/Downloads/android-ndk-r21b/toolchains/llvm/prebuilt/darwin-x86_64/bin/i686-linux-android23-clang -D_REENTRANT  -DU_HAVE_ELF_H=1 -DU_HAVE_STRTOD_L=0 -DU_HAVE_STRING_VIEW=1  -DU_ATTRIBUTE_DEPRECATED= -O3 -O2 -fPIC -std=c11 -Wall -pedantic -Wshadow -Wpointer-arith -Wmissing-prototypes -Wwrite-strings   -c
LIBFLAGS=-I/Users/andyvand/Downloads/android-ndk-r21b/i686_prefix/include -DPIC -fPIC
GENLIB=/Users/andyvand/Downloads/android-ndk-r21b/toolchains/llvm/prebuilt/darwin-x86_64/bin/i686-linux-android23-clang -O3 -O2 -fPIC -std=c11 -Wall -pedantic -Wshadow -Wpointer-arith -Wmissing-prototypes -Wwrite-strings   -O2 -fPIC  -shared -Wl,-Bsymbolic
LDICUDTFLAGS=-nodefaultlibs -nostdlib
LD_SONAME=-Wl,-soname -Wl,
RPATH_FLAGS=
BIR_LDFLAGS=-Wl,-Bsymbolic
AR=/Users/andyvand/Downloads/android-ndk-r21b/toolchains/llvm/prebuilt/darwin-x86_64/bin/i686-linux-android-ar
ARFLAGS=r
RANLIB=/Users/andyvand/Downloads/android-ndk-r21b/toolchains/llvm/prebuilt/darwin-x86_64/bin/i686-linux-android-ranlib
INSTALL_CMD=/usr/bin/install -c
