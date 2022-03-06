PUBLIC game_time_hook_trampoline

push_volatile MACRO
pushf
push   rax
push   rbx
push   rcx
push   rdx
push   r8
push   r9
push   r10
push   r11
ENDM

pop_volatile MACRO
pop    r11
pop    r10
pop    r9
pop    r8
pop    rdx
pop    rcx
pop    rbx
pop    rax
popf
ENDM

fill_hook_data MACRO
push r9
push r8
push rdi
push rsi
push rdx
push rcx
push rbx
push rax
mov rdx, rsp
ENDM

clean_up_hook_data MACRO
add rsp, 040h
ENDM

.data

extrn HookManagerServiceHook : proc

.code

game_time_hook_trampoline PROC

push_volatile

fill_hook_data
mov rcx, 1 ; service hook #1 game time
sub rsp, 020h
call HookManagerServiceHook
add rsp, 020h
clean_up_hook_data

pop_volatile

; Do the original work at the hook point

mov rcx, rax
add qword ptr [rbx + 60h], rax
inc dword ptr [rbx + 70h]
movsxd rax, dword ptr [rbx + 0C8h]

; Then return back to the original code
; We can `ret` as long as the calling
; code used a `call` to get here

ret

game_time_hook_trampoline ENDP

END