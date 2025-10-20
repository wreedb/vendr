# vim:ft=bash
unset CFLAGS CXXFLAGS CPPFLAGS CC CXX CPP LDFLAGS LD

export CPPFLAGS="-D_FORTIFY_SOURCE=2"
export CXXFLAGS="-march=x86-64 -mtune=generic -pipe -O2 -fstack-clash-protection -fstack-protector-strong -fcf-protection=full -fdiagnostics-color -fPIC"
export LDFLAGS="-Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,--as-needed"
