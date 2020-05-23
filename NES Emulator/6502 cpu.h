#pragma once

#include "bits.h"
#include "6502 cpu memory.h"

//Data Types
typedef unsigned char byte;
typedef unsigned short ushort;

class MOS6502 : public CPU_Memory
{
private:
	enum REGION {
		NTSC,
		PAL,
		DENDY
	};

	class REGISTERS {
	public:
		static byte a;
		static byte x;
		static byte y;
		static byte sp;
		static byte sr;
		static ushort pc;
	};

	class REGISTER_CONTROL {
	protected:
		const byte CARRY_REGISTER = BIT_0;		//1
		const byte ZERO_REGISTER = BIT_1;		//2
		const byte INTERRUPT_REGISTER = BIT_2;	//4
		const byte DECIMAL_REGISTER = BIT_3;	//8
		const byte BREAK_REGISTER = BIT_4;		//16
		const byte UNUSED_REGISTER = BIT_5;		//32 is ignored (Always set to 1)
		const byte OVERFLOW_REGISTER = BIT_6;	//64
		const byte SIGN_REGISTER = BIT_7;		//128

		void Set_Carry();
		void Clear_Carry();
		bool Is_Carry();

		void Set_Zero();
		void Clear_Zero();
		bool Is_Zero();

		void Set_Interrupt();
		void Clear_Interrupt();
		bool Is_Interrupt();

		void Set_Decimal();
		void Clear_Decimal();
		bool Is_Decimal();

		void Set_Break();
		void Clear_Break();
		bool Is_Break();

		void Set_Overflow();
		void Clear_Overflow();
		bool Is_Overflow();

		void Set_Sign();
		void Clear_Sign();
		bool Is_Sign();
	};

	class ADDRESS_MODES {
	protected:
		void Accumulator();
		void Immediate();
		void Zero_Page(ushort& address);
		void Zero_Page_X(ushort& address);
		void Zero_Page_Y(ushort& address);

		void Absolute(ushort& address);
		void Absolute_X(ushort& address);
		void Absolute_Y(ushort& address);
		void Implied();
		void Relative(ushort& address);

		void Indirect_X(ushort& address);
		void Indirect_Y(ushort& address);
		void Indirect(ushort& address);
	};

	class _ADC_ : REGISTER_CONTROL, ADDRESS_MODES {
		//  ADC - ADd with Carry
		//	Affects Flags : S V Z C
		//	MODE           SYNTAX         HEX LEN TIM
		//	Immediate      ADC #$44       $69  2   2
		//	Zero Page      ADC $44        $65  2   3
		//	Zero Page, X   ADC $44, X     $75  2   4
		//	Absolute       ADC $4400      $6D  3   4
		//	Absolute, X    ADC $4400, X   $7D  3   4 +
		//	Absolute, Y    ADC $4400, Y   $79  3   4 +
		//	Indirect, X    ADC($44, X)    $61  2   6
		//	Indirect, Y    ADC($44), Y    $71  2   5 +
		//	+add 1 cycles if page boundary crossed
		//	ADC results are dependant on the setting of the decimal flag.In decimal mode, addition is carried out on the assumption that the values involved are packed BCD(Binary Coded Decimal).
		//	There is no way to add without carry.
	private:
		void ADC(byte memory_data);
		void ADC(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
		void Absolute_Y(ushort address);
		void Indirect_X(ushort address);
		void Indirect_Y(ushort address);
	};

	class _AND_ : REGISTER_CONTROL, ADDRESS_MODES {
		//  AND(bitwise AND with accumulator)
		//	Affects Flags : S Z
		//	MODE           SYNTAX       HEX LEN TIM
		//	Immediate     AND #$44      $29  2   2
		//	Zero Page     AND $44       $25  2   3
		//	Zero Page, X   AND $44, X     $35  2   4
		//	Absolute      AND $4400     $2D  3   4
		//	Absolute, X    AND $4400, X   $3D  3   4 +
		//	Absolute, Y    AND $4400, Y   $39  3   4 +
		//	Indirect, X    AND($44, X)   $21  2   6
		//	Indirect, Y    AND($44), Y   $31  2   5 +
		//	+add 1 cycles if page boundary crossed
	private:
		void AND(byte memory_data);
		void AND(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
		void Absolute_Y(ushort address);
		void Indirect_X(ushort address);
		void Indirect_Y(ushort address);
	};

	class _ASL_ : REGISTER_CONTROL, ADDRESS_MODES {
		//  ASL(Arithmetic Shift Left)
		//	Affects Flags : S Z C
		//	MODE           SYNTAX       HEX LEN TIM
		//	Accumulator   ASL A         $0A  1   2
		//	Zero Page     ASL $44       $06  2   5
		//	Zero Page, X   ASL $44, X     $16  2   6
		//	Absolute      ASL $4400     $0E  3   6
		//	Absolute, X    ASL $4400, X   $1E  3   7
		//  ASL shifts all bits left one position. 0 is shifted into bit 0 and the original bit 7 is shifted into the Carry.
	private:
		void ASL();
		void ASL(ushort address);
	public:
		void Accumulator();
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
	};

	class _BCC_ : REGISTER_CONTROL, ADDRESS_MODES {
		//		                          HEX
		//	BCC(Branch on Carry Clear)    $90
	private:
		void BCC(ushort address);
	public:
		void Relative(ushort address);
	};

	class _BCS_ : REGISTER_CONTROL, ADDRESS_MODES {
		//		                          HEX
		//	BCS(Branch on Carry Set)      $B0
	private:
		void BCS(ushort address);
	public:
		void Relative(ushort address);
	};

	class _BEQ_ : REGISTER_CONTROL, ADDRESS_MODES {
		//		                          HEX
		//	BEQ(Branch on EQual)          $F0
	private:
		void BEQ(ushort address);
	public:
		void Relative(ushort address);
	};

	class _BIT_ : REGISTER_CONTROL, ADDRESS_MODES {
		//  BIT(test BITs)
		//	Affects Flags : N V Z
		//	MODE           SYNTAX       HEX LEN TIM
		//	Zero Page     BIT $44       $24  2   3
		//	Absolute      BIT $4400     $2C  3   4
		//	BIT sets the Z flag as though the value in the address tested were ANDed with the accumulator.
		//  The S and V flags are set to match bits 7 and 6 respectively in the value stored at the tested address.
	private:
		void BIT(ushort address);
	public:
		void Zero_Page(ushort address);
		void Absolute(ushort address);
	};

	class _BMI_ : REGISTER_CONTROL, ADDRESS_MODES {
		//		                          HEX
		//	BMI(Branch on MInus)          $30
	private:
		void BMI(ushort address);
	public:
		void Relative(ushort address);
	};

	class _BNE_ : REGISTER_CONTROL, ADDRESS_MODES {
		//		                          HEX
		//	BNE(Branch on Not Equal)      $D0
	private:
		void BNE(ushort address);
	public:
		void Relative(ushort address);
	};

	class _BPL_ : REGISTER_CONTROL, ADDRESS_MODES {
		//		                          HEX
		//	BPL(Branch on PLus)           $10
	private:
		void BPL(ushort address);
	public:
		void Relative(ushort address);
	};

	class _BRK_ : REGISTER_CONTROL, ADDRESS_MODES {
		//BRK(BReaK)
		//	Affects Flags : B
		//	MODE           SYNTAX       HEX LEN TIM
		//	Implied       BRK           $00  1   7
	private:
		void BRK();
	public:
		void Implied();
	};

	class _BVC_ : REGISTER_CONTROL, ADDRESS_MODES {
		//		                          HEX
		//	BVC(Branch on oVerflow Clear) $50
	private:
		void BVC(ushort address);
	public:
		void Relative(ushort address);
	};

	class _BVS_ : REGISTER_CONTROL, ADDRESS_MODES {
		//		                          HEX
		//	BVS(Branch on oVerflow Set)   $70
	private:
		void BVS(ushort address);
	public:
		void Relative(ushort address);
	};

	class _CLC_ : REGISTER_CONTROL, ADDRESS_MODES {
		//								  HEX
		//	CLC(CLear Carry)              $18
	private:
		void CLC();
	public:
		void Implied();
	};

	class _CLD_ : REGISTER_CONTROL, ADDRESS_MODES {
		//								  HEX
		//	CLD(CLear Decimal)            $D8
	private:
		void CLD();
	public:
		void Implied();
	};

	class _CLI_ : REGISTER_CONTROL, ADDRESS_MODES {
		//								  HEX
		//	CLI(CLear Interrupt)          $58
	private:
		void CLI();
	public:
		void Implied();
	};

	class _CLV_ : REGISTER_CONTROL, ADDRESS_MODES {
		//								  HEX
		//	CLV(CLear oVerflow)           $B8
	private:
		void CLV();
	public:
		void Implied();
	};

	class _CMP_ : REGISTER_CONTROL, ADDRESS_MODES {
		//  CMP(CoMPare accumulator)
		//	Affects Flags : S Z C
		//	MODE           SYNTAX       HEX LEN TIM
		//	Immediate     CMP #$44      $C9  2   2
		//	Zero Page     CMP $44       $C5  2   3
		//	Zero Page, X  CMP $44, X    $D5  2   4
		//	Absolute      CMP $4400     $CD  3   4
		//	Absolute, X   CMP $4400, X  $DD  3   4 +
		//	Absolute, Y   CMP $4400, Y  $D9  3   4 +
		//	Indirect, X   CMP($44, X)   $C1  2   6
		//	Indirect, Y   CMP($44), Y   $D1  2   5 +
		//	+add 1 cycles if page boundary crossed
		//	Compare sets flags as if a subtraction had been carried out.If the value in the accumulator is equal or greater 
		//  than the compared value, the Carry will be set.The equal(Z) and sign(S) flags will be set based on equality or 
		//  lack thereof and the sign(i.e.A >= $80) of the accumulator.
	private:
		void CMP(byte memory_data);
		void CMP(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
		void Absolute_Y(ushort address);
		void Indirect_X(ushort address);
		void Indirect_Y(ushort address);
	};

	class _CPX_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void CPX(byte memory_data);
		void CPX(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Absolute(ushort address);
	};

	class _CPY_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void CPY(byte memory_data);
		void CPY(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Absolute(ushort address);
	};

	class _DEC_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void DEC(ushort address);
	public:
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
	};

	class _DEX_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void DEX();
	public:
		void Implied();
	};

	class _DEY_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void DEY();
	public:
		void Implied();
	};

	class _EOR_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void EOR(byte memory_data);
		void EOR(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
		void Absolute_Y(ushort address);
		void Indirect_X(ushort address);
		void Indirect_Y(ushort address);
	};

	class _INC_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void INC(ushort address);
	public:
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
	};

	class _INX_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void INX();
	public:
		void Implied();
	};

	class _INY_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void INY();
	public:
		void Implied();
	};

	class _JMP_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void JMP(ushort address);
	public:
		void Absolute(ushort address);
		void Indirect(ushort address);
	};

	class _JSR_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void JSR(ushort address);
	public:
		void Absolute(ushort address);
	};

	class _LDA_ : REGISTER_CONTROL, ADDRESS_MODES {
		//LDA(LoaD Accumulator)
		//	Affects Flags : S Z
		//	MODE           SYNTAX         HEX LEN TIM
		//	Immediate      LDA #$44       $A9  2   2
		//	Zero Page      LDA $44        $A5  2   3
		//	Zero Page, X   LDA $44, X     $B5  2   4
		//	Absolute       LDA $4400      $AD  3   4
		//	Absolute, X    LDA $4400, X   $BD  3   4 +
		//	Absolute, Y    LDA $4400, Y   $B9  3   4 +
		//	Indirect, X    LDA($44, X)    $A1  2   6
		//	Indirect, Y    LDA($44), Y    $B1  2   5 +
		//	+add 1 cycles if page boundary crossed
	private:
		void LDA(byte memory_data);
		void LDA(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
		void Absolute_Y(ushort address);
		void Indirect_X(ushort address);
		void Indirect_Y(ushort address);
	};

	class _LDX_ : REGISTER_CONTROL, ADDRESS_MODES {
		//LDX(LoaD X register)
		//	Affects Flags : S Z
		//	MODE           SYNTAX       HEX LEN TIM
		//	Immediate     LDX #$44      $A2  2   2
		//	Zero Page     LDX $44       $A6  2   3
		//	Zero Page, Y   LDX $44, Y     $B6  2   4
		//	Absolute      LDX $4400     $AE  3   4
		//	Absolute, Y    LDX $4400, Y   $BE  3   4 +
		//	+add 1 cycles if page boundary crossed
	private:
		void LDX(byte memory_data);
		void LDX(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_Y(ushort address);
		void Absolute(ushort address);
		void Absolute_Y(ushort address);
	};

	class _LDY_ : REGISTER_CONTROL, ADDRESS_MODES {
		//LDY(LoaD Y register)
		//	Affects Flags : S Z
		//	MODE           SYNTAX       HEX LEN TIM
		//	Immediate     LDY #$44      $A0  2   2
		//	Zero Page     LDY $44       $A4  2   3
		//	Zero Page, X   LDY $44, X     $B4  2   4
		//	Absolute      LDY $4400     $AC  3   4
		//	Absolute, X    LDY $4400, X   $BC  3   4 +
		//	+add 1 cycles if page boundary crossed
	private:
		void LDY(byte memory_data);
		void LDY(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
	};

	class _LSR_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void LSR();
		void LSR(ushort address);
	public:
		void Accumulator();
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
	};

	class _NOP_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void NOP();
	public:
		void Implied();
	};

	class _ORA_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void ORA(byte data);
		void ORA(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
		void Absolute_Y(ushort address);
		void Indirect_X(ushort address);
		void Indirect_Y(ushort address);
	};

	class _PHA_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void PHA();
	public:
		void Implied();
	};

	class _PHP_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void PHP();
	public:
		void Implied();
	};

	class _PLA_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void PLA();
	public:
		void Implied();
	};

	class _PLP_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void PLP();
	public:
		void Implied();
	};

	class _ROL_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void ROL();
		void ROL(ushort address);
	public:
		void Accumulator();
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
	};

	class _ROR_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void ROR();
		void ROR(ushort address);
	public:
		void Accumulator();
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
	};

	class _RTI_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void RTI();
	public:
		void Implied();
	};

	class _RTS_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void RTS();
	public:
		void Implied();
	};

	class _SBC_ : REGISTER_CONTROL, ADDRESS_MODES {
		// SBC(SuBtract with Carry)
		//		Affects Flags : S V Z C
		//		MODE           SYNTAX       HEX LEN TIM
		//		Immediate     SBC #$44      $E9  2   2
		//		Zero Page     SBC $44       $E5  2   3
		//		Zero Page, X  SBC $44, X    $F5  2   4
		//		Absolute      SBC $4400     $ED  3   4
		//		Absolute, X   SBC $4400, X  $FD  3   4+
		//		Absolute, Y   SBC $4400, Y  $F9  3   4+
		//		Indirect, X   SBC($44, X)   $E1  2   6
		//		Indirect, Y   SBC($44), Y   $F1  2   5+
		//		+add 1 cycles if page boundary crossed
		//		SBC results are dependant on the setting of the decimal flag.In decimal mode, subtraction is carried out on 
		//		the assumption that the values involved are packed BCD(Binary Coded Decimal).
		//		There is no way to subtract without the carry which works as an inverse borrow.i.e, to subtract you set the 
		//		carry before the operation.If the carry is cleared by the operation, it indicates a borrow occurred.
	private:
		void SBC(byte memory_data);
		void SBC(ushort address);
	public:
		void Immediate(byte memory_data);
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
		void Absolute_Y(ushort address);
		void Indirect_X(ushort address);
		void Indirect_Y(ushort address);
	};

	class _SEC_ : REGISTER_CONTROL, ADDRESS_MODES {
		//								HEX
		// SEC(SEt Carry)               $38
	private:
		void SEC();
	public:
		void Implied();
	};

	class _SED_ : REGISTER_CONTROL, ADDRESS_MODES {
		//								 HEX
		// SED(SEt Decimal)              $F8
	private:
		void SED();
	public:
		void Implied();
	};

	class _SEI_ : REGISTER_CONTROL, ADDRESS_MODES {
		//								 HEX
		// SEI(SEt Interrupt)            $78
	private:
		void SEI();
	public:
		void Implied();
	};

	class _STA_ : REGISTER_CONTROL, ADDRESS_MODES {
		// STA(STore Accumulator)
		//		Affects Flags : none
		//		MODE           SYNTAX         HEX LEN TIM
		//		Zero Page      STA $44        $85  2   3
		//		Zero Page, X   STA $44, X     $95  2   4
		//		Absolute       STA $4400      $8D  3   4
		//		Absolute, X    STA $4400, X   $9D  3   5
		//		Absolute, Y    STA $4400, Y   $99  3   5
		//		Indirect, X    STA($44, X)    $81  2   6
		//		Indirect, Y    STA($44), Y    $91  2   6
	private:
		void STA(ushort address);
	public:
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
		void Absolute_X(ushort address);
		void Absolute_Y(ushort address);
		void Indirect_X(ushort address);
		void Indirect_Y(ushort address);
	};

	class _STX_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void STX(ushort address);
	public:
		void Zero_Page(ushort address);
		void Zero_Page_Y(ushort address);
		void Absolute(ushort address);
	};

	class _STY_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void STY(ushort address);
	public:
		void Zero_Page(ushort address);
		void Zero_Page_X(ushort address);
		void Absolute(ushort address);
	};

	class _TAX_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void TAX();
	public:
		void Implied();
	};

	class _TAY_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void TAY();
	public:
		void Implied();
	};

	class _TSX_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void TSX();
	public:
		void Implied();
	};

	class _TXA_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void TXA();
	public:
		void Implied();
	};

	class _TXS_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void TXS();
	public:
		void Implied();
	};

	class _TYA_ : REGISTER_CONTROL, ADDRESS_MODES {
	private:
		void TYA();
	public:
		void Implied();
	};

	//////////////////////
	// Constants:
	//////////////////////
	// ADC: 0x69; 0x65; 0x75; 0x6d; 0x7d; 0x79; 0x61; 0x71
	static const byte ADC_IMMEDIATE = 0x69;
	static const byte ADC_ZERO_PAGE = 0x65;
	static const byte ADC_ZERO_PAGE_X = 0x75;
	static const byte ADC_ABSOLUTE = 0x6d;
	static const byte ADC_ABSOLUTE_X = 0x7d;
	static const byte ADC_ABSOLUTE_Y = 0x79;
	static const byte ADC_INDIRECT_X = 0x61;
	static const byte ADC_INDIRECT_Y = 0x71;

	// AND: 0x29; 0x25; 0x35; 0x2d; 0x3d; 0x39; 0x21; 0x31
	static const byte AND_IMMEDIATE = 0x29;
	static const byte AND_ZERO_PAGE = 0x25;
	static const byte AND_ZERO_PAGE_X = 0x35;
	static const byte AND_ABSOLUTE = 0x2d;
	static const byte AND_ABSOLUTE_X = 0x3d;
	static const byte AND_ABSOLUTE_Y = 0x39;
	static const byte AND_INDIRECT_X = 0x21;
	static const byte AND_INDIRECT_Y = 0x31;

	// ASL: 0x0a; 0x06; 0x16; 0x0e; 0x1e
	static const byte ASL_ACCUMULATOR = 0x0a;
	static const byte ASL_ZERO_PAGE = 0x06;
	static const byte ASL_ZERO_PAGE_X = 0x16;
	static const byte ASL_ABSOLUTE = 0x0e;
	static const byte ASL_ABSOLUTE_X = 0x1e;

	// BCC: 0x90
	static const byte BCC_RELATIVE = 0x90;

	// BCS: 0xb0
	static const byte BCS_RELATIVE = 0xb0;

	// BEQ: 0xf0
	static const byte BEQ_RELATIVE = 0xf0;

	// BIT: 0x24; 0x2c
	static const byte BIT_ZERO_PAGE = 0x24;
	static const byte BIT_ABSOLUTE = 0x2c;

	// BMI: 0x30
	static const byte BMI_RELATIVE = 0x30;

	// BNE: 0xd0
	static const byte BNE_RELATIVE = 0xd0;

	// BPL: 0x10
	static const byte BPL_RELATIVE = 0x10;

	// BRK: 0x00
	static const byte BRK_IMPLIED = 0x00;

	// BVC: 0x50
	static const byte BVC_RELATIVE = 0x50;

	// BVS: 0x70
	static const byte BVS_RELATIVE = 0x70;

	// CLC: 0x18
	static const byte CLC_IMPLIED = 0x18;

	// CLD: 0xd8
	static const byte CLD_IMPLIED = 0xd8;

	// CLI: 0x58
	static const byte CLI_IMPLIED = 0x58;

	// CLV: 0xb8
	static const byte CLV_IMPLIED = 0xb8;

	// CMP: 0xc9; 0xc5; 0xd5; 0xcd; 0xdd; 0xd9; 0xc1; 0xd1
	static const byte CMP_IMMEDIATE = 0xc9;
	static const byte CMP_ZERO_PAGE = 0xc5;
	static const byte CMP_ZERO_PAGE_X = 0xd5;
	static const byte CMP_ABSOLUTE = 0xcd;
	static const byte CMP_ABSOLUTE_X = 0xdd;
	static const byte CMP_ABSOLUTE_Y = 0xd9;
	static const byte CMP_INDIRECT_X = 0xc1;
	static const byte CMP_INDIRECT_Y = 0xd1;

	// CPX: 0xe0; 0xe4; 0xec
	static const byte CPX_IMMEDIATE = 0xe0;
	static const byte CPX_ZERO_PAGE = 0xe4;
	static const byte CPX_ABSOLUTE = 0xec;

	// CPY: 0xc0; 0xc4; 0xcc
	static const byte CPY_IMMEDIATE = 0xc0;
	static const byte CPY_ZERO_PAGE = 0xc4;
	static const byte CPY_ABSOLUTE = 0xcc;

	// DEC: 0xc6; 0xd6; 0xce; 0xde
	static const byte DEC_ZERO_PAGE = 0xc6;
	static const byte DEC_ZERO_PAGE_X = 0xd6;
	static const byte DEC_ABSOLUTE = 0xce;
	static const byte DEC_ABSOLUTE_X = 0xde;

	// DEX: 0xca;
	static const byte DEX_IMPLIED = 0xca;

	// DEY: 0x88;
	static const byte DEY_IMPLIED = 0x88;

	// EOR: 0x49; 0x45; 0x55; 0x4d; 0x5d; 0x59; 0x41; 0x51
	static const byte EOR_IMMEDIATE = 0x49;
	static const byte EOR_ZERO_PAGE = 0x45;
	static const byte EOR_ZERO_PAGE_X = 0x55;
	static const byte EOR_ABSOLUTE = 0x4d;
	static const byte EOR_ABSOLUTE_X = 0x5d;
	static const byte EOR_ABSOLUTE_Y = 0x59;
	static const byte EOR_INDIRECT_X = 0x41;
	static const byte EOR_INDIRECT_Y = 0x51;

	// INC: 0xe6; 0xf6; 0xee; 0xfe
	static const byte INC_ZERO_PAGE = 0xe6;
	static const byte INC_ZERO_PAGE_X = 0xf6;
	static const byte INC_ABSOLUTE = 0xee;
	static const byte INC_ABSOLUTE_X = 0xfe;

	// INX: 0xe8
	static const byte INX_IMPLIED = 0xe8;

	// INY: 0xc8
	static const byte INY_IMPLIED = 0xc8;

	// JMP: 0x4c; 0x6c
	static const byte JMP_ABSOLUTE = 0x4c;
	static const byte JMP_INDIRECT = 0x6c;

	// JSR: 0x20
	static const byte JSR_ABSOLUTE = 0x20;

	// LDA: 0xa9; 0xa5; 0xb5; 0xad; 0xbd; 0xb9; 0xa1; 0xb1
	static const byte LDA_IMMEDIATE = 0xa9;
	static const byte LDA_ZERO_PAGE = 0xa5;
	static const byte LDA_ZERO_PAGE_X = 0xb5;
	static const byte LDA_ABSOLUTE = 0xad;
	static const byte LDA_ABSOLUTE_X = 0xbd;
	static const byte LDA_ABSOLUTE_Y = 0xb9;
	static const byte LDA_INDIRECT_X = 0xa1;
	static const byte LDA_INDIRECT_Y = 0xb1;

	// LDX: 0xa2; 0xa6; 0xb6; 0xae; 0xbe
	static const byte LDX_IMMEDIATE = 0xa2;
	static const byte LDX_ZERO_PAGE = 0xa6;
	static const byte LDX_ZERO_PAGE_Y = 0xb6;
	static const byte LDX_ABSOLUTE = 0xae;
	static const byte LDX_ABSOLUTE_Y = 0xbe;

	// LDY: 0xa0; 0xa4; 0xb4; 0xac; 0xbc
	static const byte LDY_IMMEDIATE = 0xa0;
	static const byte LDY_ZERO_PAGE = 0xa4;
	static const byte LDY_ZERO_PAGE_X = 0xb4;
	static const byte LDY_ABSOLUTE = 0xac;
	static const byte LDY_ABSOLUTE_X = 0xbc;

	// LSR: 0x4a; 0x46; 0x56; 0x4e; 0x5e
	static const byte LSR_ACCUMULATOR = 0x4a;
	static const byte LSR_ZERO_PAGE = 0x46;
	static const byte LSR_ZERO_PAGE_X = 0x56;
	static const byte LSR_ABSOLUTE = 0x4e;
	static const byte LSR_ABSOLUTE_X = 0x5e;

	// NOP: 0xea
	static const byte NOP_IMPLIED = 0xea;

	// ORA: 0x09; 0x05; 0x15; 0x0d; 0x1d; 0x19; 0x01; 0x11
	static const byte ORA_IMMEDIATE = 0x09;
	static const byte ORA_ZERO_PAGE = 0x05;
	static const byte ORA_ZERO_PAGE_X = 0x15;
	static const byte ORA_ABSOLUTE = 0x0d;
	static const byte ORA_ABSOLUTE_X = 0x1d;
	static const byte ORA_ABSOLUTE_Y = 0x19;
	static const byte ORA_INDIRECT_X = 0x01;
	static const byte ORA_INDIRECT_Y = 0x11;

	// PHA: 0x48
	static const byte PHA_IMPLIED = 0x48;

	// PHP: 0x08
	static const byte PHP_IMPLIED = 0x08;

	// PLA: 0x68
	static const byte PLA_IMPLIED = 0x68;

	// PLP: 0x28
	static const byte PLP_IMPLIED = 0x28;

	// ROL: 0x2a; 0x26; 0x36; 0x2e; 0x3e;
	static const byte ROL_ACCUMULATOR = 0x2a;
	static const byte ROL_ZERO_PAGE = 0x26;
	static const byte ROL_ZERO_PAGE_X = 0x36;
	static const byte ROL_ABSOLUTE = 0x2e;
	static const byte ROL_ABSOLUTE_X = 0x3e;

	// ROR: 0x6a; 0x66; 0x76; 0x6e; 0x7e;
	static const byte ROR_ACCUMULATOR = 0x6a;
	static const byte ROR_ZERO_PAGE = 0x66;
	static const byte ROR_ZERO_PAGE_X = 0x76;
	static const byte ROR_ABSOLUTE = 0x6e;
	static const byte ROR_ABSOLUTE_X = 0x7e;

	// RTI: 0x40
	static const byte RTI_IMPLIED = 0x40;

	// RTS: 0x60
	static const byte RTS_IMPLIED = 0x60;

	// SBC: 0xe9; 0xe5; 0xf5; 0xed; 0xfd; 0xf9; 0xe1; 0xf1
	static const byte SBC_IMMEDIATE = 0xe9;
	static const byte SBC_ZERO_PAGE = 0xe5;
	static const byte SBC_ZERO_PAGE_X = 0xf5;
	static const byte SBC_ABSOLUTE = 0xed;
	static const byte SBC_ABSOLUTE_X = 0xfd;
	static const byte SBC_ABSOLUTE_Y = 0xf9;
	static const byte SBC_INDIRECT_X = 0xe1;
	static const byte SBC_INDIRECT_Y = 0xf1;

	// SEC: 0x38
	static const byte SEC_IMPLIED = 0x38;

	// SED: 0xf8
	static const byte SED_IMPLIED = 0xf8;

	// SEI: 0x78
	static const byte SEI_IMPLIED = 0x78;

	// STA: 0x85; 0x95; 0x8d; 0x9d; 0x99; 0x81; 0x91
	static const byte STA_ZERO_PAGE = 0x85;
	static const byte STA_ZERO_PAGE_X = 0x95;
	static const byte STA_ABSOLUTE = 0x8d;
	static const byte STA_ABSOLUTE_X = 0x9d;
	static const byte STA_ABSOLUTE_Y = 0x99;
	static const byte STA_INDIRECT_X = 0x81;
	static const byte STA_INDIRECT_Y = 0x91;

	// STX: 0x86; 0x96; 0x8e
	static const byte STX_ZERO_PAGE = 0x86;
	static const byte STX_ZERO_PAGE_Y = 0x96;
	static const byte STX_ABSOLUTE = 0x8e;

	// STY: 0x84; 0x94; 0x8c
	static const byte STY_ZERO_PAGE = 0x84;
	static const byte STY_ZERO_PAGE_X = 0x94;
	static const byte STY_ABSOLUTE = 0x8c;

	// TAX: 0xaa
	static const byte TAX_IMPLIED = 0xaa;

	// TAY: 0xa8
	static const byte TAY_IMPLIED = 0xa8;

	// TSX: 0xba
	static const byte TSX_IMPLIED = 0xba;

	// TXA: 0x8a
	static const byte TXA_IMPLIED = 0x8a;

	// TXS: 0x9a
	static const byte TXS_IMPLIED = 0x9a;

	// TYA: 0x98
	static const byte TYA_IMPLIED = 0x98;

	const byte ADDRESS_ACCUMULATOR = 0;
	const byte ADDRESS_IMMEDIATE = 1;
	const byte ADDRESS_ZERO_PAGE = 2;
	const byte ADDRESS_ZERO_PAGE_X = 3;
	const byte ADDRESS_ZERO_PAGE_Y = 4;
	const byte ADDRESS_ABSOLUTE = 5;
	const byte ADDRESS_ABSOLUTE_X = 6;
	const byte ADDRESS_ABSOLUTE_Y = 7;
	const byte ADDRESS_IMPLIED = 8;
	const byte ADDRESS_RELATIVE = 9;
	const byte ADDRESS_INDIRECT_X = 10;
	const byte ADDRESS_INDIRECT_Y = 11;
	const byte ADDRESS_INDIRECT = 12;

	//////////////////////
	// Function Prototypes:
	//////////////////////
	void Init();
	static void Push8(byte data);
	static void Push16(ushort data);
	static byte Pull8();

	//////////////////////
	// Variables:
	//////////////////////
	static byte cycle_list[0x100];
	static byte memory_data;
	static ushort address;
	static byte opcode;

	class INSTRUCTION_SET {
	public:
		//////////////////////
		// Function Prototypes:
		//////////////////////
		_ADC_ ADC;
		_AND_ AND;
		_ASL_ ASL;
		_BCC_ BCC;
		_BCS_ BCS;
		_BEQ_ BEQ;
		_BIT_ BIT;
		_BMI_ BMI;
		_BNE_ BNE;
		_BPL_ BPL;

		_BRK_ BRK;
		_BVC_ BVC;
		_BVS_ BVS;
		_CLC_ CLC;
		_CLD_ CLD;
		_CLI_ CLI;
		_CLV_ CLV;
		_CMP_ CMP;
		_CPX_ CPX;
		_CPY_ CPY;

		_DEC_ DEC;
		_DEX_ DEX;
		_DEY_ DEY;
		_EOR_ EOR;
		_INC_ INC;
		_INX_ INX;
		_INY_ INY;
		_JMP_ JMP;
		_JSR_ JSR;
		_LDA_ LDA;

		_LDX_ LDX;
		_LDY_ LDY;
		_LSR_ LSR;
		_NOP_ NOP;
		_ORA_ ORA;
		_PHA_ PHA;
		_PHP_ PHP;
		_PLA_ PLA;
		_PLP_ PLP;
		_ROL_ ROL;

		_ROR_ ROR;
		_RTI_ RTI;
		_RTS_ RTS;
		_SBC_ SBC;
		_SEC_ SEC;
		_SED_ SED;
		_SEI_ SEI;
		_STA_ STA;
		_STX_ STX;
		_STY_ STY;

		_TAX_ TAX;
		_TAY_ TAY;
		_TSX_ TSX;
		_TXA_ TXA;
		_TXS_ TXS;
		_TYA_ TYA;
	};

	class INTERRUPT : public REGISTER_CONTROL {
	public:
		//////////////////////
		// Function Prototypes:
		//////////////////////
		void RESET();
		void NMI();
		void IRQ();
	};

	void Fire_Opcode(byte opcode);
	void Fire_Scanline();

public:
	//////////////////////
	// Constructor:
	//////////////////////
	MOS6502();

	//////////////////////
	// Function Prototypes:
	//////////////////////
	void Execute();

	//////////////////////
	// Constants:
	//////////////////////
	// See http://wiki.nesdev.com/w/index.php/Overscan for extra details
	// on top of http://wiki.nesdev.com/w/index.php/Cycle_reference_chart

	const char *NTSC_CPU_NAME = "Ricoh 2A03";
	const float NTSC_MASTER_CLOCK_SPEED = 21.477272f; // MHz	21.477272 MHz ± 40 Hz	236.25 MHz ÷ 11 by definition
	const float NTSC_CPU_CLOCK_SPEED = 1.789773f; // MHz	21.47~ MHz ÷ 12 machine clock counts per cpu cycle = 1.789773 MHz
	const double NTSC_CPU_CYCLES_PER_SCANLINE = 113.666666666667;
	const float NTSC_CPU_CYCLES_PER_FRAME = 29780.5; // 89341.5 ÷ 3 = 29780.5 (Total number of pixels per frame / PPU pixels per CPU cycle)
	const byte NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE = 12;
	const byte NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_PPU_CYCLE = 4;
	const ushort NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_SCANLINE = static_cast<ushort>(NTSC_CPU_CYCLES_PER_SCANLINE * static_cast<float>(NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE));

	const char *PAL_CPU_NAME = "Ricoh 2A07";
	const float PAL_MASTER_CLOCK_SPEED = 26.601712f; // MHz		26.601712 MHz ± 50 Hz	26.6017125 MHz by definition
	const float PAL_CPU_CLOCK_SPEED = 1.662607f; // MHz		26.60~ MHz ÷ 16 machine clock counts per cpu cycle = 1.662607 MHz
	const double PAL_CPU_CYCLES_PER_SCANLINE = 106.5625000000;
	const float PAL_CPU_CYCLES_PER_FRAME = 33247.5; // 	106392 ÷ 3.2 = 33247.5 (Total number of pixels per frame / PPU pixels per CPU cycle)
	const byte PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE = 16;
	const byte PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_PPU_CYCLE = 5;
	const ushort PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_SCANLINE = static_cast<ushort>(PAL_CPU_CYCLES_PER_SCANLINE * static_cast<float>(PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE));

	//////////////////////
	// Variables:
	//////////////////////
	REGISTERS reg;
	INSTRUCTION_SET instruction;
	INTERRUPT interrupt;
	static REGION region;
	static bool emulated;
	static bool paused;
	static bool nes_cpu;
	static ushort cycles; // Total cycles per frame. 12 machine cycles per cpu cycle (16 with PAL). 
					      //Runs at 1.789773 MHz (1.662607 MHz PAL)
	static int machine_cycles; // Runs at 21.477272 MHz (26.601712 PAL)
	static ushort instruction_cycles; // The cycles burned by the current instruction plus the page boundary penalty.
	static ushort instruction_machine_cycles; // The machine cycles burned by the current instruction plus the page boundary penalty.
	static int temp_machine_cycles;
	static int machine_cycles_on_current_scanline;
	static ushort cpu_cycles_on_current_scanline;
	
};

extern MOS6502 cpu;