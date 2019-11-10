# FDEMO

![FDEMO1](https://github.com/gered/ack3d/raw/master/scrnshot/fdemo1.gif "FDEMO1")
![FDEMO2](https://github.com/gered/ack3d/raw/master/scrnshot/fdemo2.gif "FDEMO2")

Source code and assets for the FDEMO project contained on the CD that was included on the book
"Amazing 3-D Games Adventure Set" by Lary Myers, published in 1995. The source code has been
fixed up a little bit. A Watcom `makefile` has been added, replacing a build script for Lary's 
custom `MK.EXE` build tool.

## Requirements

To build you will need Watcom C++ and Borland TASM. I used Watcom C++ 10.0a and Borland TASM 4.0,
but you should be able to build with newer versions (maybe even _slightly_ older versions too).

You must build `ACKLIB.LIB` prior to building this project. Please check the `/ack_lib` directory
at the root of this repository first.

To hear the background music you will need a Sound Blaster compatible sound card. This project is
hard coded to use address 220h, IRQ 5, DMA 1. However you can adjust these settings in the
`StartBGmusic` function found in `fdemo.c`.

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

In either case, you should get an output artifact `FDEMO.EXE` on successful build.

## Editing the Map

You must build the DOS map editor. Please check the `/map_edit` directory at the root of this 
repository first.

Once you have a working DOS map editor, run:

```
> \path\to\mapedit.exe \path\to\fdemo\assets\fdemo.l01
```

To try out your map alterations you will need to rebuild the `pics.dtf` resource file.

To add/remove bitmaps you will need to edit the `fdemo.l01` file for the map editor. However,
you **ALSO** need to modify the `fdemo.dat` and `fdemo.inf` files to successfully rebuild the
`pics.dtf` resource file. See Appendix A and the last section of Chapter 15 in the book for more
information on ACK-3D resource files and using the DOS map editor. Please make sure you are well
familiar with these chapters, as getting all these files exactly right can be quite tricky!

## Rebuilding `pics.dtf`

You must build the `BPIC` tool. Please check the `/bpic` directory at the root of this repository
first.

If you wish to make changes to the assets used by this project (bitmaps and/or map), make whatever
changes you like to the `fdemo.dat` file (which just lists the files to be included in the 
resource file) and `fdemo.inf` file. Then to build a new `pics.dtf` run:

```
> \path\to\bpic.exe \path\to\fdemo\assets\fdemo.dat \path\to\fdemo\pics.dtf
```

See Appendix A and the last section of Chapter 15 in the book for more information on ACK-3D 
resource files and using the DOS map editor. Please make sure you are well familiar with these 
chapters, as getting all these files exactly right can be quite tricky!
