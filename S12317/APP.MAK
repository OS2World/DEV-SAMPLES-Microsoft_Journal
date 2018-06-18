
cp=cl -d -c -AM -G2sw -Os -Zpei -W3

.c.obj:
    $(cp) $*.c

APP.obj: APP.c APP.h

wndproc.obj: wndproc.c APP.h

appcmd.obj: appcmd.c APP.h

apppaint.obj: apppaint.c app.h

APP.res: APP.rc APP.ico APP.h
    rc -r APP.rc

APP.exe: APP.obj wndproc.obj appcmd.obj apppaint.obj APP.res APP.def
    link @APP.rsp
    rc APP.res APP.EXE

