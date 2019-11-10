# ACK-3D for DOS/Watcom

This repository contains fixed up code and demo projects/assets for the ACK-3D engine that 
was included in the book, "Amazing 3-D Games Adventure Set" by Lary Myers, published in 1995.

The CD included with that book contained multiple versions of the ACK-3D engine, two of which were
for DOS (one for Watcom C++ 9.5 and the other Borland C++ 4.0/4.5) and another copy for Windows
which was also the latest version of the ACK-3D engine at that time. The two DOS versions do not
build out of the box without issues (crash bugs, etc). Additionally, the code for the DOS version
of the map editor does not build as-is.

In this repository, I have taken the Windows version included on the book's CD and back-ported it
to DOS using Watcom C++ 10.0a. I have also fixed build issues with the DOS map editor and cleaned
up the makefiles and build scripts for all included tools and projects. Lary used his own custom
`MK.EXE` tool which kind of did stuff similar to a makefile, but it was his own custom tool that
was limited. I have not really left Windows compatibility as that was not a goal of mine. Having
said that, I don't think it would be super difficult to add that back if someone desired it.

Some minor bugs and other improvements have been made, but nothing major. Many of the existing bugs
that were present in ACK-3D remain (e.g. secret doors render strangely or not at all in many cases,
and multi-height walls still can be seen through ceilings, etc.). I may try to fix some of these
things in the future.

Finally, I have included a somewhat simpler example project that has been thoroughly commented.
This project I hope serves to demonstrate what Lary covered in chapter's 11 and 14 of the book,
but instead focused on DOS only and without going overboard with extra things like the FDEMO
and MALL projects both did.

Also, I wrote two posts about this whole adventure [here](http://blarg.ca/amazing-3-d-games-adventure-set/) 
and [here](http://blarg.ca/rebuilding-ack-3d-for-dos/) if anyone is interested.

## Repository Layout

### `/ack_lib`

The core ACK-3D library, based on the Windows version included on the book's CD.

### `/bpic`

Source for the `BPIC` utility discussed in the book in Appendix A, used for constructing
resource files.

### `/map_edit`

Source and data files for the DOS version of the map editor. Unlike the Windows version
"ACKEDIT" (not included in this repository), this does not work with resource files directly and
so is not _quite_ as convenient to work with.

### `/fdemo`

Source and assets for the "FDEMO" project discussed briefly in the book in Appendix C.

### `/mall`

Source and assets for the "MALL" project also discussed briefly in the book in Appendix C. This
project is very, very similar to the "FDEMO" project and really only differs by assets used.

### `/example`

An example project I created from scratch (but using assets from the Station Escape, FDEMO and
MALL projects from the book's CD).
