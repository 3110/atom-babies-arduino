#pragma once

#define STR(x) #x
#define VER_STR(x, y, z) (STR(x) "." STR(y) "." STR(z))
#define ATOM_BABIES_VERSION_MAJOR 0
#define ATOM_BABIES_VERSION_MINOR 0
#define ATOM_BABIES_VERSION_REVISION 1
#define ATOM_BABIES_VERSION                                       \
    VER_STR(ATOM_BABIES_VERSION_MAJOR, ATOM_BABIES_VERSION_MINOR, \
            ATOM_BABIES_VERSION_REVISION)
