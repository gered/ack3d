# Example ACK-3D Application

![EXAMPLE1](https://github.com/gered/ack3d/raw/master/scrnshot/example1.gif "EXAMPLE1")
![EXAMPLE2](https://github.com/gered/ack3d/raw/master/scrnshot/example2.gif "EXAMPLE2")

Source files and assets for a simple example project utilizing the ACK-3D engine. Uses art assets
found on the CD included with the book "Amazing 3-D Games Adventure Set" by Lary Myers, published 
in 1995.

The purpose of this project is to provide a very simple application that helps illustrate things
discussed in detail in chapters 11 and 14 of the book. Chapter 14 in particular goes through the
process of creating an ACK-3D application from scratch, but it is Windows-specific. This project
aims to be somewhat of a DOS-equivalent of the project presented in that chapter.

## Requirements

To build you will need Watcom C++. I used Watcom C++ 10.0a, but you should be able to build with
a newer version (maybe even a _slightly_ older version too).

You must build `ACKLIB.LIB` prior to building this project. Please check the `/ack_lib` directory
at the root of this repository first.

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

In either case, you should get an output artifact `EXAMPLE.EXE` on successful build.

## Editing the Map

You must build the DOS map editor. Please check the `/map_edit` directory at the root of this 
repository first.

Once you have a working DOS map editor, run:

```
> \path\to\mapedit.exe \path\to\example\assets\example.med
```

To try out your map alterations you will need to rebuild the `pics.dtf` resource file.

To add/remove bitmaps you will need to edit the `example.med` file for the map editor. However,
you **ALSO** need to modify the `example.dat` and `example.inf` files to successfully rebuild the
`pics.dtf` resource file. See Appendix A and the last section of Chapter 15 in the book for more
information on ACK-3D resource files and using the DOS map editor. Please make sure you are well
familiar with these chapters, as getting all these files exactly right can be quite tricky!

## Rebuilding `pics.dtf`

You must build the `BPIC` tool. Please check the `/bpic` directory at the root of this repository
first.

If you wish to make changes to the assets used by this project (bitmaps and/or map), make whatever
changes you like to the `example.dat` file (which just lists the files to be included in the 
resource file) and `example.inf` file. Then to build a new `pics.dtf` run:

```
> \path\to\bpic.exe \path\to\example\assets\example.dat \path\to\example\pics.dtf
```

See Appendix A and the last section of Chapter 15 in the book for more information on ACK-3D 
resource files and using the DOS map editor. Please make sure you are well familiar with these 
chapters, as getting all these files exactly right can be quite tricky!
