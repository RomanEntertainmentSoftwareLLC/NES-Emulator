;n6502 cpu emulator
;copyright 1997 Bloodlust Software
;free for public use


;MSVC equ 1

                .386
      locals
      .model flat,C

      .data

     PUBLIC ncpu

F_CARRY = 01h
F_ZERO  = 02h
F_INT   = 04h
F_DEC   = 08h
F_BREAK = 10h
F_RSRVD = 20h
F_OVER  = 40h
F_SIGN  = 80h


;------------------------------------
;degenerate flagtable

i=0
flagtable label dword
rept 100h
  db (i AND F_ZERO) XOR F_ZERO
  db ((i AND F_OVER) SHR 6)
  db (i AND F_CARRY)
  db (i AND F_SIGN)
 i=i+1
endm


;------------------------------------
;cpu struc
n6502cpu struc
        rA  db ?
        rS  db ?
        rX  db ?
        rY  db ?

        rF  db ?  ;flags
        rPC dw ?  ;PC
 trapbadops db ?

      db ?
      db ?
 degenflags LABEL DWORD
  zero    db  ? ;degenerate flags
  over    db  ?

  carry   db  ? ;(keep zero & sign in diff dwords)
  sign    db  ?
      db ?
      db ?

 cycles      dd ?
 totalcycles dd ?

 PCBASE dd ?
 ROM    dd   ?  ;pointer to ROM at 8000-FFFF
 RAM    dd   ? ;pointer to RAM at 0-7FF

 readtrap  dd ?
 writetrap dd ?

 breakpoint dd ?

   ends


;main cpu
align
ncpu n6502cpu ?


      .code

       PUBLIC n6502_execute
       PUBLIC n6502_int,n6502_nmi

;------------------------------------
;fetch macros

fetchbyte macro reg
     mov     reg,byte ptr [esi]
     inc  esi
     endm

fetchbytesx macro reg
     movsx   reg,byte ptr [esi]
     inc  esi
     endm

fetchbytezx macro reg
     movzx   reg,byte ptr [esi]
     inc  esi
     endm

fetchword macro reg
     mov     reg,word ptr [esi]
     add  esi,2
     endm

npushb macro reg
   mov [ebp+edx+100h],reg
   dec dl
  endm

npopb macro reg
   inc dl
   mov reg,[ebp+edx+100h]
  endm


;-----------------------------
; PC nat/denat
; these must be customized

;converts 16-bit PC to a 32-bit base+PC in ESI
naturalizeESI proc
   cmp esi,8000h ;upper rom?
   jb  @@NOTROM
   mov [ncpu.PCBASE],edi
   add esi,edi
   ret

@@NOTROM:
   cmp esi,2000h ;mirrored ram?
   jae @@NOTRAM
   and esi,7FFh  ;mirror lower 2k
@@NOTRAM:
   mov [ncpu.PCBASE],ebp
   add esi,ebp
   ret
  endp

;converts 32-bit base+PC to 16-bit PC
denaturalizeESI macro
   sub esi,[ncpu.PCBASE]
  endm


;--------------------------------
; read/write memory functions
; these must be customized

readbyte proc
   cmp edx,8000h
   jb  @@NOTROM
   mov al,[edi+edx]
   ret
@@NOTROM:
   cmp edx,2000h ;mirrored ram?
   jae @@NOTRAM
   and edx,7FFh  ;mirror lower 2k
   mov al,[ebp+edx]
   ret

@@NOTRAM:
   push ebx
   push ecx
   push edx
   push esi
   push edi
  ifdef MSVC
   mov ecx,edx
  else
   mov eax,edx
  endif

   call [ncpu.readtrap]
   pop  edi
   pop  esi
   pop  edx
   pop  ecx
   pop  ebx
   and  eax,0FFh
   ret
        endp

writebyte proc
   cmp edx,2000h
   jae @@NOTRAM
   and edx,7FFh  ;mirror lower 2k
   mov [ebp+edx],al
   ret

@@NOTRAM:
   push ebx
   push ecx
   push edx
   push esi
   push edi

  ifdef MSVC
   mov ecx,eax
  endif

   call [ncpu.writetrap]
   pop  edi
   pop  esi

   test eax,eax
   jz   @@NOBANK
   denaturalizeESI    ;reset ESI
   mov  edi,[ncpu.ROM]
   call naturalizeESI
@@NOBANK:

   pop  edx
   pop  ecx
   pop  ebx
   xor  eax,eax
   ret
  endp




;----------------------------
; flag store macros

;saves zero,sign flags
savef macro reg
   mov [ncpu.zero],reg
   mov [ncpu.sign],reg
  endm

;saves zero,sign,carry flags
savefc macro reg
   setc [ncpu.carry]
   mov [ncpu.zero],reg
   mov [ncpu.sign],reg
  endm

;saves zero,sign,overflow flags
savefo macro reg
   seto [ncpu.over]
   mov [ncpu.zero],reg
   mov [ncpu.sign],reg
  endm

;saves zero,sign,carry,overflow flags
savefco macro reg
   setc [ncpu.carry]
   seto [ncpu.over]
   mov [ncpu.zero],reg
   mov [ncpu.sign],reg
  endm

;gets carry flag (Assumes ah=0)
getc macro
     cmp ah,[ncpu.carry]
    endm


;-------------------------------------
; flag conversion macros

;al -> lastval,carry,over
degenerateflags macro
   mov [ncpu.rF],al ;store full flags
   mov eax,[flagtable+eax*4]
   mov dword ptr [ncpu.degenflags],eax
   xor eax,eax
  endm

;lastval,carry,over -> al
generateflags macro
   mov al,[ncpu.rF] ;al=RSRVD|DEC|INT|BREAK
   push edx

   and al,3Ch
   mov dl,[ncpu.over]

   cmp   ah,[ncpu.zero]
   jne  @@NOTZERO
   or   al,F_ZERO  ;set zero flag
@@NOTZERO:

   mov  ah,[ncpu.sign]
   and  dl,1

   shl  dl,6
   and  ah,F_SIGN

   or   al,[ncpu.carry]  ;carry=bit 0
   or   dl,ah

   or   al,dl            ;over=bit 7
   pop  edx

   xor ah,ah
  endm


;-------------------
; execute function

n6502_execute proc
      push ebp
      push esi
      push edi
      push ebx

      xor   eax,eax
      xor   esi,esi
      mov   edi,[ncpu.ROM]
      mov   ebp,[ncpu.RAM]
      mov   bx,word ptr [ncpu.rA]
      mov   cx,word ptr [ncpu.rX]
      mov   si,[ncpu.rPC]

      mov   al,[ncpu.rF] ;get flags
      degenerateflags

      call  naturalizeESI

      xor   edx,edx
      xor   eax,eax

executeloop:
      sub   [ncpu.cycles],eax ;3
      jle   done

      mov   al,[esi]          ;1
      inc   esi

      xor   edx,edx  ;agi

      jmp   [n6502jmptable+eax*4] ;5


;      mov   edx,[ncpu.cycles] ;1
;      sub   edx,eax          ;1
;      mov   al,[esi]
;      mov   [ncpu.cycles],edx ;1
;      jle   done
;      xor   edx,edx           ;1
;      inc   esi
;      jmp  [n6502jmptable+eax*4] ;5


badopcode:
      mov   eax,4               ;nop cycles
      cmp   [ncpu.trapbadops],0 ;continue anyway?
      jne   executeloop

      dec   esi ;decrease PC ;abort
      mov   eax,-1
      jmp   short leave

done: xor   eax,eax

leave:
      sub   esi,[ncpu.PCBASE]
      mov   word ptr [ncpu.rA],bx
      mov   word ptr [ncpu.rX],cx
      mov   [ncpu.rPC],si ;store PC

      push eax
      xor  eax,eax
      generateflags    ;set flags
      mov  [ncpu.rF],al
      pop  eax

      pop ebx
      pop edi
      pop esi
      pop ebp
      ret
     endp


;-----------------------------------
; interrupts


n6502_nmi proc
   push ebp
   push edx
   push edi

   xor edx,edx
   xor eax,eax

   mov ebp,[ncpu.RAM]
   mov edi,[ncpu.ROM]

   mov ax,[ncpu.rPC] ;get pc
   mov dl,[ncpu.rS] ;get stack

   ;push pc
   npushb ah
   npushb al

   ;push flags
   mov al,[ncpu.rF]
   and al,NOT F_BREAK ;remove break flag ?
   npushb al

   ;store S
   mov [ncpu.rS],dl

   ;turn on interrupt flag
   or  al,F_INT
   mov [ncpu.rF],al

   ;PC=NMI vector
   mov ax,[edi+0FFFAh]
   mov [ncpu.rPC],ax

   pop edi
   pop edx
   pop ebp
   ret
  endp



n6502_int proc
   test [ncpu.rF],F_INT
   jnz  @@DONE

   push ebp
   push edx
   push edi

   xor edx,edx
   xor eax,eax

   mov ebp,[ncpu.RAM]
   mov edi,[ncpu.RAM]
   mov dl,[ncpu.rS] ;get stack
   mov ax,[ncpu.rPC] ;get pc

   ;push pc
   npushb ah
   npushb al

   ;push flags
   mov al,[ncpu.rF]
   and al,NOT F_BREAK ;remove break flag ?
   npushb al

   ;store S
   mov [ncpu.rS],dl

   or  al,F_INT ;turn on interrupt flag
   mov [ncpu.rF],al

   ;PC=INT vector
   mov edi,[ncpu.ROM]
   mov ax,[edi+0FFFEh]
   mov [ncpu.rPC],ax

   pop edi
   pop edx
   pop ebp
@@DONE:
   ret
  endp







;--------------------------------------
; opcode header/footer macros

opbegin macro O
  align
  op&O proc
 endm


opend macro C
   mov al,C ;store cycles
   jmp executeloop
  endp
 endm

opendzx macro C
   mov eax,C ;store cycles
   jmp executeloop
  endp
 endm

;---------------------------

nX equ cl
nY equ ch
nA equ bl
nS equ bh


;---------------------------------
; lea macros

;$XX
lea_zero macro
    fetchbyte dl
        endm

;$XX,x
lea_zeroX   macro
    fetchbyte dl
    add dl,nX
        endm

;$XX,y
lea_zeroY   macro
    fetchbyte dl
    add dl,nY
        endm

;$XXXX
lea_abs macro
    fetchword dx
        endm

;$XXXX,x
lea_absX   macro
    fetchword dx
    add  dl,nX
    adc  dh,ah ;ah=0
        endm

;$XXXX,y
lea_absY   macro
    fetchword dx
    add  dl,nY
    adc  dh,ah ;ah=0
        endm


;($XX,x)
lea_indX   macro
    fetchbyte dl
    add dl,nX
    mov dx,[ebp+edx]
        endm

;($XX),y
lea_indY   macro
    fetchbyte dl
    mov  dx,[ebp+edx]
    add  dl,nY
    adc  dh,ah  ;ah=0
        endm


;--------------------------------------
; read macros


;$XX
Rzero macro
    lea_zero
    mov al,[ebp+edx]
   endm

;$XX,x
RzeroX  macro
    lea_zeroX
    mov al,[ebp+edx]
   endm

;$XX,y
RzeroY   macro
    lea_zeroY
    mov al,[ebp+edx]
   endm

;$XXXX
Rabs macro
    lea_abs
    call readbyte
   endm

;$XXXX,x
RabsX   macro
    lea_absX
    call readbyte
   endm

;$XXXX,y
RabsY   macro
    lea_absY
    call readbyte
   endm


;($XX,x)
RindX   macro
    lea_indX
    call readbyte
   endm

;($XX),y
RindY   macro
    lea_indY
    call readbyte
   endm


;#$XX
Rimmb macro
    fetchbyte al
   endm



;-------------------------------------------
; write macros

;$XX
Wzero macro
    mov [ebp+edx],al
   endm

;$XX,x
WzeroX  macro
    mov [ebp+edx],al
   endm

;$XX,y
WzeroY   macro
    mov [ebp+edx],al
   endm

;$XXXX
Wabs macro
    call writebyte
   endm

;$XXXX,x
WabsX   macro
    call writebyte
   endm

;$XXXX,y
WabsY   macro
    call writebyte
   endm

;($XX,x)
WindX   macro
    call writebyte
   endm

;($XX),y
WindY   macro
    call writebyte
   endm


;------------------------------------
;opcodes

;NOP
opbegin %0EAh
opend 2


;----------------
; bit/logic/arithmetic ops


;nA=nA+al+carry (decimal)
DECADC proc
   getc
   adc al,nA
   daa         ;cheap, probably incorrect
   savefco al
   mov nA,al
  opend 4

;nA=nA-al+carry (decimal)
DECSBC proc
   getc
   cmc
   sbb nA,al
   mov al,nA
   das         ;cheap, probably incorrect
   cmc
   savefco al
   mov nA,al
  opend 4

ADC_op macro amode,op,cy
  opbegin op
   R&amode
   test [ncpu.rF],F_DEC
   jnz  DECADC
   getc
   adc nA,al
   savefco nA
  opend cy
 endm

SBC_op macro amode,op,cy
  local carry
  opbegin op
   R&amode
   test [ncpu.rF],F_DEC
   jnz  DECSBC
   xor  al,0FFh
   getc
   adc nA,al
   savefco nA
  opend cy
 endm

ADC_op  immb,%69h,2
ADC_op  zero,%65h,3
ADC_op zeroX,%75h,4
ADC_op   abs,%6Dh,4
ADC_op  absX,%7Dh,4
ADC_op  absY,%79h,4
ADC_op  indX,%61h,6
ADC_op  indY,%71h,5

SBC_op  immb,%0E9h,2
SBC_op  zero,%0E5h,2
SBC_op zeroX,%0F5h,4
SBC_op   abs,%0EDh,4
SBC_op  absX,%0FDh,4
SBC_op  absY,%0F9h,4
SBC_op  indX,%0E1h,6
SBC_op  indY,%0F1h,5


LOG_op  macro operator,amode,op,cy
  opbegin op
    R&amode
    operator nA,al
    savef nA
  opend cy
 ENDM

;AND
LOG_op and, immb,%29h,2
LOG_op and, zero,%25h,3
LOG_op and,zeroX,%35h,4
LOG_op and,  abs,%2Dh,4
LOG_op and, absX,%3Dh,4
LOG_op and, absY,%39h,4
LOG_op and, indX,%21h,6
LOG_op and, indY,%31h,5

;OR
LOG_op  or, immb,%09h,2
LOG_op  or, zero,%05h,3
LOG_op  or,zeroX,%15h,4
LOG_op  or,  abs,%0Dh,4
LOG_op  or, absX,%1Dh,4
LOG_op  or, absY,%19h,4
LOG_op  or, indX,%01h,6
LOG_op  or, indY,%11h,5

;EOR
LOG_op xor, immb,%49h,2
LOG_op xor, zero,%45h,3
LOG_op xor,zeroX,%55h,4
LOG_op xor,  abs,%4Dh,4
LOG_op xor, absX,%5Dh,4
LOG_op xor, absY,%59h,4
LOG_op xor, indX,%41h,6
LOG_op xor, indY,%51h,5



BIT_op  macro amode,op,cy
  opbegin op
    R&amode
    mov   dl,al

    shr   al,6
    mov   [ncpu.sign],dl ;sign=mem

    and   dl,nA     ;acc & mem
    and   al,1      ;isolate bit 40h

    mov   [ncpu.zero],dl ;set zero
    mov   [ncpu.over],al
  opend cy
 ENDM

;BIT
BIT_op   zero,%24h,3
BIT_op    abs,%2Ch,4


;------------
;load/store


LDx_OP  macro reg,amode,op,cy
  opbegin op
    R&amode
    mov reg,al
    savef al
  opend cy
 ENDM

;LDA a
LDx_OP nA, immb,%0A9h,2
LDx_OP nA, zero,%0A5h,3
LDx_OP nA,zeroX,%0B5h,4
LDx_OP nA,  abs,%0ADh,4
LDx_OP nA, absX,%0BDh,4
LDx_OP nA, absY,%0B9h,4
LDx_OP nA, indX,%0A1h,6
LDx_OP nA, indY,%0B1h,5

;LDX a
LDx_OP nX, immb,%0A2h,2
LDx_OP nX, zero,%0A6h,3
LDx_OP nX,zeroY,%0B6h,4
LDx_OP nX,  abs,%0AEh,4
LDx_OP nX, absY,%0BEh,4

;LDY a
LDx_OP nY, immb,%0A0h,2
LDx_OP nY, zero,%0A4h,3
LDx_OP nY,zeroX,%0B4h,4
LDx_OP nY,  abs,%0ACh,4
LDx_OP nY, absX,%0BCh,4


STx_OP  macro reg,amode,op,cy
  opbegin op
    lea_&amode
    mov  al,reg
    W&amode
  opend cy
 ENDM


;STA a
STx_OP nA, zero,%085h,3
STx_OP nA,zeroX,%095h,4
STx_OP nA,  abs,%08Dh,4
STx_OP nA, absX,%09Dh,5
STx_OP nA, absY,%099h,5
STx_OP nA, indX,%081h,6
STx_OP nA, indY,%091h,6

;STX a
STx_OP nX, zero,%086h,3
STx_OP nX,zeroY,%096h,4
STx_OP nX, abs ,%08Eh,4

;STY a
STx_OP nY, zero,%084h,3
STx_OP nY,zeroX,%094h,4
STx_OP nY, abs ,%08Ch,4



;--------------------------------
; compare


CMP_op  macro reg,amode,op,cy
  opbegin op
    R&amode
    mov ah,reg
    sub ah,al
    setnc [ncpu.carry]
    mov  [ncpu.sign],ah
    mov  [ncpu.zero],ah
  opendzx cy
 ENDM


;CMP a
CMP_op nA, immb,%0C9h,2
CMP_op nA, zero,%0C5h,3
CMP_op nA,zeroX,%0D5h,4
CMP_op nA,  abs,%0CDh,4
CMP_op nA, absX,%0DDh,4
CMP_op nA, absY,%0D9h,4
CMP_op nA, indX,%0C1h,6
CMP_op nA, indY,%0D1h,5


;CPX a
CMP_op nX, immb,%0E0h,2
CMP_op nX, zero,%0E4h,3
CMP_op nX,  abs,%0ECh,4

;CPY a
CMP_op nY, immb,%0C0h,2
CMP_op nY, zero,%0C4h,3
CMP_op nY,  abs,%0CCh,4



;-------------------------------
;INC/DEC

;INX
opbegin %0E8h
   inc nX
   savef nX
opend 2

;INY
opbegin %0C8h
   inc nY
   savef nY
opend 2


;DEX
opbegin %0CAh
   dec nX
   savef nX
opend 2

;DEY
opbegin %088h
   dec nY
   savef nY
opend 2


INC_op  macro amode,op,cy
  opbegin op
    R&amode
    inc  al
    savef al
    W&amode
  opend cy
 ENDM

DEC_op  macro amode,op,cy
  opbegin op
    R&amode
    dec  al
    savef al
    W&amode
  opend cy
 ENDM

INC_op  zero ,%0E6h,5
INC_op  zeroX,%0F6h,6
INC_op   abs ,%0EEh,6
INC_op   absX,%0FEh,7

DEC_op  zero ,%0C6h,5
DEC_op  zeroX,%0D6h,6
DEC_op   abs ,%0CEh,6
DEC_op   absX,%0DEh,7


;----------------
; shift


;ASLA
opbegin %0Ah
  shl nA,1
  savefc nA
opend 2

;LSRA
opbegin %4Ah
  shr nA,1
  savefc nA
opend 2

;ROLA
opbegin %2Ah
  getc
  rcl nA,1
  savefc nA
opend 2

;RORA
opbegin %6Ah
  getc
  rcr nA,1
  savefc nA
opend 2


ASL_op macro amode,op,cy
  opbegin op
    R&amode
    shl    al,1
    savefc al
    W&amode
  opend cy
 ENDM

LSR_op macro amode,op,cy
  opbegin op
    R&amode
    shr    al,1
    savefc al
    W&amode
  opend cy
 ENDM


ROL_op macro amode,op,cy
  opbegin op
    R&amode
    getc
    rcl    al,1
    savefc al
    W&amode
  opend cy
 ENDM

ROR_op macro amode,op,cy
  opbegin op
    R&amode
    getc
    rcr    al,1
    savefc al
    W&amode
  opend cy
 ENDM

ASL_op   zero,%006h,5
ASL_op  zeroX,%016h,6
ASL_op    abs,%00Eh,6
ASL_op   absX,%01Eh,7

LSR_op   zero,%046h,5
LSR_op  zeroX,%056h,6
LSR_op    abs,%04Eh,6
LSR_op   absX,%05Eh,7

ROL_op   zero,%026h,5
ROL_op  zeroX,%036h,6
ROL_op    abs,%02Eh,6
ROL_op   absX,%03Eh,7

ROR_op   zero,%066h,5
ROR_op  zeroX,%076h,6
ROR_op    abs,%06Eh,6
ROR_op   absX,%07Eh,7



;----------
;flag

;CLC
opbegin %018h
  mov [ncpu.carry],dl ;dl=0
opend 2

;CLD
opbegin %0D8h
  and [ncpu.rF],NOT F_DEC
opend 2

;CLI
opbegin %058h
  and [ncpu.rF],NOT F_INT
opend 2

;CLV
opbegin %0B8h
  mov [ncpu.over],dl
opend 2

;SEC
opbegin %038h
  mov [ncpu.carry],01h
opend 2

;SED
opbegin %0F8h
  or [ncpu.rF],F_DEC
opend 2

;SEI
opbegin %078h
  or [ncpu.rF],F_INT
opend 2


;---------------
; Txx

;TAX
opbegin %0AAh
   mov nX,nA
   savef nA
opend 2

;TXA
opbegin %08Ah
   mov nA,nX
   savef nX
opend 2

;TAY
opbegin %0A8h
   mov nY,nA
   savef nA
opend 2

;TYA
opbegin %098h
   mov nA,nY
   savef nY
opend 2

;TSX
opbegin %0BAh
   mov nX,nS
   savef nS
opend 2

;TXS
opbegin %09Ah
   mov nS,nX
opend 2



;-----------------
; branch

;execute branch
quickbranch:
  fetchbytesx edx
  mov al,4        ;4 cycles for branch taken  (should check page)
  add esi,edx
  jmp executeloop

dobranch:
  mov al,[esi]
  inc esi

  sub esi,[ncpu.PCBASE]
  test al,al

  mov edx,esi
  jl  brbackward

  ;forward branch
brforward:
  add dl,al ;add branch
  jnc short donebranch
  inc dh
  mov esi,edx
  mov al,4   ;4 cycles
;  call naturalizeESI
  add esi,[ncpu.PCBASE]
  jmp executeloop

  ;backward branch
brbackward:
  add dl,al ;add branch
  jc  short donebranch
  dec dh
  mov esi,edx
  mov al,4   ;4 cycles
;  call naturalizeESI
  add esi,[ncpu.PCBASE]
  jmp executeloop

  ;no page crossing
donebranch:
  mov esi,edx
  mov al,3    ;3 cycles
;  call naturalizeESI
  add esi,[ncpu.PCBASE]
  jmp executeloop




;BPL rel
opbegin %010h
  mov  dl,[ncpu.sign]

  test dl,dl
  jns dobranch
  inc esi
opend 2

;BMI rel
opbegin %030h
  mov  dl,[ncpu.sign]

  test dl,dl
  js  dobranch
  inc esi
opend 2

;BVC rel
opbegin %050h
  cmp dl,[ncpu.over]
  je  dobranch
  inc esi
opend 2

;BVS rel
opbegin %070h
  cmp dl,[ncpu.over]
  jne  dobranch
  inc esi
opend 2


;BCC rel
opbegin %090h
  cmp dl,[ncpu.carry]
  je  dobranch
  inc esi
opend 2

;BCS rel
opbegin %0B0h
  cmp dl,[ncpu.carry]
  jne  dobranch
  inc esi
opend 2


;BNE rel
opbegin %0D0h
  cmp dl,[ncpu.zero]
  jne dobranch
  inc esi
opend 2


;BEQ rel
opbegin %0F0h
  cmp dl,[ncpu.zero]
  je  dobranch
  inc esi
opend 2


;---------------
;jump


;JMP $XXXX
opbegin %04Ch
   movzx esi,word ptr [esi]
   call naturalizeESI
opend 3


;JMP ($XXXX)
opbegin %06Ch
   push ebx
   mov  dx,word ptr [esi] ;get addr of address
   xor  ebx,ebx

   call readbyte
   mov  bl,al

   inc  dl

   call readbyte
   mov  bh,al

   mov  esi,ebx
   pop  ebx
   call naturalizeESI
opend   5

;JSR $XXXX
opbegin %20h
   mov   eax,esi  ;get 32-bit PC
   xor   esi,esi

   mov   dl,nS    ;get SP
   mov   si,[eax] ;get new PC

   sub   eax,[ncpu.PCBASE] ;get 16-bit PC
   call  naturalizeESI ;set new 32-bit PC

   inc ax            ;increase old PC
   npushb ah
   npushb al

   mov nS,dl         ;set SP
opendzx 6


;RTS
opbegin %60h
  mov dl,nS ;get SP

  ;pop PC
  npopb al
  npopb ah

  ;store SP inc PC
  mov nS,dl
  inc ax

  ;set new PC
  mov  esi,eax
  call naturalizeESI
opendzx 6


;RTI
opbegin %40h
  mov dl,nS ;get SP

  ;pop flags
  npopb al
  or  al,F_RSRVD
  degenerateflags

  ;pop PC
  npopb al
  npopb ah

  ;store SP
  mov nS,dl

  ;set new PC
  mov  esi,eax
  call naturalizeESI
opendzx 6



;-------------------
; stack


;PHA
opbegin %48h
  mov dl,nS
  dec nS
  mov [ebp+edx+100h],nA
opend 3

;PHP
opbegin %08h
  mov dl,nS
  dec nS
  generateflags
  mov [ebp+edx+100h],al
opend 3

;PLA
opbegin %68h
  inc nS
  mov dl,nS
  mov nA,[ebp+edx+100h]
  savef nA
opend 4

;PLP
opbegin %28h
  inc nS
  mov dl,nS
  mov al,[ebp+edx+100h]
  or  al,F_RSRVD
  degenerateflags
opend 4


;---------------------------
; BRK


opbegin %00h
   denaturalizeESI
   mov eax,esi ;get pc

   mov dl,nS   ;get stack
   inc ax

   ;push pc
   npushb ah
   npushb al

   ;push flags
   mov al,[ncpu.rF]
   or  al,F_BREAK ;set break flag ?
   npushb al

   ;store S
   mov nS,dl

   ;adjust flags
   or  al,F_INT ;turn on interrupt
   xor esi,esi
   mov [ncpu.rF],al

   ;PC=BRK vector
   mov si,word ptr [edi+0FFFEh]
   call naturalizeESI
opendzx 7



;--------------------------------------
        .data

   ;jump table
makeopcode macro X
 ifdef op&X
  dd offset op&X
 else
  dd offset badopcode
 endif
endm

n6502jmptable label dword
i=0
rept 256
  makeopcode %i
  i=i+1
endm

      end










