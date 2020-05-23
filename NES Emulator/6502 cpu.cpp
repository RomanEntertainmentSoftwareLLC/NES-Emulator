#include "6502 cpu.h"
#include "6502 cpu memory.h"

MOS6502 cpu;

byte MOS6502::REGISTERS::a;
byte MOS6502::REGISTERS::x;
byte MOS6502::REGISTERS::y;
byte MOS6502::REGISTERS::sp;
byte MOS6502::REGISTERS::sr;
ushort MOS6502::REGISTERS::pc;

byte MOS6502::memory_data;
ushort MOS6502::address;

MOS6502::REGION MOS6502::region;
byte MOS6502::cycle_list[0x100];
ushort MOS6502::cycles;
int MOS6502::machine_cycles;
ushort MOS6502::instruction_cycles;
ushort MOS6502::instruction_machine_cycles;
int MOS6502::temp_machine_cycles;
int MOS6502::machine_cycles_on_current_scanline;
ushort MOS6502::cpu_cycles_on_current_scanline;
byte MOS6502::opcode;
bool MOS6502::emulated;
bool MOS6502::paused;
bool MOS6502::nes_cpu;

MOS6502::MOS6502() {
	emulated = true;
	nes_cpu = true;
	Init();
	interrupt.RESET();
}

void MOS6502::Init() {
	cycle_list[ADC_IMMEDIATE] = 2;
	cycle_list[ADC_ZERO_PAGE] = 3;
	cycle_list[ADC_ZERO_PAGE_X] = 4;
	cycle_list[ADC_ABSOLUTE] = 4;
	cycle_list[ADC_ABSOLUTE_X] = 4;
	cycle_list[ADC_ABSOLUTE_Y] = 4;
	cycle_list[ADC_INDIRECT_X] = 6;
	cycle_list[ADC_INDIRECT_Y] = 5;

	cycle_list[AND_IMMEDIATE] = 2;
	cycle_list[AND_ZERO_PAGE] = 3;
	cycle_list[AND_ZERO_PAGE_X] = 4;
	cycle_list[AND_ABSOLUTE] = 4;
	cycle_list[AND_ABSOLUTE_X] = 4;
	cycle_list[AND_ABSOLUTE_Y] = 4;
	cycle_list[AND_INDIRECT_X] = 6;
	cycle_list[AND_INDIRECT_Y] = 5;

	cycle_list[ASL_ACCUMULATOR] = 2;
	cycle_list[ASL_ZERO_PAGE] = 5;
	cycle_list[ASL_ZERO_PAGE_X] = 6;
	cycle_list[ASL_ABSOLUTE] = 6;
	cycle_list[ASL_ABSOLUTE_X] = 7;

	cycle_list[BCC_RELATIVE] = 2;

	cycle_list[BCS_RELATIVE] = 2;

	cycle_list[BEQ_RELATIVE] = 2;

	cycle_list[BIT_ZERO_PAGE] = 3;
	cycle_list[BIT_ABSOLUTE] = 4;

	cycle_list[BMI_RELATIVE] = 2;

	cycle_list[BNE_RELATIVE] = 2;

	cycle_list[BPL_RELATIVE] = 2;

	cycle_list[BRK_IMPLIED] = 7;

	cycle_list[BVC_RELATIVE] = 2;

	cycle_list[BVS_RELATIVE] = 2;

	cycle_list[CLC_IMPLIED] = 2;

	cycle_list[CLD_IMPLIED] = 2;

	cycle_list[CLI_IMPLIED] = 2;

	cycle_list[CLV_IMPLIED] = 2;

	cycle_list[CMP_IMMEDIATE] = 2;
	cycle_list[CMP_ZERO_PAGE] = 3;
	cycle_list[CMP_ZERO_PAGE_X] = 4;
	cycle_list[CMP_ABSOLUTE] = 4;
	cycle_list[CMP_ABSOLUTE_X] = 4;
	cycle_list[CMP_ABSOLUTE_Y] = 4;
	cycle_list[CMP_INDIRECT_X] = 6;
	cycle_list[CMP_INDIRECT_Y] = 5;

	cycle_list[CPX_IMMEDIATE] = 2;
	cycle_list[CPX_ZERO_PAGE] = 3;
	cycle_list[CPX_ABSOLUTE] = 4;

	cycle_list[CPY_IMMEDIATE] = 2;
	cycle_list[CPY_ZERO_PAGE] = 3;
	cycle_list[CPY_ABSOLUTE] = 4;

	cycle_list[DEC_ZERO_PAGE] = 5;
	cycle_list[DEC_ZERO_PAGE_X] = 6;
	cycle_list[DEC_ABSOLUTE] = 6;
	cycle_list[DEC_ABSOLUTE_X] = 7;

	cycle_list[DEX_IMPLIED] = 2;

	cycle_list[DEY_IMPLIED] = 2;

	cycle_list[EOR_IMMEDIATE] = 2;
	cycle_list[EOR_ZERO_PAGE] = 3;
	cycle_list[EOR_ZERO_PAGE_X] = 4;
	cycle_list[EOR_ABSOLUTE] = 4;
	cycle_list[EOR_ABSOLUTE_X] = 4;
	cycle_list[EOR_ABSOLUTE_Y] = 4;
	cycle_list[EOR_INDIRECT_X] = 6;
	cycle_list[EOR_INDIRECT_Y] = 5;

	cycle_list[INC_ZERO_PAGE] = 5;
	cycle_list[INC_ZERO_PAGE_X] = 6;
	cycle_list[INC_ABSOLUTE] = 6;
	cycle_list[INC_ABSOLUTE_X] = 7;

	cycle_list[INX_IMPLIED] = 2;

	cycle_list[INY_IMPLIED] = 2;

	cycle_list[JMP_ABSOLUTE] = 3;
	cycle_list[JMP_INDIRECT] = 5;

	cycle_list[JSR_ABSOLUTE] = 6;

	cycle_list[LDA_IMMEDIATE] = 2;
	cycle_list[LDA_ZERO_PAGE] = 3;
	cycle_list[LDA_ZERO_PAGE_X] = 4;
	cycle_list[LDA_ABSOLUTE] = 4;
	cycle_list[LDA_ABSOLUTE_X] = 4;
	cycle_list[LDA_ABSOLUTE_Y] = 4;
	cycle_list[LDA_INDIRECT_X] = 6;
	cycle_list[LDA_INDIRECT_Y] = 5;

	cycle_list[LDX_IMMEDIATE] = 2;
	cycle_list[LDX_ZERO_PAGE] = 3;
	cycle_list[LDX_ZERO_PAGE_Y] = 4;
	cycle_list[LDX_ABSOLUTE] = 4;
	cycle_list[LDX_ABSOLUTE_Y] = 4;

	cycle_list[LDY_IMMEDIATE] = 2;
	cycle_list[LDY_ZERO_PAGE] = 3;
	cycle_list[LDY_ZERO_PAGE_X] = 4;
	cycle_list[LDY_ABSOLUTE] = 4;
	cycle_list[LDY_ABSOLUTE_X] = 4;

	cycle_list[LSR_ACCUMULATOR] = 2;
	cycle_list[LSR_ZERO_PAGE] = 5;
	cycle_list[LSR_ZERO_PAGE_X] = 6;
	cycle_list[LSR_ABSOLUTE] = 6;
	cycle_list[LSR_ABSOLUTE_X] = 7;

	cycle_list[NOP_IMPLIED] = 2;

	cycle_list[ORA_IMMEDIATE] = 2;
	cycle_list[ORA_ZERO_PAGE] = 3;
	cycle_list[ORA_ZERO_PAGE_X] = 4;
	cycle_list[ORA_ABSOLUTE] = 4;
	cycle_list[ORA_ABSOLUTE_X] = 4;
	cycle_list[ORA_ABSOLUTE_Y] = 4;
	cycle_list[ORA_INDIRECT_X] = 6;
	cycle_list[ORA_INDIRECT_Y] = 5;

	cycle_list[PHA_IMPLIED] = 3;

	cycle_list[PHP_IMPLIED] = 3;

	cycle_list[PLA_IMPLIED] = 4;

	cycle_list[PLP_IMPLIED] = 4;

	cycle_list[ROL_ACCUMULATOR] = 2;
	cycle_list[ROL_ZERO_PAGE] = 5;
	cycle_list[ROL_ZERO_PAGE_X] = 6;
	cycle_list[ROL_ABSOLUTE] = 6;
	cycle_list[ROL_ABSOLUTE_X] = 7;

	cycle_list[ROR_ACCUMULATOR] = 2;
	cycle_list[ROR_ZERO_PAGE] = 5;
	cycle_list[ROR_ZERO_PAGE_X] = 6;
	cycle_list[ROR_ABSOLUTE] = 6;
	cycle_list[ROR_ABSOLUTE_X] = 7;

	cycle_list[RTI_IMPLIED] = 6;

	cycle_list[RTS_IMPLIED] = 6;

	cycle_list[SBC_IMMEDIATE] = 2;
	cycle_list[SBC_ZERO_PAGE] = 3;
	cycle_list[SBC_ZERO_PAGE_X] = 4;
	cycle_list[SBC_ABSOLUTE] = 4;
	cycle_list[SBC_ABSOLUTE_X] = 4;
	cycle_list[SBC_ABSOLUTE_Y] = 4;
	cycle_list[SBC_INDIRECT_X] = 6;
	cycle_list[SBC_INDIRECT_Y] = 5;

	cycle_list[SEC_IMPLIED] = 2;

	cycle_list[SED_IMPLIED] = 2;

	cycle_list[SEI_IMPLIED] = 2;

	cycle_list[STA_ZERO_PAGE] = 3;
	cycle_list[STA_ZERO_PAGE_X] = 4;
	cycle_list[STA_ABSOLUTE] = 4;
	cycle_list[STA_ABSOLUTE_X] = 5;
	cycle_list[STA_ABSOLUTE_Y] = 5;
	cycle_list[STA_INDIRECT_X] = 6;
	cycle_list[STA_INDIRECT_Y] = 6;

	cycle_list[STX_ZERO_PAGE] = 3;
	cycle_list[STX_ZERO_PAGE_Y] = 4;
	cycle_list[STX_ABSOLUTE] = 4;

	cycle_list[STY_ZERO_PAGE] = 3;
	cycle_list[STY_ZERO_PAGE_X] = 4;
	cycle_list[STY_ABSOLUTE] = 4;

	cycle_list[TAX_IMPLIED] = 2;

	cycle_list[TAY_IMPLIED] = 2;

	cycle_list[TSX_IMPLIED] = 2;

	cycle_list[TXA_IMPLIED] = 2;

	cycle_list[TXS_IMPLIED] = 2;

	cycle_list[TYA_IMPLIED] = 2;
}

void MOS6502::REGISTER_CONTROL::Set_Carry() {
	MOS6502::REGISTERS::sr |= CARRY_REGISTER;
}

void MOS6502::REGISTER_CONTROL::Clear_Carry() {
	MOS6502::REGISTERS::sr &= ~CARRY_REGISTER;
}

bool MOS6502::REGISTER_CONTROL::Is_Carry() {
	if (MOS6502::REGISTERS::sr & CARRY_REGISTER)
		return true;
	else
		return false;
}

void MOS6502::REGISTER_CONTROL::Set_Zero() {
	MOS6502::REGISTERS::sr |= ZERO_REGISTER;
}

void MOS6502::REGISTER_CONTROL::Clear_Zero() {
	MOS6502::REGISTERS::sr &= ~ZERO_REGISTER;
}

bool MOS6502::REGISTER_CONTROL::Is_Zero() {
	if (MOS6502::REGISTERS::sr & ZERO_REGISTER)
		return true;
	else
		return false;
}

void MOS6502::REGISTER_CONTROL::Set_Interrupt() {
	MOS6502::REGISTERS::sr |= INTERRUPT_REGISTER;
}

void MOS6502::REGISTER_CONTROL::Clear_Interrupt() {
	MOS6502::REGISTERS::sr &= ~INTERRUPT_REGISTER;
}

bool MOS6502::REGISTER_CONTROL::Is_Interrupt() {
	if (MOS6502::REGISTERS::sr & INTERRUPT_REGISTER)
		return true;
	else
		return false;
}

void MOS6502::REGISTER_CONTROL::Set_Decimal() {
	MOS6502::REGISTERS::sr |= DECIMAL_REGISTER;
}

void MOS6502::REGISTER_CONTROL::Clear_Decimal() {
	MOS6502::REGISTERS::sr &= ~DECIMAL_REGISTER;
}

bool MOS6502::REGISTER_CONTROL::Is_Decimal() {
	if (MOS6502::REGISTERS::sr & DECIMAL_REGISTER)
		return true;
	else
		return false;
}

void MOS6502::REGISTER_CONTROL::Set_Break() {
	MOS6502::REGISTERS::sr |= BREAK_REGISTER;
}

void MOS6502::REGISTER_CONTROL::Clear_Break() {
	MOS6502::REGISTERS::sr &= ~BREAK_REGISTER;
}

bool MOS6502::REGISTER_CONTROL::Is_Break() {
	if (MOS6502::REGISTERS::sr & BREAK_REGISTER)
		return true;
	else
		return false;
}

void MOS6502::REGISTER_CONTROL::Set_Overflow() {
	MOS6502::REGISTERS::sr |= OVERFLOW_REGISTER;
}

void MOS6502::REGISTER_CONTROL::Clear_Overflow() {
	MOS6502::REGISTERS::sr &= ~OVERFLOW_REGISTER;
}

bool MOS6502::REGISTER_CONTROL::Is_Overflow() {
	if (MOS6502::REGISTERS::sr & OVERFLOW_REGISTER)
		return true;
	else
		return false;
}

void MOS6502::REGISTER_CONTROL::Set_Sign() {
	MOS6502::REGISTERS::sr |= SIGN_REGISTER;
}

void MOS6502::REGISTER_CONTROL::Clear_Sign() {
	MOS6502::REGISTERS::sr &= ~SIGN_REGISTER;
}

bool MOS6502::REGISTER_CONTROL::Is_Sign() {
	if (MOS6502::REGISTERS::sr & SIGN_REGISTER)
		return true;
	else
		return false;
}

void MOS6502::_ADC_::ADC(byte memory_data) {
	ushort sum;

	if (emulated == true)
		memory_data = Read_Memory(address);

	MOS6502::memory_data = memory_data;
	byte carry = REGISTERS::sr & CARRY_REGISTER ? 1 : 0;

	if (Is_Decimal() == true && nes_cpu == false) {
		sum = (REGISTERS::a & 0xf) + (memory_data & 0xf) + carry;

		if (sum >= 0xa)
			sum = 0x10 | ((sum + 0x6) & 0xf);

		sum += (REGISTERS::a & 0xf0) + (memory_data & 0xf0);

		if (sum >= 0xa0) {
			Set_Carry();
			sum += 0x60;
		}
		else
			Clear_Carry();
	}
	else {
		sum = REGISTERS::a + memory_data + carry;

		if (sum >= 0x100)
			Set_Carry();
		else
			Clear_Carry();
	}

	// V Overflow Flag Set if sign bit is incorrect
	if (((REGISTERS::a ^ sum) & 0x80) && !((REGISTERS::a ^ memory_data) & 0x80))
		Set_Overflow();
	else
		Clear_Overflow();

	REGISTERS::a = sum & 0xff;

	// Z Zero Flag Set if A = 0
	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	// N Negative Flag Set if bit 7 set
	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_ADC_::ADC(ushort address) {
	ushort sum;
	memory_data = Read_Memory(address);
	MOS6502::address = address;
	byte carry = REGISTERS::sr & CARRY_REGISTER ? 1 : 0;

	if (Is_Decimal() == true && nes_cpu == false) {
		sum = (REGISTERS::a & 0xf) + (memory_data & 0xf) + carry;

		if (sum >= 0xa)
			sum = 0x10 | ((sum + 0x6) & 0xf);

		sum += (REGISTERS::a & 0xf0) + (memory_data & 0xf0);

		if (sum >= 0xa0) {
			Set_Carry();
			sum += 0x60;
		}
		else
			Clear_Carry();
	}
	else {
		sum = REGISTERS::a + memory_data + carry;

		if (sum >= 0x100)
			Set_Carry();
		else
			Clear_Carry();
	}

	// V Overflow Flag Set if sign bit is incorrect
	if (((REGISTERS::a ^ sum) & 0x80) && !((REGISTERS::a ^ memory_data) & 0x80))
		Set_Overflow();
	else
		Clear_Overflow();

	REGISTERS::a = sum & 0xff;

	// Z Zero Flag Set if A = 0
	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	// N Negative Flag Set if bit 7 set
	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_ADC_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	ADC(memory_data);
	instruction_cycles = cycle_list[ADC_IMMEDIATE];
}	

void MOS6502::_ADC_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	ADC(address);
	instruction_cycles = cycle_list[ADC_ZERO_PAGE];
}

void MOS6502::_ADC_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	ADC(address);
	instruction_cycles = cycle_list[ADC_ZERO_PAGE_X];
}

void MOS6502::_ADC_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	ADC(address);
	instruction_cycles = cycle_list[ADC_ABSOLUTE];
}

void MOS6502::_ADC_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	ADC(address);
	instruction_cycles = cycle_list[ADC_ABSOLUTE_X];
}

void MOS6502::_ADC_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	ADC(address);
	instruction_cycles = cycle_list[ADC_ABSOLUTE_Y];
}

void MOS6502::_ADC_::Indirect_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Indirect_X(address);
	ADC(address);
	instruction_cycles = cycle_list[ADC_INDIRECT_X];
}

void MOS6502::_ADC_::Indirect_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	instruction_cycles = 0;
	ADDRESS_MODES::Indirect_Y(address);
	ADC(address);
	instruction_cycles = cycle_list[ADC_INDIRECT_Y];
}

void MOS6502::_AND_::AND(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	REGISTERS::a &= memory_data;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_AND_::AND(ushort address) {
	memory_data = Read_Memory(address);
	REGISTERS::a &= memory_data;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_AND_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	AND(memory_data);
	instruction_cycles = cycle_list[AND_IMMEDIATE];
}

void MOS6502::_AND_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	AND(address);
	instruction_cycles = cycle_list[AND_ZERO_PAGE];
}

void MOS6502::_AND_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	AND(address);
	instruction_cycles = cycle_list[AND_ZERO_PAGE_X];
}

void MOS6502::_AND_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	AND(address);
	instruction_cycles = cycle_list[AND_ABSOLUTE];
}

void MOS6502::_AND_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	AND(address);
	instruction_cycles = cycle_list[AND_ABSOLUTE_X];
}

void MOS6502::_AND_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	AND(address);
	instruction_cycles = cycle_list[AND_ABSOLUTE_Y];
}

void MOS6502::_AND_::Indirect_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Indirect_X(address);
	AND(address);
	instruction_cycles = cycle_list[AND_INDIRECT_X];
}

void MOS6502::_AND_::Indirect_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	instruction_cycles = 0;
	ADDRESS_MODES::Indirect_Y(address);
	AND(address);
	instruction_cycles = cycle_list[AND_INDIRECT_Y];
}

void MOS6502::_ASL_::ASL() {
	ushort result = REGISTERS::a << 1;

	if ((REGISTERS::a & BIT_7) == BIT_7)
		Set_Carry();
	else
		Clear_Carry();

	REGISTERS::a = static_cast<byte>(result & 0xff);

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_ASL_::ASL(ushort address) {
	memory_data = Read_Memory(address);
	ushort result = static_cast<ushort>(memory_data << 1);

	if ((memory_data & BIT_7) == BIT_7)
		Set_Carry();
	else
		Clear_Carry();

	if (result == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((result & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	Write_Memory(address, static_cast<byte>(result));
}

void MOS6502::_ASL_::Accumulator() {
	REGISTERS::pc++;
	ADDRESS_MODES::Accumulator();
	ASL();
	instruction_cycles = cycle_list[ASL_ACCUMULATOR];
}

void MOS6502::_ASL_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	ASL(address);
	instruction_cycles = cycle_list[ASL_ZERO_PAGE];
}

void MOS6502::_ASL_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	ASL(address);
	instruction_cycles = cycle_list[ASL_ZERO_PAGE_X];
}

void MOS6502::_ASL_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	ASL(address);
	instruction_cycles = cycle_list[ASL_ABSOLUTE];
}

void MOS6502::_ASL_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	ASL(address);
	instruction_cycles = cycle_list[ASL_ABSOLUTE_X];
}

void MOS6502::_BCC_::BCC(ushort address) {
	if (Is_Carry() == false) {
		ushort old_pc = REGISTERS::pc;
		REGISTERS::pc += address;

		if ((old_pc & 0xff00) != (REGISTERS::pc & 0xff00)) {
			cycles += 2; // Add a one cycles penalty for crossing page boundary and add another cycles penalty for branch set.
			cpu_cycles_on_current_scanline += 2;
			machine_cycles += 2 * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else {
			cycles++; // Add a one cycles penalty for branch set.
			cpu_cycles_on_current_scanline++;
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}

	}
	else
		REGISTERS::pc++;
}

void MOS6502::_BCC_::Relative(ushort address) {
	REGISTERS::pc++;

	if (Is_Carry() == false)
		ADDRESS_MODES::Relative(address);

	BCC(address);
	instruction_cycles = cycle_list[BCC_RELATIVE];
}

void MOS6502::_BCS_::BCS(ushort address) {
	if (Is_Carry() == true) {
		ushort old_pc = REGISTERS::pc;
		REGISTERS::pc += address;

		if ((old_pc & 0xff00) != (REGISTERS::pc & 0xff00)) {
			cycles += 2; // Add a one cycles penalty for crossing page boundary and add another cycles penalty for branch set.
			cpu_cycles_on_current_scanline += 2;
			machine_cycles += 2 * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else {
			cycles++; // Add a one cycles penalty for branch set.
			cpu_cycles_on_current_scanline++;
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
			
	}
	else
		REGISTERS::pc++;
}

void MOS6502::_BCS_::Relative(ushort address) {
	REGISTERS::pc++;

	if (Is_Carry() == true)
		ADDRESS_MODES::Relative(address);

	BCS(address);
	instruction_cycles = cycle_list[BCS_RELATIVE];
}

void MOS6502::_BEQ_::BEQ(ushort address) {
	if (Is_Zero() == true) {
		ushort old_pc = REGISTERS::pc;
		REGISTERS::pc += address;

		if ((old_pc & 0xff00) != (REGISTERS::pc & 0xff00)) {
			cycles += 2; // Add a one cycles penalty for crossing page boundary and add another cycles penalty for branch set.
			cpu_cycles_on_current_scanline += 2;
			machine_cycles += 2 * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else {
			cycles++; // Add a one cycles penalty for branch set.
			cpu_cycles_on_current_scanline++;
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}

	}
	else
		REGISTERS::pc++;
}

void MOS6502::_BEQ_::Relative(ushort address) {
	REGISTERS::pc++;

	if (Is_Zero() == true)
		ADDRESS_MODES::Relative(address);

	BEQ(address);
	instruction_cycles = cycle_list[BEQ_RELATIVE];
}

void MOS6502::_BIT_::BIT(ushort address) {
	memory_data = Read_Memory(address);

	if ((REGISTERS::a & memory_data) == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	if ((memory_data & OVERFLOW_REGISTER) == OVERFLOW_REGISTER)
		Set_Overflow();
	else
		Clear_Overflow();
}

void MOS6502::_BIT_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	BIT(address);
	instruction_cycles = cycle_list[BIT_ZERO_PAGE];
}

void MOS6502::_BIT_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	BIT(address);
	instruction_cycles = cycle_list[BIT_ABSOLUTE];
}

void MOS6502::_BMI_::BMI(ushort address) {
	if (Is_Sign() == true) {
		ushort old_pc = REGISTERS::pc;
		REGISTERS::pc += address;

		if ((old_pc & 0xff00) != (REGISTERS::pc & 0xff00)) {
			cycles += 2; // Add a one cycles penalty for crossing page boundary and add another cycles penalty for branch set.
			cpu_cycles_on_current_scanline += 2;
			machine_cycles += 2 * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else {
			cycles++; // Add a one cycles penalty for branch set.
			cpu_cycles_on_current_scanline++;
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
			
	}
	else
		REGISTERS::pc++;
}

void MOS6502::_BMI_::Relative(ushort address) {
	REGISTERS::pc++;

	if (Is_Sign() == true)
		ADDRESS_MODES::Relative(address);

	BMI(address);
	instruction_cycles = cycle_list[BMI_RELATIVE];
}

void MOS6502::_BNE_::BNE(ushort address) {
	if (Is_Zero() == false) {
		ushort old_pc = REGISTERS::pc;
		REGISTERS::pc += address;

		if ((old_pc & 0xff00) != (REGISTERS::pc & 0xff00)) {
			cycles += 2; // Add a one cycles penalty for crossing page boundary and add another cycles penalty for branch set.
			cpu_cycles_on_current_scanline += 2;
			machine_cycles += 2 * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else {
			cycles++; // Add a one cycles penalty for branch set.
			cpu_cycles_on_current_scanline++;
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
			
	}
	else
		REGISTERS::pc++;
}

void MOS6502::_BNE_::Relative(ushort address) {
	REGISTERS::pc++;

	if (Is_Zero() == false)
		ADDRESS_MODES::Relative(address);

	BNE(address);
	instruction_cycles = cycle_list[BNE_RELATIVE];
}

void MOS6502::_BPL_::BPL(ushort address) {
	if (Is_Sign() == false) {
		ushort old_pc = REGISTERS::pc;
		REGISTERS::pc += address;

		if ((old_pc & 0xff00) != (REGISTERS::pc & 0xff00)) {
			cycles += 2; // Add a one cycles penalty for crossing page boundary and add another cycles penalty for branch set.
			cpu_cycles_on_current_scanline += 2;
			machine_cycles += 2 * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else {
			cycles++; // Add a one cycles penalty for branch set.
			cpu_cycles_on_current_scanline++;
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
			
	}
	else
		REGISTERS::pc++;
}

void MOS6502::_BPL_::Relative(ushort address) {
	REGISTERS::pc++;

	if (Is_Sign() == false)
		ADDRESS_MODES::Relative(address);

	BPL(address);
	instruction_cycles = cycle_list[BPL_RELATIVE];
}

void MOS6502::_BRK_::BRK() {
	REGISTERS::pc++;
	Push16(REGISTERS::pc);
	Push8(REGISTERS::sr | BREAK_REGISTER);
	Set_Interrupt();
	REGISTERS::pc = Read_Memory(0xFFFE) | (Read_Memory(0xFFFF) << 8);
}

void MOS6502::_BRK_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	BRK();
	instruction_cycles = cycle_list[BRK_IMPLIED];
}

void MOS6502::_BVC_::BVC(ushort address) {
	if (Is_Overflow() == false) {
		ushort old_pc = REGISTERS::pc;
		REGISTERS::pc += address;

		if ((old_pc & 0xff00) != (REGISTERS::pc & 0xff00)) {
			cycles += 2; // Add a one cycles penalty for crossing page boundary and add another cycles penalty for branch set.
			cpu_cycles_on_current_scanline += 2;
			machine_cycles += 2 * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else {
			cycles++; // Add a one cycles penalty for branch set.
			cpu_cycles_on_current_scanline++;
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
			
	}
	else
		REGISTERS::pc++;
}

void MOS6502::_BVC_::Relative(ushort address) {
	REGISTERS::pc++;

	if (Is_Overflow() == false)
		ADDRESS_MODES::Relative(address);

	BVC(address);
	instruction_cycles = cycle_list[BVC_RELATIVE];
}

void MOS6502::_BVS_::BVS(ushort address) {
	if (Is_Overflow() == true) {
		ushort old_pc = REGISTERS::pc;
		REGISTERS::pc += address;

		if ((old_pc & 0xff00) != (REGISTERS::pc & 0xff00)) {
			cycles += 2; // Add a one cycles penalty for crossing page boundary and add another cycles penalty for branch set.
			cpu_cycles_on_current_scanline += 2;
			machine_cycles += 2 * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else {
			cycles++; // Add a one cycles penalty for branch set.
			cpu_cycles_on_current_scanline++;
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
			
	}
	else
		REGISTERS::pc++;
}

void MOS6502::_BVS_::Relative(ushort address) {
	REGISTERS::pc++;

	if (Is_Overflow() == true)
		ADDRESS_MODES::Relative(address);

	BVS(address);
	instruction_cycles = cycle_list[BVS_RELATIVE];
}

void MOS6502::_CLC_::CLC() {
	Clear_Carry();
}

void MOS6502::_CLC_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	CLC();
	instruction_cycles = cycle_list[CLC_IMPLIED];
}

void MOS6502::_CLD_::CLD() {
	Clear_Decimal();
}

void MOS6502::_CLD_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	CLD();
	instruction_cycles = cycle_list[CLD_IMPLIED];
}

void MOS6502::_CLI_::CLI() {
	Clear_Interrupt();
}

void MOS6502::_CLI_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	CLI();
	instruction_cycles = cycle_list[CLI_IMPLIED];
}

void MOS6502::_CLV_::CLV() {
	Clear_Overflow();
}

void MOS6502::_CLV_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	CLV();
	instruction_cycles = cycle_list[CLV_IMPLIED];
}

void MOS6502::_CMP_::CMP(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	ushort result = static_cast<ushort>((REGISTERS::a + BIT_8) - memory_data);

	if (result > 0xff)
		Set_Carry();
	else
		Clear_Carry();

	if ((REGISTERS::a - memory_data) == 0)
		Set_Zero();
	else
		Clear_Zero();

	if (((REGISTERS::a - memory_data) & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_CMP_::CMP(ushort address) {
	memory_data = Read_Memory(address);
	ushort result = static_cast<ushort>((REGISTERS::a + BIT_8) - memory_data);

	if (result > 0xff)
		Set_Carry();
	else
		Clear_Carry();

	if ((REGISTERS::a - memory_data) == 0)
		Set_Zero();
	else
		Clear_Zero();

	if (((REGISTERS::a - memory_data) & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_CMP_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	CMP(memory_data);
	instruction_cycles = cycle_list[CMP_IMMEDIATE];
}

void MOS6502::_CMP_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	CMP(address);
	instruction_cycles = cycle_list[CMP_ZERO_PAGE];
}

void MOS6502::_CMP_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	CMP(address);
	instruction_cycles = cycle_list[CMP_ZERO_PAGE_X];
}

void MOS6502::_CMP_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	CMP(address);
	instruction_cycles = cycle_list[CMP_ABSOLUTE];
}

void MOS6502::_CMP_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	CMP(address);
	instruction_cycles = cycle_list[CMP_ABSOLUTE_X];
}

void MOS6502::_CMP_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	CMP(address);
	instruction_cycles = cycle_list[CMP_ABSOLUTE_Y];
}

void MOS6502::_CMP_::Indirect_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Indirect_X(address);
	CMP(address);
	instruction_cycles = cycle_list[CMP_INDIRECT_X];
}

void MOS6502::_CMP_::Indirect_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	instruction_cycles = 0;
	ADDRESS_MODES::Indirect_Y(address);
	CMP(address);
	instruction_cycles = cycle_list[CMP_INDIRECT_Y];
}

void MOS6502::_CPX_::CPX(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	ushort result = static_cast<ushort>((REGISTERS::x + 0x100) - memory_data);

	if (result > 0xff)
		Set_Carry();
	else
		Clear_Carry();

	if ((REGISTERS::x - memory_data) == 0)
		Set_Zero();
	else
		Clear_Zero();

	if (((REGISTERS::x - memory_data) & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_CPX_::CPX(ushort address) {
	memory_data = Read_Memory(address);
	ushort result = static_cast<ushort>((REGISTERS::x + 0x100) - memory_data);

	if (result > 0xff)
		Set_Carry();
	else
		Clear_Carry();

	if ((REGISTERS::x - memory_data) == 0)
		Set_Zero();
	else
		Clear_Zero();

	if (((REGISTERS::x - memory_data) & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_CPX_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	CPX(memory_data);
	instruction_cycles = cycle_list[CPX_IMMEDIATE];
}

void MOS6502::_CPX_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	CPX(address);
	instruction_cycles = cycle_list[CPX_ZERO_PAGE];
}

void MOS6502::_CPX_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	CPX(address);
	instruction_cycles = cycle_list[CPX_ABSOLUTE];
}

void MOS6502::_CPY_::CPY(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	ushort result = static_cast<ushort>((REGISTERS::y + 0x100) - memory_data);

	if (result > 0xff)
		Set_Carry();
	else
		Clear_Carry();

	if ((REGISTERS::y - memory_data) == 0)
		Set_Zero();
	else
		Clear_Zero();

	if (((REGISTERS::y - memory_data) & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_CPY_::CPY(ushort address) {
	memory_data = Read_Memory(address);
	ushort result = static_cast<ushort>((REGISTERS::y + 0x100) - memory_data);

	if (result > 0xff)
		Set_Carry();
	else
		Clear_Carry();

	if ((REGISTERS::y - memory_data) == 0)
		Set_Zero();
	else
		Clear_Zero();

	if (((REGISTERS::y - memory_data) & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_CPY_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	CPY(memory_data);
	instruction_cycles = cycle_list[CPY_IMMEDIATE];
}

void MOS6502::_CPY_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	CPY(address);
	instruction_cycles = cycle_list[CPY_ZERO_PAGE];
}

void MOS6502::_CPY_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	CPY(address);
	instruction_cycles = cycle_list[CPY_ABSOLUTE];
}

void MOS6502::_DEC_::DEC(ushort address) {
	memory_data = Read_Memory(address);
	memory_data--;

	if (memory_data == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	Write_Memory(address, memory_data);
}

void MOS6502::_DEC_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	DEC(address);
	instruction_cycles = cycle_list[DEC_ZERO_PAGE];
}

void MOS6502::_DEC_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	DEC(address);
	instruction_cycles = cycle_list[DEC_ZERO_PAGE_X];
}

void MOS6502::_DEC_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	DEC(address);
	instruction_cycles = cycle_list[DEC_ABSOLUTE];
}

void MOS6502::_DEC_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	DEC(address);
	instruction_cycles = cycle_list[DEC_ABSOLUTE_X];
}

void MOS6502::_DEX_::DEX() {
	REGISTERS::x--;

	if (REGISTERS::x == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::x & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_DEX_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	DEX();
	instruction_cycles = cycle_list[DEX_IMPLIED];
}

void MOS6502::_DEY_::DEY() {
	REGISTERS::y--;

	if (REGISTERS::y == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::y & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_DEY_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	DEY();
	instruction_cycles = cycle_list[DEY_IMPLIED];
}

void MOS6502::_EOR_::EOR(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	REGISTERS::a ^= memory_data;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_EOR_::EOR(ushort address) {
	memory_data = Read_Memory(address);
	REGISTERS::a ^= memory_data;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_EOR_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	EOR(memory_data);
	instruction_cycles = cycle_list[EOR_IMMEDIATE];
}

void MOS6502::_EOR_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	EOR(address);
	instruction_cycles = cycle_list[EOR_ZERO_PAGE];
}

void MOS6502::_EOR_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	EOR(address);
	instruction_cycles = cycle_list[EOR_ZERO_PAGE_X];
}

void MOS6502::_EOR_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	EOR(address);
	instruction_cycles = cycle_list[EOR_ABSOLUTE];
}

void MOS6502::_EOR_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	EOR(address);
	instruction_cycles = cycle_list[EOR_ABSOLUTE_X];
}

void MOS6502::_EOR_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	EOR(address);
	instruction_cycles = cycle_list[EOR_ABSOLUTE_Y];
}

void MOS6502::_EOR_::Indirect_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Indirect_X(address);
	EOR(address);
	instruction_cycles = cycle_list[EOR_INDIRECT_X];
}

void MOS6502::_EOR_::Indirect_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	instruction_cycles = 0;
	ADDRESS_MODES::Indirect_Y(address);
	EOR(address);
	instruction_cycles = cycle_list[EOR_INDIRECT_Y];
}

void MOS6502::_INC_::INC(ushort address) {
	memory_data = Read_Memory(address);
	Write_Memory(address, memory_data);

	memory_data++;

	if (memory_data == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	Write_Memory(address, memory_data);
}

void MOS6502::_INC_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	INC(address);
	instruction_cycles = cycle_list[INC_ZERO_PAGE];
}

void MOS6502::_INC_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	INC(address);
	instruction_cycles = cycle_list[INC_ZERO_PAGE_X];
}

void MOS6502::_INC_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	INC(address);
	instruction_cycles = cycle_list[INC_ABSOLUTE];
}

void MOS6502::_INC_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	INC(address);
	instruction_cycles = cycle_list[INC_ABSOLUTE_X];
}

void MOS6502::_INX_::INX() {
	REGISTERS::x++;

	if (REGISTERS::x == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::x & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_INX_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	INX();
	instruction_cycles = cycle_list[INX_IMPLIED];
}

void MOS6502::_INY_::INY() {
	REGISTERS::y++;

	if (REGISTERS::y == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::y & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_INY_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	INY();
	instruction_cycles = cycle_list[INY_IMPLIED];
}

void MOS6502::_JMP_::JMP(ushort address) {
	REGISTERS::pc = address;
}

void MOS6502::_JMP_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	JMP(address);
	instruction_cycles = cycle_list[JMP_ABSOLUTE];
}

void MOS6502::_JMP_::Indirect(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Indirect(address);
	JMP(address);
	instruction_cycles = cycle_list[JMP_INDIRECT];
}

void MOS6502::_JSR_::JSR(ushort address) {
	REGISTERS::pc++;
	Push16(REGISTERS::pc);
	REGISTERS::pc--;
	ADDRESS_MODES::Absolute(address);
	REGISTERS::pc = address;
}

void MOS6502::_JSR_::Absolute(ushort address) {
	REGISTERS::pc++;
	// ADDRESS_MODES::Absolute(address); // Does not work
	JSR(address);
	instruction_cycles = cycle_list[JSR_ABSOLUTE];
}

void MOS6502::_LDA_::LDA(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	MOS6502::memory_data = memory_data;

	if (memory_data == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	REGISTERS::a = memory_data;
}

void MOS6502::_LDA_::LDA(ushort address) {
	memory_data = Read_Memory(address);
	MOS6502::address = address;

	if (memory_data == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	REGISTERS::a = memory_data;
}

void MOS6502::_LDA_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	LDA(memory_data);
	instruction_cycles = cycle_list[LDA_IMMEDIATE];
}

void MOS6502::_LDA_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	LDA(address);
	instruction_cycles = cycle_list[LDA_ZERO_PAGE];
}

void MOS6502::_LDA_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	LDA(address);
	instruction_cycles = cycle_list[LDA_ZERO_PAGE_X];
}

void MOS6502::_LDA_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	LDA(address);
	instruction_cycles = cycle_list[LDA_ABSOLUTE];
}

void MOS6502::_LDA_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	LDA(address);
	instruction_cycles = cycle_list[LDA_ABSOLUTE_X];
}

void MOS6502::_LDA_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	LDA(address);
	instruction_cycles = cycle_list[LDA_ABSOLUTE_Y];
}

void MOS6502::_LDA_::Indirect_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Indirect_X(address);
	LDA(address);
	instruction_cycles = cycle_list[LDA_INDIRECT_X];
}

void MOS6502::_LDA_::Indirect_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	instruction_cycles = 0;
	ADDRESS_MODES::Indirect_Y(address);
	LDA(address);
	instruction_cycles = cycle_list[LDA_INDIRECT_Y];
}

void MOS6502::_LDX_::LDX(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	if (memory_data == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	REGISTERS::x = memory_data;
}

void MOS6502::_LDX_::LDX(ushort address) {
	memory_data = Read_Memory(address);

	if (memory_data == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	REGISTERS::x = memory_data;
}

void MOS6502::_LDX_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	LDX(memory_data);
	instruction_cycles = cycle_list[LDX_IMMEDIATE];
}

void MOS6502::_LDX_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	LDX(address);
	instruction_cycles = cycle_list[LDX_ZERO_PAGE];
}

void MOS6502::_LDX_::Zero_Page_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_Y(address);
	LDX(address);
	instruction_cycles = cycle_list[LDX_ZERO_PAGE_Y];
}

void MOS6502::_LDX_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	LDX(address);
	instruction_cycles = cycle_list[LDX_ABSOLUTE];
}

void MOS6502::_LDX_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	LDX(address);
	instruction_cycles = cycle_list[LDX_ABSOLUTE_Y];
}

void MOS6502::_LDY_::LDY(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	if (memory_data == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	REGISTERS::y = memory_data;
}

void MOS6502::_LDY_::LDY(ushort address) {
	memory_data = Read_Memory(address);

	if (memory_data == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((memory_data & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	REGISTERS::y = memory_data;
}

void MOS6502::_LDY_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	LDY(memory_data);
	instruction_cycles = cycle_list[LDY_IMMEDIATE];
}

void MOS6502::_LDY_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	LDY(address);
	instruction_cycles = cycle_list[LDY_ZERO_PAGE];
}

void MOS6502::_LDY_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	LDY(address);
	instruction_cycles = cycle_list[LDY_ZERO_PAGE_X];
}

void MOS6502::_LDY_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	LDY(address);
	instruction_cycles = cycle_list[LDY_ABSOLUTE];
}

void MOS6502::_LDY_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	LDY(address);
	instruction_cycles = cycle_list[LDY_ABSOLUTE_X];
}

void MOS6502::_LSR_::LSR() {
	byte result = REGISTERS::a >> 1;

	if ((REGISTERS::a & BIT_0) == BIT_0)
		Set_Carry();
	else
		Clear_Carry();

	REGISTERS::a = result & 0xff;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_LSR_::LSR(ushort address) {
	memory_data = Read_Memory(address);
	byte result = memory_data >> 1;

	if ((memory_data & BIT_0) == BIT_0)
		Set_Carry();
	else
		Clear_Carry();

	if (result == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((result & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	Write_Memory(address, result);
}

void MOS6502::_LSR_::Accumulator() {
	REGISTERS::pc++;
	ADDRESS_MODES::Accumulator();
	LSR();
	instruction_cycles = cycle_list[LSR_ACCUMULATOR];
}

void MOS6502::_LSR_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	LSR(address);
	instruction_cycles = cycle_list[LSR_ZERO_PAGE];
}

void MOS6502::_LSR_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	LSR(address);
	instruction_cycles = cycle_list[LSR_ZERO_PAGE_X];
}

void MOS6502::_LSR_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	LSR(address);
	instruction_cycles = cycle_list[LSR_ABSOLUTE];
}

void MOS6502::_LSR_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	LSR(address);
	instruction_cycles = cycle_list[LSR_ABSOLUTE_X];
}

void MOS6502::_NOP_::NOP() {
	//No Operation
}

void MOS6502::_NOP_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	NOP();
	instruction_cycles = cycle_list[NOP_IMPLIED];
}

void MOS6502::_ORA_::ORA(byte memory_data) {
	if (emulated == true)
		memory_data = Read_Memory(address);

	REGISTERS::a |= memory_data;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_ORA_::ORA(ushort address) {
	memory_data = Read_Memory(address);
	REGISTERS::a |= memory_data;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_ORA_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	ORA(memory_data);
	instruction_cycles = cycle_list[ORA_IMMEDIATE];
}

void MOS6502::_ORA_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	ORA(address);
	instruction_cycles = cycle_list[ORA_ZERO_PAGE];
}

void MOS6502::_ORA_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	ORA(address);
	instruction_cycles = cycle_list[ORA_ZERO_PAGE_X];
}

void MOS6502::_ORA_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	ORA(address);
	instruction_cycles = cycle_list[ORA_ABSOLUTE];
}

void MOS6502::_ORA_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	ORA(address);
	instruction_cycles = cycle_list[ORA_ABSOLUTE_X];
}

void MOS6502::_ORA_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	ORA(address);
	instruction_cycles = cycle_list[ORA_ABSOLUTE_Y];
}

void MOS6502::_ORA_::Indirect_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Indirect_X(address);
	ORA(address);
	instruction_cycles = cycle_list[ORA_INDIRECT_X];
}

void MOS6502::_ORA_::Indirect_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	instruction_cycles = 0;
	ADDRESS_MODES::Indirect_Y(address);
	ORA(address);
	instruction_cycles = cycle_list[ORA_INDIRECT_Y];
}

void MOS6502::_PHA_::PHA() {
	Push8(REGISTERS::a);
}

void MOS6502::_PHA_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	PHA();
	instruction_cycles = cycle_list[PHA_IMPLIED];
}

void MOS6502::_PHP_::PHP() {
	REGISTERS::sr = (REGISTERS::sr | BIT_5) | BREAK_REGISTER;
	Push8(REGISTERS::sr);
}

void MOS6502::_PHP_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	PHP();
	instruction_cycles = cycle_list[PHP_IMPLIED];
}

void MOS6502::_PLA_::PLA() {
	REGISTERS::a = Pull8();

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_PLA_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	PLA();
	instruction_cycles = cycle_list[PLA_IMPLIED];
}

void MOS6502::_PLP_::PLP() {
	REGISTERS::sr = Pull8() | BIT_5;
}

void MOS6502::_PLP_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	PLP();
	instruction_cycles = cycle_list[PLP_IMPLIED];
}

void MOS6502::_ROL_::ROL() {
	int result = REGISTERS::a << 1;

	if (Is_Carry())
		result |= BIT_0;

	if ((REGISTERS::a & BIT_7) == BIT_7)
		Set_Carry();
	else
		Clear_Carry();

	REGISTERS::a = static_cast<byte>(result) & 0xff;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_ROL_::ROL(ushort address) {
	memory_data = Read_Memory(address);
	Write_Memory(address, memory_data);

	int Result = memory_data << 1;

	if (Is_Carry())
		Result |= BIT_0;

	if ((memory_data & BIT_7) == BIT_7)
		Set_Carry();
	else
		Clear_Carry();

	if (Result == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((Result & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	Write_Memory(address, static_cast<byte>(Result));
}

void MOS6502::_ROL_::Accumulator() {
	REGISTERS::pc++;
	ADDRESS_MODES::Accumulator();
	ROL();
	instruction_cycles = cycle_list[ROL_ACCUMULATOR];
}

void MOS6502::_ROL_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	ROL(address);
	instruction_cycles = cycle_list[ROL_ZERO_PAGE];
}

void MOS6502::_ROL_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	ROL(address);
	instruction_cycles = cycle_list[ROL_ZERO_PAGE_X];
}

void MOS6502::_ROL_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	ROL(address);
	instruction_cycles = cycle_list[ROL_ABSOLUTE];
}

void MOS6502::_ROL_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	ROL(address);
	instruction_cycles = cycle_list[ROL_ABSOLUTE_X];
}

void MOS6502::_ROR_::ROR() {
	byte result = REGISTERS::a >> 1;

	if (Is_Carry())
		result |= BIT_7;

	if ((REGISTERS::a & BIT_0) == BIT_0)
		Set_Carry();
	else
		Clear_Carry();

	REGISTERS::a = result;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_ROR_::ROR(ushort address) {
	memory_data = Read_Memory(address);
	Write_Memory(address, memory_data);

	byte result = memory_data >> 1;

	if (Is_Carry())
		result |= BIT_7;

	if ((memory_data & BIT_0) == BIT_0)
		Set_Carry();
	else
		Clear_Carry();

	if (result == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((result & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();

	Write_Memory(address, result);
}

void MOS6502::_ROR_::Accumulator() {
	REGISTERS::pc++;
	ADDRESS_MODES::Accumulator();
	ROR();
	instruction_cycles = cycle_list[ROR_ACCUMULATOR];
}

void MOS6502::_ROR_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	ROR(address);
	instruction_cycles = cycle_list[ROR_ZERO_PAGE];
}

void MOS6502::_ROR_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	ROR(address);
	instruction_cycles = cycle_list[ROR_ZERO_PAGE_X];
}

void MOS6502::_ROR_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	ROR(address);
	instruction_cycles = cycle_list[ROR_ABSOLUTE];
}

void MOS6502::_ROR_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	ROR(address);
	instruction_cycles = cycle_list[ROR_ABSOLUTE_X];
}

void MOS6502::_RTI_::RTI() {
	REGISTERS::sr = Pull8();
	REGISTERS::sr |= BIT_5;
	REGISTERS::pc = Pull8();
	REGISTERS::pc += Pull8() << 8;
}

void MOS6502::_RTI_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	RTI();
	instruction_cycles = cycle_list[RTI_IMPLIED];
}

void MOS6502::_RTS_::RTS() {
	REGISTERS::pc = Pull8();
	REGISTERS::pc += Pull8() << 8;
	REGISTERS::pc++;
}

void MOS6502::_RTS_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	RTS();
	instruction_cycles = cycle_list[RTS_IMPLIED];
}

void MOS6502::_SBC_::SBC(byte memory_data) {
	ushort temp;
	ushort sum;

	if (emulated == true)
		memory_data = Read_Memory(address);

	byte carry = REGISTERS::sr & CARRY_REGISTER ? 1 : 0;

	if ((REGISTERS::a ^ memory_data) & BIT_7)
		Set_Overflow();
	else
		Clear_Overflow();

	if (Is_Decimal() == true && nes_cpu == false) {
		temp = 0xf + (REGISTERS::a & 0xf) - (memory_data & 0xf) + carry;

		if (temp <= 0xf) {
			sum = 0;
			temp -= 6;
		}
		else {
			sum = 0x10;
			temp -= 0x10;
		}

		sum += 0xf0 + (REGISTERS::a & 0xf0) - (memory_data & 0xf0);

		if (sum <= 0xff) {
			Clear_Carry();
			sum -= 0x60;
		}
		else
			Set_Carry();

		sum += temp;
	}
	else {
		sum = 0xff + REGISTERS::a - memory_data + carry;

		if (sum > 0xff)
			Set_Carry();
		else
			Clear_Carry();
	}

	if (((REGISTERS::a ^ static_cast<byte>(sum)) & SIGN_REGISTER) && ((REGISTERS::a ^ memory_data) & SIGN_REGISTER))
		Set_Overflow();
	else
		Clear_Overflow();

	REGISTERS::a = sum & 0xff;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_SBC_::SBC(ushort address) {
	ushort temp;
	ushort sum;
	memory_data = Read_Memory(address);
	byte carry = REGISTERS::sr & CARRY_REGISTER ? 1 : 0;

	if ((REGISTERS::a ^ memory_data) & BIT_7)
		Set_Overflow();
	else
		Clear_Overflow();

	if (Is_Decimal() == true && nes_cpu == false) {
		temp = 0xf + (REGISTERS::a & 0xf) - (memory_data & 0xf) + carry;

		if (temp <= 0xf) {
			sum = 0;
			temp -= 6;
		}
		else {
			sum = 0x10;
			temp -= 0x10;
		}

		sum += 0xf0 + (REGISTERS::a & 0xf0) - (memory_data & 0xf0);

		if (sum <= 0xff) {
			Clear_Carry();
			sum -= 0x60;
		}
		else
			Set_Carry();

		sum += temp;
	}
	else {
		sum = 0xff + REGISTERS::a - memory_data + carry;

		if (sum > 0xff)
			Set_Carry();
		else
			Clear_Carry();
	}

	if (((REGISTERS::a ^ static_cast<byte>(sum)) & SIGN_REGISTER) && ((REGISTERS::a ^ memory_data) & SIGN_REGISTER))
		Set_Overflow();
	else
		Clear_Overflow();

	REGISTERS::a = sum & 0xff;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_SBC_::Immediate(byte memory_data) {
	REGISTERS::pc++;
	ADDRESS_MODES::Immediate();
	SBC(memory_data);
	instruction_cycles = cycle_list[SBC_IMMEDIATE];
}

void MOS6502::_SBC_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	SBC(address);
	instruction_cycles = cycle_list[SBC_ZERO_PAGE];
}

void MOS6502::_SBC_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	SBC(address);
	instruction_cycles = cycle_list[SBC_ZERO_PAGE_X];
}

void MOS6502::_SBC_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	SBC(address);
	instruction_cycles = cycle_list[SBC_ABSOLUTE];
}

void MOS6502::_SBC_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	SBC(address);
	instruction_cycles = cycle_list[SBC_ABSOLUTE_X];
}

void MOS6502::_SBC_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	SBC(address);
	instruction_cycles = cycle_list[SBC_ABSOLUTE_Y];
}

void MOS6502::_SBC_::Indirect_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Indirect_X(address);
	SBC(address);
	instruction_cycles = cycle_list[SBC_INDIRECT_X];
}

void MOS6502::_SBC_::Indirect_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	instruction_cycles = 0;
	ADDRESS_MODES::Indirect_Y(address);
	SBC(address);
	instruction_cycles = cycle_list[SBC_INDIRECT_Y];
}

void MOS6502::_SEC_::SEC() {
	Set_Carry();
}

void MOS6502::_SEC_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	SEC();
	instruction_cycles = cycle_list[SEC_IMPLIED];
}

void MOS6502::_SED_::SED() {
	Set_Decimal();
}

void MOS6502::_SED_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	SED();
	instruction_cycles = cycle_list[SED_IMPLIED];
}

void MOS6502::_SEI_::SEI() {
	Set_Interrupt();
}

void MOS6502::_SEI_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	SEI();
	instruction_cycles = cycle_list[SEI_IMPLIED];
}

void MOS6502::_STA_::STA(ushort address) {
	MOS6502::address = address;
	Write_Memory(address, REGISTERS::a);
}

void MOS6502::_STA_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	STA(address);
	instruction_cycles = cycle_list[STA_ZERO_PAGE];
}

void MOS6502::_STA_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	STA(address);
	instruction_cycles = cycle_list[STA_ZERO_PAGE_X];
}

void MOS6502::_STA_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	STA(address);
	instruction_cycles = cycle_list[STA_ABSOLUTE];
}

void MOS6502::_STA_::Absolute_X(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_X(address);
	STA(address);
	instruction_cycles = cycle_list[STA_ABSOLUTE_X];
}

void MOS6502::_STA_::Absolute_Y(ushort address) {
	REGISTERS::pc++;
	instruction_cycles = 0;
	ADDRESS_MODES::Absolute_Y(address);
	STA(address);
	instruction_cycles = cycle_list[STA_ABSOLUTE_Y];
}

void MOS6502::_STA_::Indirect_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Indirect_X(address);
	STA(address);
	instruction_cycles = cycle_list[STA_INDIRECT_X];
}

void MOS6502::_STA_::Indirect_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	instruction_cycles = 0;
	ADDRESS_MODES::Indirect_Y(address);
	STA(address);
	instruction_cycles = cycle_list[STA_INDIRECT_Y];
}

void MOS6502::_STX_::STX(ushort address) {
	Write_Memory(address, REGISTERS::x);
}

void MOS6502::_STX_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	STX(address);
	instruction_cycles = cycle_list[STX_ZERO_PAGE];
}

void MOS6502::_STX_::Zero_Page_Y(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_Y(address);
	STX(address);
	instruction_cycles = cycle_list[STX_ZERO_PAGE_Y];
}

void MOS6502::_STX_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	STX(address);
	instruction_cycles = cycle_list[STX_ABSOLUTE];
}

void MOS6502::_STY_::STY(ushort address) {
	Write_Memory(address, REGISTERS::y);
}

void MOS6502::_STY_::Zero_Page(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page(address);
	STY(address);
	instruction_cycles = cycle_list[STY_ZERO_PAGE];
}

void MOS6502::_STY_::Zero_Page_X(ushort address) {
	REGISTERS::pc++;
	address &= 0xff; // Zero page wrap around
	ADDRESS_MODES::Zero_Page_X(address);
	STY(address);
	instruction_cycles = cycle_list[STY_ZERO_PAGE_X];
}

void MOS6502::_STY_::Absolute(ushort address) {
	REGISTERS::pc++;
	ADDRESS_MODES::Absolute(address);
	STY(address);
	instruction_cycles = cycle_list[STY_ABSOLUTE];
}

void MOS6502::_TAX_::TAX() {
	REGISTERS::x = REGISTERS::a;

	if (REGISTERS::x == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::x & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_TAX_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	TAX();
	instruction_cycles = cycle_list[TAX_IMPLIED];
}

void MOS6502::_TAY_::TAY() {
	REGISTERS::y = REGISTERS::a;

	if (REGISTERS::y == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::y & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_TAY_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	TAY();
	instruction_cycles = cycle_list[TAY_IMPLIED];
}

void MOS6502::_TSX_::TSX() {
	REGISTERS::x = REGISTERS::sp;

	if (REGISTERS::x == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::x & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_TSX_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	TSX();
	instruction_cycles = cycle_list[TSX_IMPLIED];
}

void MOS6502::_TXA_::TXA() {
	REGISTERS::a = REGISTERS::x;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_TXA_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	TXA();
	instruction_cycles = cycle_list[TXA_IMPLIED];
}

void MOS6502::_TXS_::TXS() {
	REGISTERS::sp = REGISTERS::x;
}

void MOS6502::_TXS_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	TXS();
	instruction_cycles = cycle_list[TXS_IMPLIED];
}

void MOS6502::_TYA_::TYA() {
	REGISTERS::a = REGISTERS::y;

	if (REGISTERS::a == 0)
		Set_Zero();
	else
		Clear_Zero();

	if ((REGISTERS::a & SIGN_REGISTER) == SIGN_REGISTER)
		Set_Sign();
	else
		Clear_Sign();
}

void MOS6502::_TYA_::Implied() {
	REGISTERS::pc++;
	ADDRESS_MODES::Implied();
	TYA();
	instruction_cycles = cycle_list[TYA_IMPLIED];
}

void MOS6502::INTERRUPT::RESET() {
	REGISTERS::a = 0;
	REGISTERS::x = 0;
	REGISTERS::y = 0;
	REGISTERS::sr = BIT_5;
	REGISTERS::sp = 0xFF;
	REGISTERS::pc = Read_Memory(0xFFFC) | (Read_Memory(0xFFFD) << 8);
}

void MOS6502::INTERRUPT::NMI() {
	Push16(REGISTERS::pc);
	Push8(REGISTERS::sr);
	REGISTERS::sr |= INTERRUPT_REGISTER;
	REGISTERS::pc = Read_Memory(0xFFFA) | (Read_Memory(0xFFFB) << 8);
	instruction_cycles = 7;
	cycles += instruction_cycles;
	cpu_cycles_on_current_scanline += instruction_cycles;
	machine_cycles += instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;

	if (region == NTSC) {
		machine_cycles += instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		instruction_machine_cycles = instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
	}
	else if (region == PAL) {
		machine_cycles += instruction_cycles * cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		instruction_machine_cycles = instruction_cycles * cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
	}
}

void MOS6502::INTERRUPT::IRQ() {
	// Maskable interrupt
	if ((REGISTERS::sr & INTERRUPT_REGISTER) != INTERRUPT_REGISTER) {
		Push16(REGISTERS::pc);
		Push8(REGISTERS::sr);
		REGISTERS::sr |= INTERRUPT_REGISTER;
		REGISTERS::pc = Read_Memory(0xFFFE) | (Read_Memory(0xFFFF) << 8);
		instruction_cycles = 7;
		cycles += instruction_cycles;
		cpu_cycles_on_current_scanline += instruction_cycles;
		machine_cycles += instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;

		if (region == NTSC) {
			machine_cycles += instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles = instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else if (region == PAL) {
			machine_cycles += instruction_cycles * cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles = instruction_cycles * cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
	}
}

void MOS6502::Push8(byte data) {
	Write_Memory((0x100 + (REGISTERS::sp & 0xff)), (data & 0xff));
	REGISTERS::sp--;
}

void MOS6502::Push16(ushort data) {
	Push8(data >> 8);
	Push8(data & 0xff);
}

byte MOS6502::Pull8() {
	REGISTERS::sp++;
	return Read_Memory(0x100 + REGISTERS::sp);
}

void MOS6502::ADDRESS_MODES::Accumulator() {
	// Does nothing
}

void MOS6502::ADDRESS_MODES::Immediate() {
	address = REGISTERS::pc;
	REGISTERS::pc++;
}

void MOS6502::ADDRESS_MODES::Zero_Page(ushort& address) {
	address = static_cast<ushort>(Read_Memory(REGISTERS::pc)) & 0xff;
	REGISTERS::pc++;
}

void MOS6502::ADDRESS_MODES::Zero_Page_X(ushort& address) {
	byte data = Read_Memory(REGISTERS::pc);
	REGISTERS::pc++;
	data += REGISTERS::x;
	address = static_cast<ushort>(data) & 0xff;
}

void MOS6502::ADDRESS_MODES::Zero_Page_Y(ushort& address) {
	byte data = Read_Memory(REGISTERS::pc);
	REGISTERS::pc++;
	data += REGISTERS::y;
	address = static_cast<ushort>(data) & 0xff;
}

void MOS6502::ADDRESS_MODES::Absolute(ushort& address) {
	address = static_cast<ushort>(Read_Memory(REGISTERS::pc));
	REGISTERS::pc++;
	address += static_cast<ushort>(Read_Memory(REGISTERS::pc) << 8);
	REGISTERS::pc++;
}

void MOS6502::ADDRESS_MODES::Absolute_X(ushort& address) {
	address = static_cast<ushort>(Read_Memory(REGISTERS::pc));
	REGISTERS::pc++;
	address += static_cast<ushort>(Read_Memory(REGISTERS::pc) << 8);
	REGISTERS::pc++;
	ushort start_page = address & 0xff00; // ff00 = 1111 1111 0000 0000
	address += REGISTERS::x;
	ushort page = address & 0xff00;

	if (start_page != page) { // 1 cycles penalty for page crossing
		cycles++;
		instruction_cycles++;
		cpu_cycles_on_current_scanline++;
		machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;

		if (region == NTSC) {
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else if (region == PAL) {
			machine_cycles += cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles += cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
	}
}

void MOS6502::ADDRESS_MODES::Absolute_Y(ushort& address) {
	address = static_cast<ushort>(Read_Memory(REGISTERS::pc));
	REGISTERS::pc++;
	address += static_cast<ushort>(Read_Memory(REGISTERS::pc) << 8);
	REGISTERS::pc++;
	ushort start_page = address & 0xff00; // ff00 = 1111 1111 0000 0000
	address += REGISTERS::y;
	ushort page = address & 0xff00;

	if (start_page != page) { // 1 cycles penalty for page crossing
		cycles++;
		instruction_cycles++;
		cpu_cycles_on_current_scanline++;
		machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;

		if (region == NTSC) {
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else if (region == PAL) {
			machine_cycles += cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles += cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
	}
}

void MOS6502::ADDRESS_MODES::Implied() {
	// Does nothing
}

void MOS6502::ADDRESS_MODES::Relative(ushort& address) {
	address = static_cast<ushort>(Read_Memory(REGISTERS::pc++));

	if (address & BIT_7)
		address = address | 0xff00;
}

void MOS6502::ADDRESS_MODES::Indirect_X(ushort& address) {
	byte data = Read_Memory(REGISTERS::pc);
	REGISTERS::pc++;
	data += REGISTERS::x & 0xff;
	byte low_address = data & 0xff;
	byte high_address = (data + 1) & 0xff;
	address = static_cast<ushort>(Read_Memory(low_address));
	address += static_cast<ushort>(Read_Memory(high_address) << 8);
}

void MOS6502::ADDRESS_MODES::Indirect_Y(ushort& address) {
	byte data = Read_Memory(REGISTERS::pc);
	REGISTERS::pc++;
	byte low_address = data & 0xff;
	byte high_address = (data & 0xff00) | ((data + 1) & 0x00ff); // Zero page wrap around
	address = static_cast<ushort>(Read_Memory(low_address));
	address += static_cast<ushort>(Read_Memory(high_address) << 8);
	ushort start_page = address & 0xff00; // ff00 = 1111 1111 0000 0000
	address += REGISTERS::y;
	ushort page = address & 0xff00;

	if (start_page != page) { // 1 cycles penalty for page crossing
		cycles++;
		instruction_cycles++;
		cpu_cycles_on_current_scanline++;
		machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;

		if (region == NTSC) {
			machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles += cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
		else if (region == PAL) {
			machine_cycles += cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles += cpu.PAL_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
		}
	}
}

void MOS6502::ADDRESS_MODES::Indirect(ushort& address) {
	ushort address_helper, address_helper2;
	address_helper = Read_Memory(REGISTERS::pc) + (Read_Memory(REGISTERS::pc + 1) << 8);
	address_helper2 = (address_helper & 0xff00) + ((address_helper + 1) & 0x00ff); // replicate 6502 page-boundary wraparound bug
	address = Read_Memory(address_helper) + (Read_Memory(address_helper2) << 8);
	REGISTERS::pc += 2;
}
