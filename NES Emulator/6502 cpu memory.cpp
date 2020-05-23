#include "6502 cpu memory.h"
#include "MMC.h"
#include "Controls.h"
#include "ppu.h"
#include "bits.h"
#include "mappers.h"
#include "Main.h"

byte CPU_Memory::controller_1_strobe = 0;
byte CPU_Memory::controller_2_strobe = 0;

byte CPU_Memory::memory_bank[0x10000];

CPU_Memory::CPU_Memory() {

}

CPU_Memory::~CPU_Memory() {

}

void CPU_Memory::Memory_Mirror(unsigned short address, unsigned short start_address_chunk, unsigned short end_address_chunk, unsigned short start_address_mirror, unsigned short end_address_mirror) {
	// Note: This is not a physical write to the address per say. It mearly mirrors the "already written" address to
	//       the missing locations in memory where its suppose to be mirrored!
	//
	// address: The address being written that will be mirrored within a mirror range
	//			Ex. If the range of the chunk is between 0x0000 and 0x07ff,
	//              the mirror is between 0x0800 and 0x1fff,
	//				and the address that was written to is at 0x0000, then
	//				the data at 0x0000 will be mirrored at 0x0800, 0x1000, and 0x1800.
	//			Note: the address must be in between the start address chunk and end address chunk or it won't make sense.
	// start_address_chunk: The beginning range of where the address was written to enlies.
	// end_address_chunk: The end range of where the address was written to enlies.
	// start_address_mirror: The beginning section of where to mirror the written address multiple times in chunks.
	// end_address_mirror: The end section of where to mirror the written address multiple times in chunks.

	if (address >= start_address_chunk && address <= end_address_chunk) {
		unsigned short mirrorChunkSize = (end_address_chunk - start_address_chunk) + 1;
		int offset = address % mirrorChunkSize;
		int numberOfMirrors = ((end_address_mirror - start_address_mirror) / mirrorChunkSize) + 1;
		int numberOfTimesMirrored = 0;

		if (numberOfMirrors > 0) {
			for (int i = 0; i < numberOfMirrors; i++) {
				memory_bank[start_address_mirror + offset + (mirrorChunkSize * i)] = memory_bank[start_address_chunk + offset + (mirrorChunkSize * i)];
				numberOfTimesMirrored++;
			}
		}
	}
}

byte CPU_Memory::Read_Memory(ushort address) {
	if ((address >= 0x8000) && (address <= 0xffff)) { return Read_PRGROM(address); }				// 0x8000 to 0xffff (PRG-ROM)
	else if ((address >= 0x6000) && (address <= 0x7fff)) { return Read_SRAM(address); }				// 0x6000 to 0x7fff (Battery Backed Save aka SRAM aka Work RAM)
	else if ((address >= 0x4020) && (address <= 0x5fff)) { return Read_EXP_ROM(address); }			// 0x4020 to 0x5fff (EXP ROM)
	else if ((address >= 0x4018) && (address <= 0x401f)) { return Read_CPU_Test_Mode(address); }	// 0x4018 to 0x401f (APU and I/O functionality that is normally disabled)
	else if ((address >= 0x4000) && (address <= 0x4017)) { return Read_IO_Registers(address); }		// 0x4000 To 0x4017
	else if ((address >= 0x2000) && (address <= 0x3fff)) { return Read_PPU_Registers(address); }	// 0x2000 to 0x3fff (NES PPU registers)
	else if ((address >= 0x0000) && (address <= 0x1fff)) { return Read_RAM(address); }				// 0x0000 to 0x1FFF (2KB internal RAM)

	return 0;
}

void CPU_Memory::Write_Memory(ushort address, byte data) {
	memory_bank[address] = data;

	if ((address >= 0x8000) && (address <= 0xffff)) { Write_PRGROM(address, data); }				// 0x8000 to 0xffff (PRG-ROM)
	else if ((address >= 0x6000) && (address <= 0x7fff)) { Write_SRAM(address, data); }				// 0x6000 to 0x7fff (Battery Backed Save aka SRAM aka Work RAM)
	else if ((address >= 0x4020) && (address <= 0x5fff)) { Write_EXP_Rom(address, data); }			// 0x4020 to 0x5fff (Expansion ROM)
	else if ((address >= 0x4018) && (address <= 0x401f)) { Write_CPU_Test_Mode(address, data); }	// 0x4018 to 0x401f (APU and I/O functionality that is normally disabled)
	else if ((address >= 0x4000) && (address <= 0x4017)) { Write_IO_Registers(address, data); }		// 0x4000 to 0x4017 (I/O Registers for APU, joypad, and IRQ interrupts)
	else if ((address >= 0x2000) && (address <= 0x3fff)) { Write_PPU_Registers(address, data); }	// 0x2000 To 0x3fff (NES PPU registers)
	else if ((address >= 0x0000) && (address <= 0x1fff)) { Write_RAM(address, data); }				// 0x0000 To 0x1FFF (2KB internal RAM)
}