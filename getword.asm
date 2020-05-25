.686
.MODEL FLAT, C
include         \masm32\include\kernel32.inc
include         \masm32\include\user32.inc
includelib      \masm32\lib\kernel32.lib
includelib      \masm32\lib\user32.lib

.stack

.data

.code

getword			proc

				push edi
				push esi
				mov edi, [esp+16]
				mov esi, [esp+12]
				mov eax, 2000Dh			;0D = '\r', 2 - учёт смещения для \n \r

next:			mov ah, [esi]
				cmp al, ah
				je stop
				cmp ah, 0
				je eof
				movsb
				add eax, 10000h
				jmp next

stop:			mov byte ptr [edi], 0
				shr eax, 16
				pop esi 
				pop edi
				retn

eof:			mov byte ptr [edi], 0
				xor eax, eax
				pop esi 
				pop edi
				retn
				
getword			endp

END