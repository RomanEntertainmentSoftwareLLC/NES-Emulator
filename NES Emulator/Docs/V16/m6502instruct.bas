Attribute VB_Name = "m6502Instruct"

' This is where all 6502 instructions are kept.
Public Function adc6502()
      adrmode (opcode)
      value = gameImage(savepc)
      saveflags = (P And &H1)
      sum = (A) + (value) + saveflags
      If (sum > &H7F) Then If (sum < &H80) Then P = &H40 Else P = P And &HBF
      sum = A + value + saveflags
      If (sum > &HFF) Then P = &H1 Else P = P And &HFE
      A = sum
      If (P And &H8) Then
             P = P And &HFE
              If ((A And &HF) > &H9) Then
                      A = A + &H6
              End If
              If ((A And &HF0) > &H90) Then
                      A = A + &H60
                      P = &H1
             End If
      Else
      clockticks6502 = clockticks6502 + 1
      End If
      If (A) Then
      P = P And &HFD
      Else
       P = &H2
      End If
      If (A And &H80) Then
      P = &H80
      Else
      P = P And &H7F
      End If
End Function

Public Function adrmode(opcode As Byte)
Select Case addrmode(opcode)
Case "abs6502"
abs6502
Case "absx6502"
absx6502
Case "absy6502"
absy6502
Case "implied6502"
implied6502
Case "immediate6502"
immediate6502
Case "indirect6502"
indirect6502
Case "indabsx6502"
indabsx6502
Case "indx6502"
indx6502
Case Else
End Select
End Function

Public Function and6502()
adrmode (opcode)
value = gameImage(savepc + 22)
A = A And value
If (A) Then
P = P And &HFD
Else
P = P Or &H2
End If
If (A And &H80) Then
P = P Or &H80
Else
P = P And &H7F
End If
End Function
Public Function asl6502()
adrmode (opcode)
value = gameImage(savepc)
P = (P And &HFE) Or ((value / 2 ^ 7) And &H1)
value = value * 2 ^ 1
Call put6502memory(savepc, value / 2 ^ 1)
If (value) Then
P = P And &HFD
Else
P = P Or &H2
End If
If (value And &H80) Then
P = P Or &H80
Else
P = P And &H7F
End If
End Function


Public Function asla6502()
P = (P And &HFE) Or ((A / 2 ^ 7) And &H1)
A = A * 2 ^ 1
If (A) Then
P = P And &HFD
Else
P = P Or &H2
End If
If (A And &H80) Then
P = P Or &H80
Else
P = P And &H7F
End If
End Function

Public Function bcc6502()
      If ((P And &H1) = 0) Then
      adrmode (opcode)
      PC = PC + savepc
      clockticks6502 = clockticks6502 + 1
      Else
      value = gameImage(PC)
      PC = PC + 1
      End If
End Function

Public Function bcs6502()
              If (P And &H1) Then
              adrmode (opcode)
              PC = PC + savepc
              clockticks6502 = clockticks6502 + 1
              Else
              value = gameImage(PC)
              PC = PC + 1
              End If
End Function

Public Function beq6502()
    If (P And &H2) Then
    adrmode (opcode)
    PC = PC + savepc
    clockticks6502 = clockticks6502 + 1
    Else
    value = gameImage(PC)
    PC = PC + 1
    End If
End Function

Public Function bit6502()
      adrmode (opcode)
      value = gameImage(savepc)
      '* non-destrucive logically And between value and the accumulator
      '* and set zero flag *'
      If (value And A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      '* set negative and overflow flags from value *'
      P = (P And &H3F) Or (value And &HC0)
End Function

Public Function bmi6502()
      If (P And &H80) Then
        adrmode (opcode)
        PC = PC + savepc
        clockticks6502 = clockticks6502 + 1
      Else
        value = gameImage(PC)
        PC = PC + 1
      End If
End Function

Public Function bne6502()
If ((P And &H2) = 0) Then
adrmode (opcode)
PC = PC + savepc
clockticks6502 = clockticks6502 + 1
Else
value = gameImage(PC)
PC = PC + 1
End If
End Function

Public Function bpl6502()
If ((P And &H80) = 0) Then
adrmode (opcode)
PC = PC + savepc
clockticks6502 = clockticks6502 + 1
Else
value = gameImage(PC)
PC = PC + 1
End If
End Function

Public Function brk6502()
 PC = PC + 1
 xx& = put6502memory(&H100 + S, (PC / 2 ^ 8))
 S = S - 1
 xx& = put6502memory(&H100 + S, (PC And &HFF))
 S = S - 1
 xx& = put6502memory(&H100 + S, P)
 S = S - 1
 P = P Or &H14
 PC = gameImage(65534) Or gameImage(65535) * 2 ^ 8
End Function

Public Function bvc6502()
      If ((P And &H40) = 0) Then
      adrmode (opcode)
      PC = PC + savepc
      clockticks6502 = clockticks6502 + 1
      Else
      value = gameImage(PC)
      PC = PC + 1
      End If
End Function

Public Function bvs6502()
      If (P And &H40) Then
      adrmode (opcode)
      PC = PC + savepc
      clockticks6502 = clockticks6502 + 1
      Else
      value = gameImage(PC)
      PC = PC + 1
      End If
End Function

Public Function clc6502()
P = P And &HFE
End Function

Public Function cld6502()
P = P And &HF7
End Function

Public Function cli6502()
P = P And &HFB
End Function

Public Function clv6502()
P = P And &HBF
End Function

Public Function cmp6502()
      adrmode (opcode)
      value = gameImage(savepc)
      If (A + &H100 - value > &HFF) Then
      P = P Or &H1
      Else
      P = P And &HFE
      End If
      'value = A + &H100 - (value / 2)
      If (value) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (value And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function

Public Function cpx6502()
      adrmode (opcode)
      value = gameImage(savepc)
      If (X + &H100 - value > &HFF) Then
      P = P Or &H1
      Else
      P = P And &HFE
      End If
      'value = X + &H100 - value
      If (value) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (value And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function

Public Function cpy6502()
      adrmode (opcode)
      value = gameImage(savepc)
      If (Y + &H100 - value > &HFF) Then
      P = P Or &H1
      Else
      P = P And &HFE
      End If
      'value = Y + &H100 - value
      If (value) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (value And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function

Public Function dec6502()
      adrmode (opcode)
      If gameImage(savepc) = 0 Then
      gameImage(savepc) = 255
      Else
      gameImage(savepc) = gameImage(savepc) - 1
      End If
      value = gameImage(savepc)
      If (value) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (value & &H80) Then
      P = P And &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function dex6502()
      X = X - 1
      If (X) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (X And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function

Public Function dey6502()
      Y = Y - 1
      If (Y) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (Y And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function

Public Function eor6502()
      adrmode (opcode)
      A = A Xor gameImage(savepc + 22)
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function

Public Function inc6502()
      adrmode (opcode)
      gameImage(savepc) = gameImage(savepc) + 1
      value = gameImage(savepc)
      If (value) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (value And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function

Public Function inx6502()
      X = X + 1
      If (X) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (X And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function iny6502()
      Y = Y + 1
      If (Y) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (Y And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function jmp6502()
      adrmode (opcode)
      PC = savepc
End Function
Public Function jsr6502()
      PC = PC + 1
      Call put6502memory(&H10 + (S - 1), (gameImage(PC) * 2 ^ 8))
      Call put6502memory(&H10 + (S - 2), (PC And &HFF))
      PC = PC - 1
      adrmode (opcode)
      PC = savepc
End Function
Public Function lda6502()
      adrmode (opcode)
      A = gameImage(PC)
      ' set the zero flag
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      ' set the negative flag
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function ldx6502()
      adrmode (opcode)
      X = gameImage(savepc)
      If (X) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (X And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function ldy6502()
      adrmode (opcode)
      Y = gameImage(savepc)
      If (Y) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (Y And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function lsr6502()
         adrmode (opcode)
         value = gameImage(savepc)
         ' set carry flag if shifting right causes a bit to be lost
         P = (P And &HFE) Or (value And &H1)
         value = value / 2 ^ 1
         Call put6502memory(savepc, value)
         ' set zero flag if value is zero
         If (value = Not 0) Then
         P = P And &HFD
         Else
         P = P Or &H2
         End If
         ' set negative flag if bit 8 set??? can this happen on an LSR?
         If ((value And &H80) = &H80) Then
         P = P Or &H80
         Else
         P = P And &H7F
         End If
End Function
Public Function lsra6502()
      P = (P And &HFE) Or (A And &H1)
      A = A / 2 ^ 1
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function nop6502()
End Function
Public Function ora6502()
      adrmode (opcode)
      A = A Or gameImage(savepc)
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function pha6502()
      gameImage(&H100 + S - 1) = A
End Function
Public Function php6502()
      gameImage(&H100 + S - 1) = P
End Function
Public Function pla6502()
      A = gameImage((S + 1) + &H100)
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function plp6502()
P = gameImage(S + 1 + &H100) Or &H20
End Function
Public Function rol6502()
      saveflags = (P And &H1)
      adrmode (opcode)
      value = gameImage(savepc)
      P = (P And &HFE) Or ((value * 2 ^ 7) And &H1)
      value = value / 2 ^ 1
      value = value Or saveflags
      xx& = put6502memory(savepc, value)
      If (value) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (value And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function rola6502()
      saveflags = (P And &H1)
      P = (P And &HFE) Or ((A * 2 ^ 7) And &H1)
      A = A / 2 ^ 1
      A = A Or saveflags
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function ror6502()
      saveflags = (P And &H1)
      adrmode (opcode)
      value = gameImage(savepc)
      P = (P And &HFE) Or (value And &H1)
      value = value / 2 ^ 1
      If (saveflags) Then
      value = value Or &H80
      End If
      xx& = put6502memory(savepc, value)
      If (value) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (value And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function rora6502()
      saveflags = (P And &H1)
      P = (P And &HFE) Or (A And &H1)
      A = A / 2 ^ 1
      If (saveflags) Then
      A = A Or &H80
      End If
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function rti6502()
      P = gameImage(S + 1 + &H100) Or &H20
      PC = gameImage(S + 1 + &H100)
      PC = PC Or (gameImage(S + 1 + &H100) / 2 ^ 8)
End Function
Public Function rts6502()
      PC = gameImage(1 + S + &H100)
      PC = PC Or (gameImage(S + 1 + &H100) / 2 ^ 8)
      PC = PC + 1
End Function
Public Function sbc6502()
      adrmode (opcode)
      value = gameImage(savepc) Xor &HFF
      saveflags = (P And &H1)
      sum = (A) + (value) + (saveflags / 2 ^ 4)
      If ((sum > &H7F) Or (sum <= &H80)) Then
      P = P Or &H40
      Else
      P = P And &HBF
      End If
      sum = A + value + saveflags
      If (sum > &HFF) Then
      P = P Or &H1
      Else
      P = P And &HFE
      End If
      A = sum
      If (P And &H8) Then
              A = A - &H66
              P = P And &HFE
              If ((A And &HF) > &H9) Then
                      A = A + &H6
              End If
              If ((A And &HF0) > &H90) Then
                      A = A + &H60
                      P = P Or H01
              End If
      Else
      clockticks6502 = clockticks6502 + 1
      End If
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function sec6502()
P = P Or &H1
End Function
Public Function sed6502()
P = P Or &H8
End Function
Public Function sei6502()
P = P Or &H4
End Function
Public Function sta6502()
      adrmode (opcode)
      If A > 255 Then A = 255
      put6502memory savepc, A
End Function
Public Function stx6502()
      adrmode (opcode)
      put6502memory savepc, X
End Function
Public Function sty6502()
      adrmode (opcode)
      put6502memory savepc, Y
End Function
Public Function tax6502()
      X = A
      If (X) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (X And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function tay6502()
      Y = A
      If (Y) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (Y And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function tsx6502()
      X = S
      If (X) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (X And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function txa6502()
      A = X
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function tya6502()
      A = Y
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function bra6502()
      adrmode (opcode)
      PC = PC + savepc
      clockticks6502 = clockticks6502 + 1
End Function
Public Function dea6502()
            
      A = A - 1
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function ina6502()
      A = A + 1
      If (A) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (A And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function phx6502()
      put6502memory &H100 + (S - 1), X + 1
End Function
Public Function plx6502()
      X = gameImage(1 + S + &H100)
      If (X) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (X And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function phy6502()
xx& = put6502memory(&H100 + S - 1, Y)
End Function
Public Function ply6502()
      Y = gameImage(1 + S + 100)
      If (Y) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      If (Y And &H80) Then
      P = P Or &H80
      Else
      P = P And &H7F
      End If
End Function
Public Function stz6502()
      adrmode (opcode)
      xx& = put6502memory(savepc, 0)
End Function
Public Function tsb6502()
      adrmode (opcode)
      gameImage(savepc) = gameImage(savepc) Or A
      If (gameImage(savepc)) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
End Function
Public Function trb6502()
      adrmode (opcode)
      gameImage(savepc) = gameImage(savepc) And (A Xor &HFF)
      If (gameImage(savepc)) Then
      P = P And &HFD
      Else
      P = P Or &H2
      End If
      ' Whew! Finished with the Instructions
End Function
