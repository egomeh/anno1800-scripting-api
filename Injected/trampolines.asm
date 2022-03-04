PUBLIC game_time_hook_trampoline

.data

; 8 bytes used to store the hook manager
hook_manager_address db '        '

.code

game_time_hook_trampoline PROC
ret
game_time_hook_trampoline ENDP

END