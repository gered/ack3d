target_config = debug

test_dat_file = ..\example\assets\example.dat
test_out_file = pics.dtf

target_name = bpic

object_files = bpic.obj


cc_flags_debug = /d2 /zp1 /4r /fp3 /j
cc_flags_release = /d1+ /zp1 /4r /fp3 /onetx /j
cc_flags = /mf $(cc_flags_$(target_config))

link_flags_debug = debug all
link_flags_release = debug all
link_flags = $(link_flags_$(target_config))

.NOCHECK
build : $(target_name).exe

.c.obj: .AUTODEPEND
    wcc386 $[. /zq $(cc_flags)

.asm.obj: .AUTODEPEND
    tasm $[. /t $(asm_flags)

$(target_name).lnk: $(object_files)
    %create $^@
    %append $^@ NAME $(target_name).exe
    %append $^@ SYSTEM DOS4G
    %append $^@ OPTION QUIET
    %append $^@ OPTION STACK=16k
    @for %i in ($(object_files)) do %append $^@ FILE %i

$(target_name).exe: $(object_files) $(target_name).lnk
    wlink $(link_flags) @$(target_name).lnk

clean : .SYMBOLIC
    del *.obj
    del *.err
    del $(target_name).exe
    del $(target_name).lnk

.NOCHECK
run : $(target_name).exe
    $(target_name).exe $(test_dat_file) $(test_out_file)

.NOCHECK
debug : $(target_name).exe
    wd /swap /trap=rsi $(target_name).exe $(test_dat_file) $(test_out_file)

