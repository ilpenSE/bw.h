# bw.h

- This is a bit writer/reader single header library.
- Written in plain C99, OK to use in C++.
- BitWriter structure is used to read bit(s) by the way.
- For documentation, header contains necessary information. But if you want, you can use `doxygen` command to create documentation in HTML or PDF.

## Quick Start

- Include the header to a translation unit then define `BW_IMPL` or `BW_IMPLEMENTATION` in one translation unit for implementations.
```c
#define BW_IMPL
#include "bw.h"
```
