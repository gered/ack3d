# BPIC

Source code for the `BPIC` utility, which is a resource file builder/compiler for the ACK-3D
engine. A Watcom `makefile` has been added, replacing a build script for Lary's custom `MK.EXE` 
tool.

This can be used to build the `pics.dtf`-type resource files used by all the demo/example projects.
All the details regarding ACK-3D resource files can be found in Appendix A of the book.

Unlike the version that shipped on the book's CD, this version has been updated so that it does
not assume that files are located in the current working directory. Instead, files specified by
the input `.DAT` file are assumed to be located beside that file, making it possible to do things
like place `BPIC.EXE` somewhere on your PATH instead of needing to copy it around to all of your
project asset directories.

## Requirements

To build you will need Watcom C++. I used Watcom C++ 10.0a, but you should be able to build with 
a newer version (maybe even a _slightly_ older version too).

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

In either case, you should get an output artifact `BPIC.EXE` on successful build.