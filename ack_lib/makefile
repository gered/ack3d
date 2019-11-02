target_config = debug

target_name = acklib

object_files = &
    ackbkgd.obj &
    ackdata.obj &
    ackdoor.obj &
    ackfloor.obj &
    ackgif.obj &
    ackiff.obj &
    ackinit.obj &
    ackldbmp.obj &
    ackobj.obj &
    ackover.obj &
    ackpcx.obj &
    ackpov.obj &
    ackray.obj &
    ackrtn.obj &
    ackrtn1.obj &
    ackrtn3.obj &
    ackrtn4.obj &
    ackrtn5.obj &
    ackutil.obj &
    ackview.obj &
    ackwrap.obj &


cc_flags_debug = /d2 /zp1 /4r /fp3 /j
cc_flags_release = /d1+ /zp1 /4r /fp3 /ontx /oe=40 /j
cc_flags = /mf $(cc_flags_$(target_config))

link_flags_debug = debug all
link_flags_release = debug all
link_flags = $(link_flags_$(target_config))

asm_flags_debug = /zi
asm_flags_release = /zi
asm_flags = /m /ml $(asm_flags_$(target_config))


.c.obj: .AUTODEPEND
    wcc386 $[. /zq $(cc_flags)

.asm.obj: .AUTODEPEND
    tasm $[. /t $(asm_flags)


$(target_name).lbc: $(object_files)
    %create $^@
    @for %i in ($(object_files)) do %append $^@ +%i

$(target_name).lib: $(object_files) $(target_name).lbc
    wlib /n /q /b $(target_name).lib @$(target_name).lbc


clean : .SYMBOLIC
    del *.obj
    del *.err
    del $(target_name).exe
    del $(target_name).lib
    del $(target_name).lnk
    del $(target_name).lbc

.NOCHECK
build : $(target_name).lib

.NOCHECK
library : $(target_name).lib

