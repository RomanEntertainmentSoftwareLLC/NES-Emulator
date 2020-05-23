Attribute VB_Name = "M6502AddrModes"
' My 2nd try at a 6502 emulator. This one is based on Neil Bradley's
' 6502 C source. I don't like Marat's because it's slower...
' This emulator features all 6502 addressing modes. Also
' all 157 6502 OpCodes are supported.
' The emulator is *VERY* buggy still and the addressing modes
' are called at the wrong times.
' This was written for all the Visual Basic programmers that
' want to write a CPU emulator but don't know where to start.
' I didn't write most of this, it was written by some guy, and the
' source was used in EMU by Neil Bradley. Thanks for donating this
' Neil =).
Public A, X, Y As Byte
Global Const IPeriod = 26500
Public F, S, P As Byte
Public Const DEBUGGING = 0
Public PC As Long
Public opcode As Byte
Public clockticks6502 As Long
' Help Variables
Public savepc As Long
Public value As Integer
Public sum, saveflags As Integer
' arrays
Public ticks(0 To 256) As Integer
Public addrmode(0 To 256)
Public RAM(0 To 65536) As Byte
Public instruction(0 To 256)
Public gameImage(0 To 524888) As Byte ' Load the game into this array with this code:
' Open "filename" for binary access read as #1
' for i = 0 to filelen(filename)
' get #1,, gameImage(i)
' next i
' close #1
Declare Function SetPixel Lib "gdi32" (ByVal hdc%, ByVal X%, ByVal Y%, ByVal crColor&) As Long
Public TrapBadOps As Integer  ' Quit emulation when OpCode is invalid...
Public CPUPaused As Integer  ' Is the CPU Paused
Public CPURunning As Integer ' Is the CPU running?
Public ROMLoaded As Integer  ' Is a Game loaded?
Public Trace As Integer      ' Print Current OpCode?
Public Function implied6502()

End Function

Public Function indabsx6502()
      help = gameImage(PC) + (gameImage(PC + 1) * 256) + X
      savepc = gameImage(help) + (gameImage(help + 1) * 2 ^ 8)
End Function
Public Function indx6502()
      value = gameImage(PC + 1) + X
      'savepc = gameImage(value) + (gameImage(value + 1) * 2 ^ 8)
End Function
Public Function indy6502()
       ' This is the zpx6502() OpCode
      value = gameImage(PC + 1)
      savepc = gameImage(value) + (gameImage(value + 1) * 2 ^ 8)
      If (ticks(opcode) = 5) Then
              If ((savepc / 2 ^ 8) = ((savepc + Y) / 2 ^ 8)) Then
                    clockticks6502 = clockticks6502 + 1
      savepc = savepc + Y
              End If
      End If
End Function
Public Function exec6502()
If ROMLoaded <> 0 Then
If CPUPaused = 1 Then
Do Until CPUPaused = 0 And CPURunning = 1
xx& = DoEvents()
Loop
End If
clockticks6502 = clockticks6502 - ticks(opcode)
opcode = gameImage(PC)  ' Fetch Next Operation
If DEBUGGING = 1 Then
Debug.Print "Opcode #" & opcode & " A:" & A & " X:" & X & " Y:" & Y & " S:" & S & " P:" & P & " PC:" & PC & " CYCLES:" & clockticks6502
End If
PC = PC + 1
If S > 255 Then S = 255
If S < 0 Then S = 0
Select Case instruction(opcode)
Case "asl6502": asl6502
Case "nop6502": nop6502
Case "sbc6502": sbc6502
Case "sta6502": sta6502
Case "stx6502": stx6502
Case "sty6502": sty6502
Case "and6502": and6502
Case "asla6502": asla6502
Case "bne6502": bne6502
Case "sei6502": sei6502
Case "bcc6502": bcc6502
Case "bcs6502": bcs6502
Case "beq6502": beq6502
Case "bit6502": bit6502
Case "bmi6502": bmi6502
Case "brk6502": brk6502
Case "bpl6502": bpl6502
Case "bvc6502": bvc6502
Case "bvs6502": bvs6502
Case "clc6502": clc6502
Case "cld6502": cld6502
Case "cli6502": cli6502
Case "clv6502": clv6502
Case "cmp6502": cmp6502
Case "cpx6502": cpx6502
Case "cpy6502": cpy6502
Case "dec6502": dec6502
Case "dex6502": dex6502
Case "dey6502": dey6502
Case "eor6502": eor6502
Case "inc6502": inc6502
Case "inx6502": inx6502
Case "iny6502": iny6502
Case "jmp6502": jmp6502
Case "jsr6502": jsr6502
Case "lda6502": lda6502
Case "ldx6502": ldx6502
Case "ldy6502": ldy6502
Case "lsr6502": lsr6502
Case "lsra6502": lsra6502
Case "ora6502": ora6502
Case "pha6502": pha6502
Case "php6502": php6502
Case "pla6502": pla6502
Case "plp6502": plp6502
Case "rol6502": rol6502
Case "rola6502": rola6502
Case "ror6502": ror6502
Case "rora6502": rora6502
Case "rti6502": rti6502
Case "rts6502": rts6502
Case "sbc6502": sbc6502
Case "sec6502": sec6502
Case "sed6502": sed6502
Case "sei6502": sei6502
Case "tax6502": tax6502
Case "tay6502": tay6502
Case "tsx6502": tsx6502
Case "txa6502": txa6502
Case "tya6502": tya6502
Case "bra6502": bra6502
Case "dea6502": dea6502
Case "dex6502": dex6502
Case "dey6502": dey6502
Case "ina6502": ina6502
Case "phx6502": phx6502
Case "plx6502": plx6502
Case "phy6502": phy6502
Case "ply6502": ply6502
Case "stz6502": stz6502
Case "tsb6502": tsb6502
Case "trb6502": trb6502
End Select
clockticks6502 = clockticks6502 + ticks(opcode)
Else
MsgBox "m6502 Error: NO ROM LOADED", 48, "m6502"
End If
End Function
Public Function indzp6502()
      value = gameImage(PC)
      savepc = gameImage(value) + (gameImage(value) * 2 ^ 8)
End Function

Public Function zpy6502()
      savepc = gameImage(PC) + Y
      savepc = savepc And &HFF
End Function

Public Function absy6502()
      savepc = gameImage(PC) + (gameImage(PC + 1) * 256)
      PC = PC + 1
      PC = PC + 1
      If (ticks(opcode) = 4) Then
              If ((savepc / 2 ^ 8) = ((savepc + Y) / 2 ^ 8)) Then
                      clockticks6502 = clockticks6502 + 1
                      savepc = savepc + Y
              End If
      End If
End Function

Public Function get6502memory(addr As Long) As Variant
    get6502memory = gameImage(addr)
End Function
Public Function immediate6502()
savepc = PC + 1
End Function
Public Function indirect6502()
      help = gameImage(PC) + (gameImage(PC + 1) * 256)
      savepc = gameImage(help) + (gameImage(help + 1) * 256)
      PC = PC + 1
      PC = PC + 1
End Function
Public Function absx6502()
      savepc = gameImage(PC) + (gameImage(PC + 1) * 256)
      PC = PC + 1
      PC = PC + 1
      If (ticks(opcode) = 4) Then
              If ((savepc / 2 ^ 8) = ((savepc + X) / 2 ^ 8)) Then
                      clockticks6502 = clockticks6502 + 1
                      savepc = savepc + X
              End If
      End If
End Function
Public Function put6502memory(addr As Long, byte1 As Byte)
     gameImage(addr) = value
End Function
Public Function abs6502()
      savepc = gameImage(PC) + (gameImage(PC + 1) * 256)
      PC = PC + 1
      PC = PC + 1
End Function
Public Function relative6502()
savepc = gameImage(PC + 1)
If (savepc And &H80) Then savepc = savepc - &H100
If ((savepc / 2 ^ 8) = (PC / 2 ^ 8)) Then
clockticks6502 = clockticks6502 + 1
End If
End Function

Public Function reset6502()
 A = 0: X = 0: Y = 0: P = 0
 P = P Or &H20
 S = &HFF
 PC = gameImage(65532) + (gameImage(65533) * 2 ^ 8)
    'PC = 32768
    Debug.Print "Reset to " & PC
End Function
Public Function zp6502()
savepc = gameImage(PC)
PC = PC + 1
End Function
Public Function zpx6502()
      savepc = gameImage(PC) + X
      PC = PC + 1
      savepc = savepc And &HFF
End Function
Public Function irq6502()
   ' Maskable interrupt
   xx& = put6502memory(&H100 + S, (PC / 2 ^ 8))
   S = S - 1
   xx& = put6502memory(&H100 + S, (PC And &HFF))
   S = S - 1
   xx& = put6502memory(&H100 + S, P)
   S = S - 1
   P = P Or &H4
   PC = gameImage(65534) Or gameImage(65535) * 2 ^ 8
End Function
Public Function nmi6502()
      xx& = put6502memory(&H100 + S, (PC * 2 ^ 8))
      S = S - 1
      xx& = put6502memory(&H100 + S, (PC And 255))
      S = S - 1
      xx& = put6502memory(&H100 + S, P)
      P = P Or &H4
      PC = gameImage(65530) Or gameImage(65531) * 2 ^ 8
End Function
