.286p
.model medium
.stack 1
.data
.code

main proc
call far ptr reboot
main endp

_TEXT2 segment byte public 'CODE'
assume cs:_TEXT2

reboot proc
mov al,0feh
out 64h,al
reboot endp

_TEXT2 ends

end main
