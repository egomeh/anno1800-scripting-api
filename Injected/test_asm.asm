PUBLIC test_function_to_hook

.code

test_function_to_hook PROC

mov [rsp + 8h], rbx
push rbp
push rsi
push rdi
push r12
push r13
push r14
push r15

sub rsp, 0230h
add rsp, 0230h

pop r15
pop r14
pop r13
pop r12
pop rdi
pop rsi
pop rbp
ret

test_function_to_hook ENDP

END
