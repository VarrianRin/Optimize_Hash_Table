.686
.MODEL FLAT, C
include         \masm32\include\kernel32.inc
include         \masm32\include\user32.inc
includelib      \masm32\lib\kernel32.lib
includelib      \masm32\lib\user32.lib

.stack

.data

.code
;-------------------------------------------
;int getword(char* text, char* word)
;gets \n-terminating string (pointed by word) from \0-terminating string (pointed by text) and lenght of word
;
;Return 	eax - lenght of word	
;
;Destroy	eax
;-------------------------------------------
getword			proc

				push edi
				push esi
				mov edi, [esp+16]
				mov esi, [esp+12]
				mov eax, 2000Dh			;0D = '\r', 2 - offset for \n \r

next:			mov ah, [esi]
				cmp al, ah
				je stop
				cmp ah, 0
				je term
				movsb
				add eax, 10000h
				jmp next

stop:			mov byte ptr [edi], 0
				shr eax, 16
				pop esi 
				pop edi
				retn

term:			mov byte ptr [edi], 0
				xor eax, eax
				pop esi 
				pop edi
				retn
				
getword			endp

;-------------------------------------------
;unsigned int Rolling(const char* word)
;gets hash of word; hash = SUM(ror word[i], 1) for each i until word[i] == '\0'
;
;Return 	eax - hash of word	
;
;Destroy	eax
;-------------------------------------------

Rolling			proc
				
			push esi
			push edx
			xor edx, edx
			xor eax, eax
			mov esi,  [esp+12]

next:			lodsb
			cmp al, 0
			je stop
			ror al, 1
			add edx, eax
			jmp next

stop:			mov eax, edx	
			pop edx
			pop esi
			retn

Rolling			endp

END
