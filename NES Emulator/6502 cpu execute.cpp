#include "6502 cpu.h"
#include "ppu.h"
#include "Main.h"

bool show_once;

void MOS6502::Fire_Opcode(byte opcode) {
	switch (opcode) {
		// ADC: 0x69; 0x65; 0x75; 0x6d; 0x7d; 0x79; 0x61; 0x71
	case ADC_IMMEDIATE: cpu.instruction.ADC.Immediate(cpu.memory_data); break;
	case ADC_ZERO_PAGE: cpu.instruction.ADC.Zero_Page(cpu.address); break;
	case ADC_ZERO_PAGE_X: cpu.instruction.ADC.Zero_Page_X(cpu.address); break;
	case ADC_ABSOLUTE: cpu.instruction.ADC.Absolute(cpu.address); break;
	case ADC_ABSOLUTE_X: cpu.instruction.ADC.Absolute_X(cpu.address); break;
	case ADC_ABSOLUTE_Y: cpu.instruction.ADC.Absolute_Y(cpu.address); break;
	case ADC_INDIRECT_X: cpu.instruction.ADC.Indirect_X(cpu.address); break;
	case ADC_INDIRECT_Y: cpu.instruction.ADC.Indirect_Y(cpu.address); break;

		// AND: 0x29; 0x25; 0x35; 0x2d; 0x3d; 0x39; 0x21; 0x31
	case AND_IMMEDIATE: cpu.instruction.AND.Immediate(cpu.memory_data); break;
	case AND_ZERO_PAGE: cpu.instruction.AND.Zero_Page(cpu.address); break;
	case AND_ZERO_PAGE_X: cpu.instruction.AND.Zero_Page_X(cpu.address); break;
	case AND_ABSOLUTE: cpu.instruction.AND.Absolute(cpu.address); break;
	case AND_ABSOLUTE_X: cpu.instruction.AND.Absolute_X(cpu.address); break;
	case AND_ABSOLUTE_Y: cpu.instruction.AND.Absolute_Y(cpu.address); break;
	case AND_INDIRECT_X: cpu.instruction.AND.Indirect_X(cpu.address); break;
	case AND_INDIRECT_Y: cpu.instruction.AND.Indirect_Y(cpu.address); break;

		// ASL: 0x0a; 0x06; 0x16; 0x0e; 0x1e
	case ASL_ACCUMULATOR: cpu.instruction.ASL.Accumulator(); break;
	case ASL_ZERO_PAGE: cpu.instruction.ASL.Zero_Page(cpu.address); break;
	case ASL_ZERO_PAGE_X: cpu.instruction.ASL.Zero_Page_X(cpu.address); break;
	case ASL_ABSOLUTE: cpu.instruction.ASL.Absolute(cpu.address); break;
	case ASL_ABSOLUTE_X: cpu.instruction.ASL.Absolute_X(cpu.address); break;

		// BCC: 0x90
	case BCC_RELATIVE: cpu.instruction.BCC.Relative(cpu.address); break;

		// BCS: 0xb0
	case BCS_RELATIVE: cpu.instruction.BCS.Relative(cpu.address); break;

		// BEQ: 0xf0
	case BEQ_RELATIVE: cpu.instruction.BEQ.Relative(cpu.address); break;

		// BIT: 0x24; 0x2c
	case BIT_ZERO_PAGE: cpu.instruction.BIT.Zero_Page(cpu.address); break;
	case BIT_ABSOLUTE: cpu.instruction.BIT.Absolute(cpu.address); break;

		// BMI: 0x30
	case BMI_RELATIVE: cpu.instruction.BMI.Relative(cpu.address); break;

		// BNE: 0xd0
	case BNE_RELATIVE: cpu.instruction.BNE.Relative(cpu.address); break;

		// BPL: 0x10
	case BPL_RELATIVE: cpu.instruction.BPL.Relative(cpu.address); break;

		// BRK: 0x00
	case BRK_IMPLIED: cpu.instruction.BRK.Implied(); break;

		// BVC: 0x50
	case BVC_RELATIVE: cpu.instruction.BVC.Relative(cpu.address); break;

		// BVS: 0x70
	case BVS_RELATIVE: cpu.instruction.BVS.Relative(cpu.address); break;

		// CLC: 0x18
	case CLC_IMPLIED: cpu.instruction.CLC.Implied(); break;

		// CLD: 0xd8
	case CLD_IMPLIED: cpu.instruction.CLD.Implied(); break;

		// CLI: 0x58
	case CLI_IMPLIED: cpu.instruction.CLI.Implied(); break;

		// CLV: 0xb8
	case CLV_IMPLIED: cpu.instruction.CLV.Implied(); break;

		// CMP: 0xc9; 0xc5; 0xd5; 0xcd; 0xdd; 0xd9; 0xc1; 0xd1
	case CMP_IMMEDIATE: cpu.instruction.CMP.Immediate(cpu.memory_data); break;
	case CMP_ZERO_PAGE: cpu.instruction.CMP.Zero_Page(cpu.address); break;
	case CMP_ZERO_PAGE_X: cpu.instruction.CMP.Zero_Page_X(cpu.address); break;
	case CMP_ABSOLUTE: cpu.instruction.CMP.Absolute(cpu.address); break;
	case CMP_ABSOLUTE_X: cpu.instruction.CMP.Absolute_X(cpu.address); break;
	case CMP_ABSOLUTE_Y: cpu.instruction.CMP.Absolute_Y(cpu.address); break;
	case CMP_INDIRECT_X: cpu.instruction.CMP.Indirect_X(cpu.address); break;
	case CMP_INDIRECT_Y: cpu.instruction.CMP.Indirect_Y(cpu.address); break;

		// CPX: 0xe0; 0xe4; 0xec
	case CPX_IMMEDIATE: cpu.instruction.CPX.Immediate(cpu.memory_data); break;
	case CPX_ZERO_PAGE: cpu.instruction.CPX.Zero_Page(cpu.address); break;
	case CPX_ABSOLUTE: cpu.instruction.CPX.Absolute(cpu.address); break;

		// CPY: 0xc0; 0xc4; 0xcc
	case CPY_IMMEDIATE: cpu.instruction.CPY.Immediate(cpu.memory_data); break;
	case CPY_ZERO_PAGE: cpu.instruction.CPY.Zero_Page(cpu.address); break;
	case CPY_ABSOLUTE: cpu.instruction.CPY.Absolute(cpu.address); break;

		// DEC: 0xc6; 0xd6; 0xce; 0xde
	case DEC_ZERO_PAGE: cpu.instruction.DEC.Zero_Page(cpu.address); break;
	case DEC_ZERO_PAGE_X: cpu.instruction.DEC.Zero_Page_X(cpu.address); break;
	case DEC_ABSOLUTE: cpu.instruction.DEC.Absolute(cpu.address); break;
	case DEC_ABSOLUTE_X: cpu.instruction.DEC.Absolute_X(cpu.address); break;

		// DEX: 0xca;
	case DEX_IMPLIED: cpu.instruction.DEX.Implied(); break;

		// DEY: 0x88;
	case DEY_IMPLIED: cpu.instruction.DEY.Implied(); break;

		// EOR: 0x49; 0x45; 0x55; 0x4d; 0x5d; 0x59; 0x41; 0x51
	case EOR_IMMEDIATE: cpu.instruction.EOR.Immediate(cpu.memory_data); break;
	case EOR_ZERO_PAGE: cpu.instruction.EOR.Zero_Page(cpu.address); break;
	case EOR_ZERO_PAGE_X: cpu.instruction.EOR.Zero_Page_X(cpu.address); break;
	case EOR_ABSOLUTE: cpu.instruction.EOR.Absolute(cpu.address); break;
	case EOR_ABSOLUTE_X: cpu.instruction.EOR.Absolute_X(cpu.address); break;
	case EOR_ABSOLUTE_Y: cpu.instruction.EOR.Absolute_Y(cpu.address); break;
	case EOR_INDIRECT_X: cpu.instruction.EOR.Indirect_X(cpu.address); break;
	case EOR_INDIRECT_Y: cpu.instruction.EOR.Indirect_Y(cpu.address); break;

		// INC: 0xe6; 0xf6; 0xee; 0xfe
	case INC_ZERO_PAGE: cpu.instruction.INC.Zero_Page(cpu.address); break;
	case INC_ZERO_PAGE_X: cpu.instruction.INC.Zero_Page_X(cpu.address); break;
	case INC_ABSOLUTE: cpu.instruction.INC.Absolute(cpu.address); break;
	case INC_ABSOLUTE_X: cpu.instruction.INC.Absolute_X(cpu.address); break;

		// INX: 0xe8
	case INX_IMPLIED: cpu.instruction.INX.Implied(); break;

		// INY: 0xc8
	case INY_IMPLIED: cpu.instruction.INY.Implied(); break;

		// JMP: 0x4c; 0x6c
	case JMP_ABSOLUTE: cpu.instruction.JMP.Absolute(cpu.address); break;
	case JMP_INDIRECT: cpu.instruction.JMP.Indirect(cpu.address); break;

		// JSR: 0x20
	case JSR_ABSOLUTE: cpu.instruction.JSR.Absolute(cpu.address); break;

		// LDA: 0xa9; 0xa5; 0xb5; 0xad; 0xbd; 0xb9; 0xa1; 0xb1
	case LDA_IMMEDIATE: cpu.instruction.LDA.Immediate(cpu.memory_data); break;
	case LDA_ZERO_PAGE: cpu.instruction.LDA.Zero_Page(cpu.address); break;
	case LDA_ZERO_PAGE_X: cpu.instruction.LDA.Zero_Page_X(cpu.address); break;
	case LDA_ABSOLUTE: cpu.instruction.LDA.Absolute(cpu.address); break;
	case LDA_ABSOLUTE_X: cpu.instruction.LDA.Absolute_X(cpu.address); break;
	case LDA_ABSOLUTE_Y: cpu.instruction.LDA.Absolute_Y(cpu.address); break;
	case LDA_INDIRECT_X: cpu.instruction.LDA.Indirect_X(cpu.address); break;
	case LDA_INDIRECT_Y: cpu.instruction.LDA.Indirect_Y(cpu.address); break;

		// LDX: 0xa2; 0xa6; 0xb6; 0xae; 0xbe
	case LDX_IMMEDIATE: cpu.instruction.LDX.Immediate(cpu.memory_data); break;
	case LDX_ZERO_PAGE: cpu.instruction.LDX.Zero_Page(cpu.address); break;
	case LDX_ZERO_PAGE_Y: cpu.instruction.LDX.Zero_Page_Y(cpu.address); break;
	case LDX_ABSOLUTE: cpu.instruction.LDX.Absolute(cpu.address); break;
	case LDX_ABSOLUTE_Y: cpu.instruction.LDX.Absolute_Y(cpu.address); break;

		// LDY: 0xa0; 0xa4; 0xb4; 0xac; 0xbc
	case LDY_IMMEDIATE: cpu.instruction.LDY.Immediate(cpu.memory_data); break;
	case LDY_ZERO_PAGE: cpu.instruction.LDY.Zero_Page(cpu.address); break;
	case LDY_ZERO_PAGE_X: cpu.instruction.LDY.Zero_Page_X(cpu.address); break;
	case LDY_ABSOLUTE: cpu.instruction.LDY.Absolute(cpu.address); break;
	case LDY_ABSOLUTE_X: cpu.instruction.LDY.Absolute_X(cpu.address); break;

		// LSR: 0x4a; 0x46; 0x56; 0x4e; 0x5e
	case LSR_ACCUMULATOR: cpu.instruction.LSR.Accumulator(); break;
	case LSR_ZERO_PAGE: cpu.instruction.LSR.Zero_Page(cpu.address); break;
	case LSR_ZERO_PAGE_X: cpu.instruction.LSR.Zero_Page_X(cpu.address); break;
	case LSR_ABSOLUTE: cpu.instruction.LSR.Absolute(cpu.address); break;
	case LSR_ABSOLUTE_X: cpu.instruction.LSR.Absolute_X(cpu.address); break;

		// NOP: 0xea
	case NOP_IMPLIED: cpu.instruction.NOP.Implied(); break;

		// ORA: 0x09; 0x05; 0x15; 0x0d; 0x1d; 0x19; 0x01; 0x11
	case ORA_IMMEDIATE: cpu.instruction.ORA.Immediate(cpu.memory_data); break;
	case ORA_ZERO_PAGE: cpu.instruction.ORA.Zero_Page(cpu.address); break;
	case ORA_ZERO_PAGE_X: cpu.instruction.ORA.Zero_Page_X(cpu.address); break;
	case ORA_ABSOLUTE: cpu.instruction.ORA.Absolute(cpu.address); break;
	case ORA_ABSOLUTE_X: cpu.instruction.ORA.Absolute_X(cpu.address); break;
	case ORA_ABSOLUTE_Y: cpu.instruction.ORA.Absolute_Y(cpu.address); break;
	case ORA_INDIRECT_X: cpu.instruction.ORA.Indirect_X(cpu.address); break;
	case ORA_INDIRECT_Y: cpu.instruction.ORA.Indirect_Y(cpu.address); break;

		// PHA: 0x48
	case PHA_IMPLIED: cpu.instruction.PHA.Implied(); break;

		// PHP: 0x08
	case PHP_IMPLIED: cpu.instruction.PHP.Implied(); break;

		// PLA: 0x68
	case PLA_IMPLIED: cpu.instruction.PLA.Implied(); break;

		// PLP: 0x28
	case PLP_IMPLIED: cpu.instruction.PLP.Implied(); break;

		// ROL: 0x2a; 0x26; 0x36; 0x2e; 0x3e;
	case ROL_ACCUMULATOR: cpu.instruction.ROL.Accumulator(); break;
	case ROL_ZERO_PAGE: cpu.instruction.ROL.Zero_Page(cpu.address); break;
	case ROL_ZERO_PAGE_X: cpu.instruction.ROL.Zero_Page_X(cpu.address); break;
	case ROL_ABSOLUTE: cpu.instruction.ROL.Absolute(cpu.address); break;
	case ROL_ABSOLUTE_X: cpu.instruction.ROL.Absolute_X(cpu.address); break;

		// ROR: 0x6a; 0x66; 0x76; 0x6e; 0x7e;
	case ROR_ACCUMULATOR: cpu.instruction.ROR.Accumulator(); break;
	case ROR_ZERO_PAGE: cpu.instruction.ROR.Zero_Page(cpu.address); break;
	case ROR_ZERO_PAGE_X: cpu.instruction.ROR.Zero_Page_X(cpu.address); break;
	case ROR_ABSOLUTE: cpu.instruction.ROR.Absolute(cpu.address); break;
	case ROR_ABSOLUTE_X: cpu.instruction.ROR.Absolute_X(cpu.address); break;

		// RTI: 0x40
	case RTI_IMPLIED: cpu.instruction.RTI.Implied(); break;

		// RTS: 0x60
	case RTS_IMPLIED: cpu.instruction.RTS.Implied(); break;

		// SBC: 0xe9; 0xe5; 0xf5; 0xed; 0xfd; 0xf9; 0xe1; 0xf1
	case SBC_IMMEDIATE: cpu.instruction.SBC.Immediate(cpu.memory_data); break;
	case SBC_ZERO_PAGE: cpu.instruction.SBC.Zero_Page(cpu.address); break;
	case SBC_ZERO_PAGE_X: cpu.instruction.SBC.Zero_Page_X(cpu.address); break;
	case SBC_ABSOLUTE: cpu.instruction.SBC.Absolute(cpu.address); break;
	case SBC_ABSOLUTE_X: cpu.instruction.SBC.Absolute_X(cpu.address); break;
	case SBC_ABSOLUTE_Y: cpu.instruction.SBC.Absolute_Y(cpu.address); break;
	case SBC_INDIRECT_X: cpu.instruction.SBC.Indirect_X(cpu.address); break;
	case SBC_INDIRECT_Y: cpu.instruction.SBC.Indirect_Y(cpu.address); break;

		// SEC: 0x38
	case SEC_IMPLIED: cpu.instruction.SEC.Implied(); break;

		// SED: 0xf8
	case SED_IMPLIED: cpu.instruction.SED.Implied(); break;

		// SEI: 0x78
	case SEI_IMPLIED: cpu.instruction.SEI.Implied(); break;

		// STA: 0x85; 0x95; 0x8d; 0x9d; 0x99; 0x81; 0x91
	case STA_ZERO_PAGE: cpu.instruction.STA.Zero_Page(cpu.address); break;
	case STA_ZERO_PAGE_X: cpu.instruction.STA.Zero_Page_X(cpu.address); break;
	case STA_ABSOLUTE: cpu.instruction.STA.Absolute(cpu.address); break;
	case STA_ABSOLUTE_X: cpu.instruction.STA.Absolute_X(cpu.address); break;
	case STA_ABSOLUTE_Y: cpu.instruction.STA.Absolute_Y(cpu.address); break;
	case STA_INDIRECT_X: cpu.instruction.STA.Indirect_X(cpu.address); break;
	case STA_INDIRECT_Y: cpu.instruction.STA.Indirect_Y(cpu.address); break;

		// STX: 0x86; 0x96; 0x8e
	case STX_ZERO_PAGE: cpu.instruction.STX.Zero_Page(cpu.address); break;
	case STX_ZERO_PAGE_Y: cpu.instruction.STX.Zero_Page_Y(cpu.address); break;
	case STX_ABSOLUTE: cpu.instruction.STX.Absolute(cpu.address); break;

		// STY: 0x84; 0x94; 0x8c
	case STY_ZERO_PAGE: cpu.instruction.STY.Zero_Page(cpu.address); break;
	case STY_ZERO_PAGE_X: cpu.instruction.STY.Zero_Page_X(cpu.address); break;
	case STY_ABSOLUTE: cpu.instruction.STY.Absolute(cpu.address); break;

		// TAX: 0xaa
	case TAX_IMPLIED: cpu.instruction.TAX.Implied(); break;

		// TAY: 0xa8
	case TAY_IMPLIED: cpu.instruction.TAY.Implied(); break;

		// TSX: 0xba
	case TSX_IMPLIED: cpu.instruction.TSX.Implied(); break;

		// TXA: 0x8a
	case TXA_IMPLIED: cpu.instruction.TXA.Implied(); break;

		// TXS: 0x9a
	case TXS_IMPLIED: cpu.instruction.TXS.Implied(); break;

		// TYA: 0x98
	case TYA_IMPLIED: cpu.instruction.TYA.Implied(); break;
	}
}

void MOS6502::Fire_Scanline() {
	//ushort temp_cpu_instruction_cycle = 0;
	//if (cpu_cycle_on_current_scanline < static_cast<int>(floor(CPU_CLOCK_CYCLES_PER_SCANLINE))) {
	//	temp_cpu_instruction_cycle++;
	//	cpu_cycle_on_current_scanline++;
	//}
	//else {
	//	// No adding of any cpu cycles here. Its why I put it in an if else statement

	//	if (fracion_portion_of_cpu_cycle == false) {
	//		fracion_portion_of_cpu_cycle = true;
	//	}
	//}
}

void MOS6502::Execute() {
	if (cpu.paused == false || (cpu.paused == true && Step == true)) {
		//if (Step == true)
			//MessageBox(NULL, "STEP = true", "NES Emulator", MB_ICONERROR | MB_OK);

		// One scanline
		temp_machine_cycles = 0;

		if (region == NTSC) {
			opcode = Read_Memory(REGISTERS::pc);
			Fire_Opcode(opcode);
			cycles += instruction_cycles;
			cpu_cycles_on_current_scanline += instruction_cycles;
			machine_cycles += instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
			instruction_machine_cycles = instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;

			do {
				if (temp_machine_cycles < instruction_machine_cycles) {
					temp_machine_cycles++;
					machine_cycles_on_current_scanline++;

					if (temp_machine_cycles % 4 == 0) {
						ppu.cycles++;
					}
				}
				else if (temp_machine_cycles == instruction_machine_cycles) {
					temp_machine_cycles = 0;

					if (cpu_cycles_on_current_scanline <= floor(cpu.NTSC_CPU_CYCLES_PER_SCANLINE)) {
						opcode = Read_Memory(REGISTERS::pc);
						Fire_Opcode(opcode);
						cycles += instruction_cycles;
						cpu_cycles_on_current_scanline += instruction_cycles;


						machine_cycles += instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
						instruction_machine_cycles = instruction_cycles * cpu.NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_CPU_CYCLE;
					}
				}
			} while (machine_cycles_on_current_scanline < NTSC_MACHINE_CYCLE_CLOCK_COUNTS_PER_SCANLINE); //(cycles < floor(cpu.NTSC_CPU_CYCLES_PER_SCANLINE));
		}
		/////////////////////////////////////////////////////////////////////////////

		//if (cycles >= floor(cpu.NTSC_CPU_CYCLES_PER_SCANLINE) && ppu.frame_count == 0 && show_once == false) {
		//	char Text[255];
		//	sprintf_s(Text, sizeof(Text), "cycles: %i \ncpu_cycles_on_current_scanline: %i \nppu.cycles: %i \nmachine_cycles: %i", cycles, cpu_cycles_on_current_scanline, ppu.cycles, machine_cycles);
		//	MessageBox(NULL, Text, "NES Emulator", MB_ICONERROR | MB_OK);
		//}

		if (cpu_cycles_on_current_scanline >= floor(cpu.NTSC_CPU_CYCLES_PER_SCANLINE)) {
			Render_Scanline(ppu.scanline);

			if (ppu.scanline == ppu.NTSC_POST_RENDER_SCANLINE)
				ppu.ppu_status.reg = 0;

			if (ppu.scanline == ppu.NTSC_VERTICAL_BLANK_BEGIN) {
				ppu.ppu_status.reg = ppu.ppu_status.Set_VBlank_Occuring_Flag();

				if (ppu.ppu_controller.Get_Execute_NMI_On_VBlank() == true)
					interrupt.NMI();
			}

			if (ppu.scanline == ppu.NTSC_PRE_RENDER_SCANLINE) {
				Blit_Screen();
				if (Step == true)
					Step = false;

				//char Text[255];
				//sprintf_s(Text, sizeof(Text), "cycles: %i \ncpu_cycles_on_current_scanline: %i \nppu.cycles: %i \nmachine_cycles: %i", cycles, cpu_cycles_on_current_scanline, ppu.cycles, machine_cycles);
				//MessageBox(NULL, Text, "NES Emulator", MB_ICONERROR | MB_OK);
				
				ppu.cycles = 0;
				cycles = 0;
				machine_cycles = 0;
				ppu.scanline = 0;
				ppu.ppu_status.reg = 0;
				ppu.frame_count++;
				if (ppu.frame_count == 1)
					ppu.odd_frame = true;
				else
					ppu.odd_frame = false;
			}
			else
				ppu.scanline++;

			machine_cycles_on_current_scanline = 0;
			cpu_cycles_on_current_scanline = 0;


			//if (ppu.frame_count == 0 && show_once == false) {
			//	show_once = true;
			//	char Text[255];
			//	sprintf_s(Text, sizeof(Text), "cycles: %i \ncpu_cycles_on_current_scanline: %i \nppu.cycles: %i \nmachine_cycles: %i", cycles, cpu_cycles_on_current_scanline, ppu.cycles, machine_cycles);
			//	MessageBox(NULL, Text, "NES Emulator", MB_ICONERROR | MB_OK);
			//}

		}
	}
}
