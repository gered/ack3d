# ACK-3D Library

Source code for the latest version of the ACK-3D library (at least, at the time of publishing of
Lary Myer's book, "Amazing 3-D Games Adventure Set" in 1995) back-ported to DOS. Windows
compatibility has not been kept. A Watcom `makefile` has been added, replacing a build script for
Lary's custom `MK.EXE` build tool.

## Requirements

To build, you will need Watcom C++ and Borland TASM. I used Watcom C++ 10.0a and Borland TASM 4.0,
but you should be able to build with newer versions (maybe even _slightly_ older versions too).

## Building

Within this directory, run

```
> wmake
```

This will build using the included `makefile` using a debug build settings.

To build using a release configuration:

```
> wmake target_config=release
```

In either case, you should get an output artifact `ACKLIB.LIB` on successful build.

### Compiler Settings Note

Of particular note is that `/zp1` is specified for both debug and release builds. This matches
Lary's original build scripts but is also a requirement as some things in the code make 
assumptions that might not hold true for larger alignment settings (e.g. `/zp4`). Change this
at your own risk!
