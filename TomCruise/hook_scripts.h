#pragma once


namespace HookScripts
{

const char* TimeAndFrame =
R"(
-pattern:
48 8b c8                        // mov    rcx,rax
49 01 46 60                     // add    QWORD PTR [r14+0x60],rax
41 ff 46 70                     // inc    DWORD PTR [r14+0x70]
49 63 86 c8 00 00 00            // movsxd rax,DWORD PTR [r14+0xc8]

-replacement:
48 b9 [detourAddress : 8]       // movabs rcx, detour address
ff d1                           // call   rcx

-detour:
48 8b c8                        // mov    rcx,rax
49 01 46 60                     // add    QWORD PTR [r14+0x60],rax
41 ff 46 70                     // inc    DWORD PTR [r14+0x70]
49 63 86 c8 00 00 00            // movsxd rax,DWORD PTR [r14+0xc8]
[push volatile]
49 8b 4e 60                     // mov    rcx,QWORD PTR [r14+0x60]
49 8b 56 70                     // mov    rdx,QWORD PTR [r14+0x70]
48 b8 [targetAddress : 8]       // movabs rax, hookAddress
FF D0                           // call rax
[pop volatile]
C3                              // ret
)";

const char* IslandBuildingIteration =
R"(
-pattern:
4c 8b dc                        // mov    r11,rsp
57                              // push   rdi
48 83 ec 70                     // sub    rsp,0x70
49 c7 43 a8 fe ff ff            // mov    QWORD PTR [r11-0x58],0xfffffffffffffffe
ff                              // 
49 89 5b 10                     // mov    QWORD PTR [r11+0x10],rbx
49 89 73 18                     // mov    QWORD PTR [r11+0x18],rsi
49 8b f0                        // mov    rsi,r8
8b fa                           // mov    edi,edx
48 8b d9                        // mov    rbx,rcx
48 83 b9 b0 05 00 00 00         // cmp    QWORD PTR [rcx+0x5b0],0x0

-replacement:
48 b8 [detourAddress : 8]       // movabs rax, detour address
ff e0                           // jmp   rax

-detour:
[push volatile]
48 b8 [targetAddress : 8]       // movabs rax, targetAddress
ff d0                           // call   rax
[pop volatile]
4c 8b dc                        // mov    r11,rsp
57                              // push   rdi
48 83 ec 70                     // sub    rsp,0x70
49 c7 43 a8 fe ff ff            // mov    QWORD PTR [r11-0x58],0xfffffffffffffffe
ff                              // 
49 89 5b 10                     // mov    QWORD PTR [r11+0x10],rbx
49 89 73 18                     // mov    QWORD PTR [r11+0x18],rsi
49 8b f0                        // mov    rsi,r8
8b fa                           // mov    edi,edx
48 8b d9                        // mov    rbx,rcx
48 83 b9 b0 05 00 00 00         // cmp    QWORD PTR [rcx+0x5b0],0x0
48 b8 [returnAddress : 8]       // movabs rax, detour address
ff e0                           // jmp   rax
)";

const char* UserThreadLoop =
R"(
-pattern:
0f b6 43 2c                     // movzx  eax,BYTE PTR [rbx+0x2c]
88 44 24 40                     // mov    BYTE PTR [rsp+0x40],al
0f b6 43 2d                     // movzx  eax,BYTE PTR [rbx+0x2d]
88 44 24 41                     // mov    BYTE PTR [rsp+0x41],al
0f b6 43 2e                     // movzx  eax,BYTE PTR [rbx+0x2e]

-replacement:
41 56                           // push   r14
49 be [detourAddress : 8]       // movabs r14,0x1122334455667788 
41 ff d6                        // call   r14
41 5e                           // pop    r14

-detour:
48 83 c4 10                     // add    rsp,0x10
0f b6 43 2c                     // movzx  eax,BYTE PTR [rbx+0x2c]
88 44 24 40                     // mov    BYTE PTR [rsp+0x40],al
0f b6 43 2d                     // movzx  eax,BYTE PTR [rbx+0x2d]
88 44 24 41                     // mov    BYTE PTR [rsp+0x41],al
0f b6 43 2e                     // movzx  eax,BYTE PTR [rbx+0x2e]
48 83 ec 10                     // sub    rsp,0x10
48 83 ec 08                     // sub    rsp,0x8
[push volatile]
48 b8 [targetAddress : 8]       // movabs rax, hookAddress
FF D0                           // call rax
[pop volatile]
48 83 c4 08                     // add    rsp,0x8
C3                              // ret
)";

const char* ShipListIteration =
R"(
-pattern:
48 89 4c 24 08                      // mov    QWORD PTR [rsp+0x8],rcx
55                                  // push   rbp
53                                  // push   rbx
56                                  // push   rsi
57                                  // push   rdi
41 54                               // push   r12
41 55                               // push   r13

-replacement:
48 b8 [detourAddress : 8]           // movabs rax, detourAddress
ff e0                               // jmp    rax

-detour:
48 89 4c 24 08                      // mov    QWORD PTR [rsp+0x8],rcx
55                                  // push   rbp
53                                  // push   rbx
56                                  // push   rsi
57                                  // push   rdi
41 54                               // push   r12
41 55                               // push   r13
[push volatile]
48 b8 [targetAddress : 8]           // movabs rax, targetAddress
ff d0                               // call   rax
[pop volatile]
48 b8 [returnAddress : 8]           // movabs rax, returnAddress
ff e0                               // jmp    rax
)";

const char* ShipContextSetHook =
R"(
-pattern:
48 c1 e1 10                         // shl    rcx,0x10
41 0f b7 c4                         // movzx  eax,r12w
48 0b c8                            // or     rcx,rax
48 89 0a                            // mov    QWORD PTR [rdx],rcx
4c 89 4c 24 40                      // mov    QWORD PTR [rsp+0x40],r9

-replacement:
50                                  // push   rax
48 b8 [detourAddress : 8]           // movabs rax, detourAddress
ff e0                               // jmp    rax
90
returnLocation
58                                  // pop    rax

-detour:
58                                  // pop    rax
48 c1 e1 10                         // shl    rcx,0x10
41 0f b7 c4                         // movzx  eax,r12w
48 0b c8                            // or     rcx,rax
48 89 0a                            // mov    QWORD PTR [rdx],rcx
4c 89 4c 24 40                      // mov    QWORD PTR [rsp+0x40],r9

[push volatile]
48 83 ec 20                         // sub    rsp,0x20
65 48 8b 0c 25 58 00 00 00          // mov    rcx,QWORD PTR gs:0x58
48 b8 [targetAddress : 8]           // movabs rax, targetAddress
ff d0                               // call   rax
48 83 c4 20                         // add    rsp,0x20
[pop volatile]
48 b8 [returnAddress : 8]           // movabs rax, returnAddress
ff d0                               // call   rax
)";

const char* RegionIteration =
R"(
-pattern:
48 8d 44 24 30                      // lea    rax, [rsp + 0x30]
48 89 84 24 90 00 00 00             // mov    QWORD PTR[rsp + 0x90], rax
49 8b 06                            // mov    rax, QWORD PTR[r14]

-replacement:
48 b8 [detourAddress : 8]           // movabs rax, detourAddress
ff e0                               // jmp    rax
returnLocation
90                                  // nop
58                                  // pop rax

-detour:
48 8d 44 24 30                      // lea    rax, [rsp + 0x30]
48 89 84 24 90 00 00 00             // mov    QWORD PTR[rsp + 0x90], rax
49 8b 06                            // mov    rax, QWORD PTR[r14]
[push volatile]
48 83 ec 28                         // sub    rsp,0x28
4c 89 f1                            // mov    rcx,r14
48 b8 [targetAddress : 8]           // movabs rax, targetAddress
ff d0                               // call   rax
48 83 c4 28                         // add    rsp,0x28
[pop volatile]
50                                  // push   rax
48 b8 [returnAddress : 8]           // movabs rax, returnAddress
ff e0                               // jmp    rax
)";

const char* IslandConsumptionIteration =
R"(
-pattern:
0f 29 70 b8                         // movaps XMMWORD PTR [rax-0x48],xmm6
0f 29 78 a8                         // movaps XMMWORD PTR [rax-0x58],xmm7
44 0f 29 40 98                      // movaps XMMWORD PTR [rax-0x68],xmm8
44 0f 29 48 88                      // movaps XMMWORD PTR [rax-0x78],xmm9
44 0f 29 90 78 ff ff ff             // movaps XMMWORD PTR [rax-0x88],xmm10


-replacement:
48 bb [detourAddress : 8]           // movabs rbx, detourAddress
ff e3                               // jmp    rbx
returnLocation
90                                  // nop
58                                  // pop rax

-detour:
0f 29 70 b8                         // movaps XMMWORD PTR [rax-0x48],xmm6
0f 29 78 a8                         // movaps XMMWORD PTR [rax-0x58],xmm7
44 0f 29 40 98                      // movaps XMMWORD PTR [rax-0x68],xmm8
44 0f 29 48 88                      // movaps XMMWORD PTR [rax-0x78],xmm9
44 0f 29 90 78 ff ff ff             // movaps XMMWORD PTR [rax-0x88],xmm10
[push volatile]
48 83 ec 28                         // sub    rsp,0x28
48 b8 [targetAddress : 8]           // movabs rax, targetAddress
ff d0                               // call   rax
48 83 c4 28                         // add    rsp,0x28
[pop volatile]
50                                  // push   rax
48 b8 [returnAddress : 8]           // movabs rax, returnAddress
ff e0                               // jmp    rax
)";

}

