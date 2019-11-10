# DOS Map Editor for ACK-3D

![MAPEDIT1](https://github.com/gered/ack3d/raw/master/scrnshot/mapedit1.gif "MAPEDIT1")
![MAPEDIT2](https://github.com/gered/ack3d/raw/master/scrnshot/mapedit2.gif "MAPEDIT2")

Source code and assets for the DOS map editor contained on the CD that was included on the book
"Amazing 3-D Games Adventure Set" by Lary Myers, published in 1995. The source code has been
fixed up a little bit and sources/assets for a second, older version of this map editor have been
removed. A Watcom `makefile` has been added, replacing a build script for Lary's custom `MK.EXE` 
build tool.

Unlike the version that shipped on the book's CD, this version has been updated so that it does
not assume that files are all located in the current working directory. Instead the following is
what happens for path handling:

* The `medit.dtf` file required by the map editor is assumed to be located in the same directory as `MAPEDIT.EXE`
* Files specified by the map editor description file are assumed to be located in the same directory as that description file.

These changes make it possible to do things like place `MAPEDIT.EXE` and `MEDIT.DTF` somewhere on
your PATH instead of needing to copy them around to all of your project asset directories.

## Requirements

To build you will need Watcom C++ and Borland TASM. I used Watcom C++ 10.0a and Borland TASM 4.0,
but you should be able to build with newer versions (maybe even _slightly_ older versions too).

You must build `ACKLIB.LIB` prior to building this project. Please check the `/ack_lib` directory
at the root of this repository first.

Because the DOS map editor requires it's own `medit.dtf` resource file, you must also build the
`BPIC.EXE` tool prior to building this project, as the included `makefile` will attempt to build
this resource file as part of a normal build. Please check the `/bpic` directory at the root of
this repository first.

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

In either case, you should get an output artifact `MAPEDIT.EXE` on successful build.

## How to Run

To run the map editor, you must provide it with a text map description file or "configuration file"
as it is referred to in the book. This file specifies what `.MAP` file to load, in addition to
what bitmaps to load, what objects are available for placement and a bunch of other things. These
are very similar to the `.INF` files used by ACK-3D resource files (e.g. `pics.dtf`) but do **NOT**
confuse the two! They are not the same thing!

This map editor does **NOT** read ACK-3D resource files like `pics.dtf`. It is only capable of
producing `.MAP` files that are to be included in resource files. This is very much unlike the 
Windows version of the map editor that was included on the book's CD (which is not included in this
repository).

Please read the last section of Chapter 15 in the book for more information on using the DOS map
editor and what needs to go in configuration files. Unfortunately, the book's description of
configuration files is very light (only a single page), so you will also want to refer to the
`/fdemo/assets/fdemo.l01`, `/mall/assets/mall.l01` and `/example/assets/example.med` configuration 
files found in this repository as examples to help you write your own.
