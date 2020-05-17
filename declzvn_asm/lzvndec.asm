

================ B E G I N N I N G   O F   P R O C E D U R E ================



                                       ; 
                                       ; Section .text
                                       ; 
                                       ; Range 0x401000 - 0x4012f0 (752 bytes)
                                       ; File offset 512 (1024 bytes)
                                       ; Flags : 0x60500020
                                       ; 
                     lzvn_decode:
0000000000401000         push       rbp
0000000000401001         mov        rbp, rsp
0000000000401004         push       rdi
0000000000401005         push       rsi
0000000000401006         mov        rdi, rcx
0000000000401009         mov        rsi, rdx
000000000040100c         mov        rdx, r8
000000000040100f         mov        rcx, r9
0000000000401012         push       rbx
0000000000401013         push       r12
0000000000401015         lea        rbx, qword [ds:lzvn_call]                   ; lzvn_call
000000000040101c         xor        rax, rax
000000000040101f         xor        r12, r12
0000000000401022         sub        rsi, 0x8
0000000000401026         jb         lzvn_exit2

000000000040102c         lea        rcx, qword [ds:rdx+rcx-0x8]
0000000000401031         cmp        rdx, rcx
0000000000401034         ja         lzvn_exit2

000000000040103a         movzx      r9, byte [ds:rdx]
000000000040103e         mov        r8, qword [ds:rdx]
0000000000401041         jmp        qword [ds:rbx+r9*8]
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table2:
0000000000401045         add        rdx, 0x1                                    ; XREF=0x402070, 0x4020b0
0000000000401049         cmp        rdx, rcx
000000000040104c         ja         lzvn_exit2

0000000000401052         movzx      r9, byte [ds:rdx]
0000000000401056         mov        r8, qword [ds:rdx]
0000000000401059         jmp        qword [ds:rbx+r9*8]
                        ; endp
000000000040105d         nop        qword [ds:rax]


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table0:
0000000000401060         shr        r9, 0x6                                     ; XREF=lzvn_call, 0x402008, 0x402010, 0x402018, 0x402020, 0x402028, 0x402040, 0x402048, 0x402050, 0x402058, 0x402060, …
0000000000401064         lea        rdx, qword [ds:rdx+r9+0x2]
0000000000401069         cmp        rdx, rcx
000000000040106c         ja         lzvn_exit2

0000000000401072         mov        r12, r8
0000000000401075         bswap      r12
0000000000401078         mov        r10, r12
000000000040107b         shl        r12, 0x5
000000000040107f         shl        r10, 0x2
0000000000401083         shr        r12, 0x35
0000000000401087         shr        r10, 0x3d
000000000040108b         shr        r8, 0x10
000000000040108f         add        r10, 0x3
                     lzvn_l1:
0000000000401093         lea        r11, qword [ds:rax+r9]                      ; XREF=lzvn_table3+40, lzvn_table1+54, lzvn_table4+69
0000000000401097         add        r11, r10
000000000040109a         cmp        r11, rsi
000000000040109d         jae        lzvn_l2

000000000040109f         mov        qword [ds:rdi+rax], r8
00000000004010a3         add        rax, r9
00000000004010a6         mov        r8, rax
00000000004010a9         sub        r8, r12
00000000004010ac         jb         lzvn_exit2

00000000004010b2         cmp        r12, 0x8
00000000004010b6         jb         lzvn_l6

                     lzvn_l5:
00000000004010b8         mov        r9, qword [ds:rdi+r8]                       ; XREF=lzvn_l5+20, lzvn_l7+23
00000000004010bc         add        r8, 0x8
00000000004010c0         mov        qword [ds:rdi+rax], r9
00000000004010c4         add        rax, 0x8
00000000004010c8         sub        r10, 0x8
00000000004010cc         ja         lzvn_l5

00000000004010ce         add        rax, r10
00000000004010d1         movzx      r9, byte [ds:rdx]
00000000004010d5         mov        r8, qword [ds:rdx]
00000000004010d8         jmp        qword [ds:rbx+r9*8]

                     lzvn_l2:
00000000004010dc         test       r9, r9                                      ; XREF=lzvn_l1+10
00000000004010df         je         lzvn_l3

00000000004010e1         lea        r11, qword [ds:rsi+0x8]

                     lzvn_l4:
00000000004010e5         mov        byte [ds:rdi+rax], r8b                      ; XREF=lzvn_l4+25
00000000004010e9         add        rax, 0x1
00000000004010ed         cmp        r11, rax
00000000004010f0         je         lzvn_exit1

00000000004010f6         shr        r8, 0x8
00000000004010fa         sub        r9, 0x1
00000000004010fe         jne        lzvn_l4

                     lzvn_l3:
0000000000401100         mov        r8, rax                                     ; XREF=lzvn_l2+3
0000000000401103         sub        r8, r12
0000000000401106         jb         lzvn_exit2

                     lzvn_l6:
000000000040110c         lea        r11, qword [ds:rsi+0x8]                     ; XREF=lzvn_l1+35, lzvn_l7+13, lzvn_l7+29

                     lzvn_l0:
0000000000401110         movzx      r9, byte [ds:rdi+r8]                        ; XREF=lzvn_l0+30
0000000000401115         add        r8, 0x1
0000000000401119         mov        byte [ds:rdi+rax], r9b
000000000040111d         add        rax, 0x1
0000000000401121         cmp        r11, rax
0000000000401124         je         lzvn_exit1

000000000040112a         sub        r10, 0x1
000000000040112e         jne        lzvn_l0

0000000000401130         movzx      r9, byte [ds:rdx]
0000000000401134         mov        r8, qword [ds:rdx]
0000000000401137         jmp        qword [ds:rbx+r9*8]
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table3:
000000000040113b         shr        r9, 0x6                                     ; XREF=0x402230, 0x402270, 0x4022b0, 0x4022f0, 0x402330, 0x402370, 0x402430, 0x402470, 0x4024b0, 0x4024f0, 0x402630, …
000000000040113f         lea        rdx, qword [ds:rdx+r9+0x1]
0000000000401144         cmp        rdx, rcx
0000000000401147         ja         lzvn_exit2

000000000040114d         mov        r10, 0x38
0000000000401154         and        r10, r8
0000000000401157         shr        r8, 0x8
000000000040115b         shr        r10, 0x3
000000000040115f         add        r10, 0x3
0000000000401163         jmp        lzvn_l1
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table1:
0000000000401168         shr        r9, 0x6                                     ; XREF=0x402038, 0x402078, 0x4020b8, 0x4020f8, 0x402138, 0x402178, 0x4021b8, 0x4021f8, 0x402238, 0x402278, 0x4022b8, …
000000000040116c         lea        rdx, qword [ds:rdx+r9+0x3]
0000000000401171         cmp        rdx, rcx
0000000000401174         ja         lzvn_exit2

000000000040117a         mov        r10, 0x38
0000000000401181         mov        r12, 0xffff
0000000000401188         and        r10, r8
000000000040118b         shr        r8, 0x8
000000000040118f         shr        r10, 0x3
0000000000401193         and        r12, r8
0000000000401196         shr        r8, 0x10
000000000040119a         add        r10, 0x3
000000000040119e         jmp        lzvn_l1
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table4:
00000000004011a3         shr        r9, 0x3                                     ; XREF=0x402500, 0x402508, 0x402510, 0x402518, 0x402520, 0x402528, 0x402530, 0x402538, 0x402540, 0x402548, 0x402550, …
00000000004011a7         and        r9, 0x3
00000000004011ab         lea        rdx, qword [ds:rdx+r9+0x3]
00000000004011b0         cmp        rdx, rcx
00000000004011b3         ja         lzvn_exit2

00000000004011b9         mov        r10, r8
00000000004011bc         and        r10, 0x307
00000000004011c3         shr        r8, 0xa
00000000004011c7         movzx      r12, r10b
00000000004011cb         shr        r10, 0x8
00000000004011cf         shl        r12, 0x2
00000000004011d3         or         r10, r12
00000000004011d6         mov        r12, 0x3fff
00000000004011dd         add        r10, 0x3
00000000004011e1         and        r12, r8
00000000004011e4         shr        r8, 0xe
00000000004011e8         jmp        lzvn_l1
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table8:
00000000004011ed         add        rdx, 0x1                                    ; XREF=0x402788, 0x402790, 0x402798, 0x4027a0, 0x4027a8, 0x4027b0, 0x4027b8, 0x4027c0, 0x4027c8, 0x4027d0, 0x4027d8, …
00000000004011f1         cmp        rdx, rcx
00000000004011f4         ja         lzvn_exit2

00000000004011fa         mov        r10, r8
00000000004011fd         and        r10, 0xf
0000000000401201         jmp        lzvn_l7
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table7:
0000000000401203         add        rdx, 0x2                                    ; XREF=0x402780
0000000000401207         cmp        rdx, rcx
000000000040120a         ja         lzvn_exit2

0000000000401210         mov        r10, r8
0000000000401213         shr        r10, 0x8
0000000000401217         and        r10, 0xff
000000000040121e         add        r10, 0x10
                     lzvn_l7:
0000000000401222         mov        r8, rax                                     ; XREF=lzvn_table8+20
0000000000401225         sub        r8, r12
0000000000401228         lea        r11, qword [ds:rax+r10]
000000000040122c         cmp        r11, rsi
000000000040122f         jae        lzvn_l6

0000000000401235         cmp        r12, 0x8
0000000000401239         jae        lzvn_l5

000000000040123f         jmp        lzvn_l6
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table6:
0000000000401244         and        r8, 0xf                                     ; XREF=0x402708, 0x402710, 0x402718, 0x402720, 0x402728, 0x402730, 0x402738, 0x402740, 0x402748, 0x402750, 0x402758, …
0000000000401248         lea        rdx, qword [ds:rdx+r8+0x1]
000000000040124d         jmp        lzvn_l8
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_table5:
000000000040124f         shr        r8, 0x8                                     ; XREF=0x402700
0000000000401253         and        r8, 0xff
000000000040125a         add        r8, 0x10
000000000040125e         lea        rdx, qword [ds:rdx+r8+0x2]
                     lzvn_l8:
0000000000401263         cmp        rdx, rcx                                    ; XREF=lzvn_table6+9
0000000000401266         ja         lzvn_exit2

0000000000401268         lea        r11, qword [ds:rax+r8]
000000000040126c         neg        r8
000000000040126f         cmp        r11, rsi
0000000000401272         ja         lzvn_l9

0000000000401274         lea        r11, qword [ds:rdi+r11]

0000000000401278         mov        r9, qword [ds:rdx+r8]                       ; XREF=lzvn_l8+33
000000000040127c         mov        qword [ds:r11+r8], r9
0000000000401280         add        r8, 0x8
0000000000401284         jae        0x401278

0000000000401286         mov        rax, r11
0000000000401289         sub        rax, rdi
000000000040128c         movzx      r9, byte [ds:rdx]
0000000000401290         mov        r8, qword [ds:rdx]
0000000000401293         jmp        qword [ds:rbx+r9*8]

                     lzvn_l9:
0000000000401297         lea        r11, qword [ds:rsi+0x8]                     ; XREF=lzvn_l8+15

                     lzvn_l10:
000000000040129b         movzx      r9, byte [ds:rdx+r8]                        ; XREF=lzvn_l10+22
00000000004012a0         mov        byte [ds:rdi+rax], r9b
00000000004012a4         add        rax, 0x1
00000000004012a8         cmp        r11, rax
00000000004012ab         je         lzvn_exit1

00000000004012ad         add        r8, 0x1
00000000004012b1         jne        lzvn_l10

00000000004012b3         movzx      r9, byte [ds:rdx]
00000000004012b7         mov        r8, qword [ds:rdx]
00000000004012ba         jmp        qword [ds:rbx+r9*8]
                        ; endp


================ B E G I N N I N G   O F   P R O C E D U R E ================



                     lzvn_exit2:
00000000004012be         xor        rax, rax                                    ; XREF=lzvn_decode+38, lzvn_decode+52, lzvn_table2+7, lzvn_table0+12, lzvn_l1+25, lzvn_l3+6, lzvn_table3+12, lzvn_table1+12, lzvn_table4+16, lzvn_table8+7, lzvn_table7+7, …
                     lzvn_exit1:
00000000004012c1         pop        r12                                         ; XREF=lzvn_l4+11, lzvn_l0+20, lzvn_l10+16, 0x402030
00000000004012c3         pop        rbx
00000000004012c4         pop        rsi
00000000004012c5         pop        rdi
00000000004012c6         pop        rbp
00000000004012c7         ret        
                        ; endp
00000000004012c8         nop        
00000000004012c9         nop        
00000000004012ca         nop        
00000000004012cb         nop        
00000000004012cc         nop        
00000000004012cd         nop        
00000000004012ce         nop        
00000000004012cf         nop        
00000000004012d0         dq         0xffffffffffffffff
00000000004012d8         dq         0x0000000000000000
00000000004012e0         dq         0xffffffffffffffff
00000000004012e8         dq         0x0000000000000000
                                       ; 
                                       ; Section .data
                                       ; 
                                       ; Range 0x402000 - 0x402800 (2048 bytes)
                                       ; File offset 1536 (2048 bytes)
                                       ; Flags : 0xc0500040
                                       ; 
                     lzvn_call:
0000000000402000         dq         lzvn_table0                                 ; XREF=lzvn_decode+21
0000000000402008         dq         lzvn_table0
0000000000402010         dq         lzvn_table0
0000000000402018         dq         lzvn_table0
0000000000402020         dq         lzvn_table0
0000000000402028         dq         lzvn_table0
0000000000402030         dq         lzvn_exit1
0000000000402038         dq         lzvn_table1
0000000000402040         dq         lzvn_table0
0000000000402048         dq         lzvn_table0
0000000000402050         dq         lzvn_table0
0000000000402058         dq         lzvn_table0
0000000000402060         dq         lzvn_table0
0000000000402068         dq         lzvn_table0
0000000000402070         dq         lzvn_table2
0000000000402078         dq         lzvn_table1
0000000000402080         dq         lzvn_table0
0000000000402088         dq         lzvn_table0
0000000000402090         dq         lzvn_table0
0000000000402098         dq         lzvn_table0
00000000004020a0         dq         lzvn_table0
00000000004020a8         dq         lzvn_table0
00000000004020b0         dq         lzvn_table2
00000000004020b8         dq         lzvn_table1
00000000004020c0         dq         lzvn_table0
00000000004020c8         dq         lzvn_table0
00000000004020d0         dq         lzvn_table0
00000000004020d8         dq         lzvn_table0
00000000004020e0         dq         lzvn_table0
00000000004020e8         dq         lzvn_table0
00000000004020f0         dq         lzvn_exit2
00000000004020f8         dq         lzvn_table1
0000000000402100         dq         lzvn_table0
0000000000402108         dq         lzvn_table0
0000000000402110         dq         lzvn_table0
0000000000402118         dq         lzvn_table0
0000000000402120         dq         lzvn_table0
0000000000402128         dq         lzvn_table0
0000000000402130         dq         lzvn_exit2
0000000000402138         dq         lzvn_table1
0000000000402140         dq         lzvn_table0
0000000000402148         dq         lzvn_table0
0000000000402150         dq         lzvn_table0
0000000000402158         dq         lzvn_table0
0000000000402160         dq         lzvn_table0
0000000000402168         dq         lzvn_table0
0000000000402170         dq         lzvn_exit2
0000000000402178         dq         lzvn_table1
0000000000402180         dq         lzvn_table0
0000000000402188         dq         lzvn_table0
0000000000402190         dq         lzvn_table0
0000000000402198         dq         lzvn_table0
00000000004021a0         dq         lzvn_table0
00000000004021a8         dq         lzvn_table0
00000000004021b0         dq         lzvn_exit2
00000000004021b8         dq         lzvn_table1
00000000004021c0         dq         lzvn_table0
00000000004021c8         dq         lzvn_table0
00000000004021d0         dq         lzvn_table0
00000000004021d8         dq         lzvn_table0
00000000004021e0         dq         lzvn_table0
00000000004021e8         dq         lzvn_table0
00000000004021f0         dq         lzvn_exit2
00000000004021f8         dq         lzvn_table1
0000000000402200         dq         lzvn_table0
0000000000402208         dq         lzvn_table0
0000000000402210         dq         lzvn_table0
0000000000402218         dq         lzvn_table0
0000000000402220         dq         lzvn_table0
0000000000402228         dq         lzvn_table0
0000000000402230         dq         lzvn_table3
0000000000402238         dq         lzvn_table1
0000000000402240         dq         lzvn_table0
0000000000402248         dq         lzvn_table0
0000000000402250         dq         lzvn_table0
0000000000402258         dq         lzvn_table0
0000000000402260         dq         lzvn_table0
0000000000402268         dq         lzvn_table0
0000000000402270         dq         lzvn_table3
0000000000402278         dq         lzvn_table1
0000000000402280         dq         lzvn_table0
0000000000402288         dq         lzvn_table0
0000000000402290         dq         lzvn_table0
0000000000402298         dq         lzvn_table0
00000000004022a0         dq         lzvn_table0
00000000004022a8         dq         lzvn_table0
00000000004022b0         dq         lzvn_table3
00000000004022b8         dq         lzvn_table1
00000000004022c0         dq         lzvn_table0
00000000004022c8         dq         lzvn_table0
00000000004022d0         dq         lzvn_table0
00000000004022d8         dq         lzvn_table0
00000000004022e0         dq         lzvn_table0
00000000004022e8         dq         lzvn_table0
00000000004022f0         dq         lzvn_table3
00000000004022f8         dq         lzvn_table1
0000000000402300         dq         lzvn_table0
0000000000402308         dq         lzvn_table0
0000000000402310         dq         lzvn_table0
0000000000402318         dq         lzvn_table0
0000000000402320         dq         lzvn_table0
0000000000402328         dq         lzvn_table0
0000000000402330         dq         lzvn_table3
0000000000402338         dq         lzvn_table1
0000000000402340         dq         lzvn_table0
0000000000402348         dq         lzvn_table0
0000000000402350         dq         lzvn_table0
0000000000402358         dq         lzvn_table0
0000000000402360         dq         lzvn_table0
0000000000402368         dq         lzvn_table0
0000000000402370         dq         lzvn_table3
0000000000402378         dq         lzvn_table1
0000000000402380         dq         lzvn_exit2
0000000000402388         dq         lzvn_exit2
0000000000402390         dq         lzvn_exit2
0000000000402398         dq         lzvn_exit2
00000000004023a0         dq         lzvn_exit2
00000000004023a8         dq         lzvn_exit2
00000000004023b0         dq         lzvn_exit2
00000000004023b8         dq         lzvn_exit2
00000000004023c0         dq         lzvn_exit2
00000000004023c8         dq         lzvn_exit2
00000000004023d0         dq         lzvn_exit2
00000000004023d8         dq         lzvn_exit2
00000000004023e0         dq         lzvn_exit2
00000000004023e8         dq         lzvn_exit2
00000000004023f0         dq         lzvn_exit2
00000000004023f8         dq         lzvn_exit2
0000000000402400         dq         lzvn_table0
0000000000402408         dq         lzvn_table0
0000000000402410         dq         lzvn_table0
0000000000402418         dq         lzvn_table0
0000000000402420         dq         lzvn_table0
0000000000402428         dq         lzvn_table0
0000000000402430         dq         lzvn_table3
0000000000402438         dq         lzvn_table1
0000000000402440         dq         lzvn_table0
0000000000402448         dq         lzvn_table0
0000000000402450         dq         lzvn_table0
0000000000402458         dq         lzvn_table0
0000000000402460         dq         lzvn_table0
0000000000402468         dq         lzvn_table0
0000000000402470         dq         lzvn_table3
0000000000402478         dq         lzvn_table1
0000000000402480         dq         lzvn_table0
0000000000402488         dq         lzvn_table0
0000000000402490         dq         lzvn_table0
0000000000402498         dq         lzvn_table0
00000000004024a0         dq         lzvn_table0
00000000004024a8         dq         lzvn_table0
00000000004024b0         dq         lzvn_table3
00000000004024b8         dq         lzvn_table1
00000000004024c0         dq         lzvn_table0
00000000004024c8         dq         lzvn_table0
00000000004024d0         dq         lzvn_table0
00000000004024d8         dq         lzvn_table0
00000000004024e0         dq         lzvn_table0
00000000004024e8         dq         lzvn_table0
00000000004024f0         dq         lzvn_table3
00000000004024f8         dq         lzvn_table1
0000000000402500         dq         lzvn_table4
0000000000402508         dq         lzvn_table4
0000000000402510         dq         lzvn_table4
0000000000402518         dq         lzvn_table4
0000000000402520         dq         lzvn_table4
0000000000402528         dq         lzvn_table4
0000000000402530         dq         lzvn_table4
0000000000402538         dq         lzvn_table4
0000000000402540         dq         lzvn_table4
0000000000402548         dq         lzvn_table4
0000000000402550         dq         lzvn_table4
0000000000402558         dq         lzvn_table4
0000000000402560         dq         lzvn_table4
0000000000402568         dq         lzvn_table4
0000000000402570         dq         lzvn_table4
0000000000402578         dq         lzvn_table4
0000000000402580         dq         lzvn_table4
0000000000402588         dq         lzvn_table4
0000000000402590         dq         lzvn_table4
0000000000402598         dq         lzvn_table4
00000000004025a0         dq         lzvn_table4
00000000004025a8         dq         lzvn_table4
00000000004025b0         dq         lzvn_table4
00000000004025b8         dq         lzvn_table4
00000000004025c0         dq         lzvn_table4
00000000004025c8         dq         lzvn_table4
00000000004025d0         dq         lzvn_table4
00000000004025d8         dq         lzvn_table4
00000000004025e0         dq         lzvn_table4
00000000004025e8         dq         lzvn_table4
00000000004025f0         dq         lzvn_table4
00000000004025f8         dq         lzvn_table4
0000000000402600         dq         lzvn_table0
0000000000402608         dq         lzvn_table0
0000000000402610         dq         lzvn_table0
0000000000402618         dq         lzvn_table0
0000000000402620         dq         lzvn_table0
0000000000402628         dq         lzvn_table0
0000000000402630         dq         lzvn_table3
0000000000402638         dq         lzvn_table1
0000000000402640         dq         lzvn_table0
0000000000402648         dq         lzvn_table0
0000000000402650         dq         lzvn_table0
0000000000402658         dq         lzvn_table0
0000000000402660         dq         lzvn_table0
0000000000402668         dq         lzvn_table0
0000000000402670         dq         lzvn_table3
0000000000402678         dq         lzvn_table1
0000000000402680         dq         lzvn_exit2
0000000000402688         dq         lzvn_exit2
0000000000402690         dq         lzvn_exit2
0000000000402698         dq         lzvn_exit2
00000000004026a0         dq         lzvn_exit2
00000000004026a8         dq         lzvn_exit2
00000000004026b0         dq         lzvn_exit2
00000000004026b8         dq         lzvn_exit2
00000000004026c0         dq         lzvn_exit2
00000000004026c8         dq         lzvn_exit2
00000000004026d0         dq         lzvn_exit2
00000000004026d8         dq         lzvn_exit2
00000000004026e0         dq         lzvn_exit2
00000000004026e8         dq         lzvn_exit2
00000000004026f0         dq         lzvn_exit2
00000000004026f8         dq         lzvn_exit2
0000000000402700         dq         lzvn_table5
0000000000402708         dq         lzvn_table6
0000000000402710         dq         lzvn_table6
0000000000402718         dq         lzvn_table6
0000000000402720         dq         lzvn_table6
0000000000402728         dq         lzvn_table6
0000000000402730         dq         lzvn_table6
0000000000402738         dq         lzvn_table6
0000000000402740         dq         lzvn_table6
0000000000402748         dq         lzvn_table6
0000000000402750         dq         lzvn_table6
0000000000402758         dq         lzvn_table6
0000000000402760         dq         lzvn_table6
0000000000402768         dq         lzvn_table6
0000000000402770         dq         lzvn_table6
0000000000402778         dq         lzvn_table6
0000000000402780         dq         lzvn_table7
0000000000402788         dq         lzvn_table8
0000000000402790         dq         lzvn_table8
0000000000402798         dq         lzvn_table8
00000000004027a0         dq         lzvn_table8
00000000004027a8         dq         lzvn_table8
00000000004027b0         dq         lzvn_table8
00000000004027b8         dq         lzvn_table8
00000000004027c0         dq         lzvn_table8
00000000004027c8         dq         lzvn_table8
00000000004027d0         dq         lzvn_table8
00000000004027d8         dq         lzvn_table8
00000000004027e0         dq         lzvn_table8
00000000004027e8         dq         lzvn_table8
00000000004027f0         dq         lzvn_table8
00000000004027f8         dq         lzvn_table8
                                       ; 
                                       ; Section .idata
                                       ; 
                                       ; Range 0x403000 - 0x403014 (20 bytes)
                                       ; File offset 3584 (512 bytes)
                                       ; Flags : 0xc0300040
                                       ; 
0000000000403000         dd         0x00000000
0000000000403004         dd         0x00000000
0000000000403008         dd         0x00000000
000000000040300c         dd         0x00000000
0000000000403010         dd         0x00000000
