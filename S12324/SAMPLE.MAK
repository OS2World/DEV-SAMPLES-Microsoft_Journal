#=========================================
# Sample makefile
#=========================================

cp=cl -c -ALu -G2s -Od -Zpei
cv=/CO

.c.obj:
    $(cp) $*.c

#=========================================
#  Dependencies
#=========================================

sample.obj: sample.c
    $(cp) sample.c

sample.exe: sample.obj
    link $(cv) sample.obj,sample.exe,,/NOD LLIBCE OS2,sample.def
