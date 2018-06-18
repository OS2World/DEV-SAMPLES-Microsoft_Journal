PROJ = UIDEMO
PROJFILE = UIDEMO.MAK
DEBUG = 1

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
CC  = cl
CFLAGS_G  = /BATCH
CFLAGS_D  = /Gi$(PROJ).mdt /Zi /Od
CFLAGS_R  = /Ot
BC  = bc
BCFLAGS_D  = /Zi /S
BCFLAGS_G  = /O /FPi /X /Lp /Fs /Ah
MAPFILE_D  = $(PROJ).map
MAPFILE_R  = NUL
LFLAGS_G  = /NOI /STACK:4096  /BATCH
LFLAGS_D  = /CO /M /NOF /NOP  /PMTYPE:VIO
LFLAGS_R  = /EXE /FAR /PACKC /PACKD  /PMTYPE:VIO
LINKER  = link
ILINK  = ilink
LRF  = echo > NUL
ASM  = masm
BRFLAGS  = /o $(PROJ).bsc /Iu
BROWSE  = 0
CVFLAGS  = /43

OBJS  = UIDEMO.obj GENERAL.obj MOUSE.obj MENU.obj WINDOW.obj UIASM.obj\
        DOSBEEP.obj
SBRS  = UIDEMO.sbr GENERAL.sbr MOUSE.sbr MENU.sbr WINDOW.sbr UIASM.sbr\
        DOSBEEP.sbr

all: $(PROJ).exe

.SUFFIXES:
.SUFFIXES: .obj .sbr .bas

UIDEMO.obj : UIDEMO.BAS general.bi mouse.bi menu.bi window.bi

UIDEMO.sbr : UIDEMO.BAS general.bi mouse.bi menu.bi window.bi

GENERAL.obj : GENERAL.BAS general.bi mouse.bi

GENERAL.sbr : GENERAL.BAS general.bi mouse.bi

MOUSE.obj : MOUSE.BAS general.bi mouse.bi menu.bi

MOUSE.sbr : MOUSE.BAS general.bi mouse.bi menu.bi

MENU.obj : MENU.BAS general.bi mouse.bi menu.bi

MENU.sbr : MENU.BAS general.bi mouse.bi menu.bi

WINDOW.obj : WINDOW.BAS general.bi mouse.bi menu.bi window.bi

WINDOW.sbr : WINDOW.BAS general.bi mouse.bi menu.bi window.bi

UIASM.obj : UIASM.BAS

UIASM.sbr : UIASM.BAS

DOSBEEP.obj : DOSBEEP.BAS

DOSBEEP.sbr : DOSBEEP.BAS


$(PROJ).bsc : $(SBRS)
        $(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

$(PROJ).exe : $(OBJS)
!IF $(DEBUG)
        $(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_D)
$(LLIBS_G: = +^
) +
$(LLIBS_D: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_D);
<<
!ELSE
        $(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_R)
$(LLIBS_G: = +^
) +
$(LLIBS_R: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_R);
<<
!ENDIF
        $(LINKER) @$(PROJ).lrf


.bas.obj :
!IF $(DEBUG)
        $(BC) /Z $(BCFLAGS_G) $(BCFLAGS_D) $<, $@;
!ELSE
        $(BC) /Z $(BCFLAGS_G) $(BCFLAGS_R) $<, $@;
!ENDIF

.bas.sbr :
!IF $(DEBUG)
        $(BC) /Z $(BCFLAGS_G) $(BCFLAGS_D) $<, nul;
!ELSE
        $(BC) /Z $(BCFLAGS_G) $(BCFLAGS_R) $<, nul;
!ENDIF


run: $(PROJ).exe
        $(PROJ).exe $(RUNFLAGS)

debug: $(PROJ).exe
        CVP $(CVFLAGS) $(PROJ).exe $(RUNFLAGS)
