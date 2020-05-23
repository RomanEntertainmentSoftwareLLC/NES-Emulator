#pragma once

//Data Types
typedef unsigned char byte;
typedef unsigned short ushort;

class CPU_Memory
{
private:
	static byte Read_PRGROM(ushort address);
	static byte Read_SRAM(ushort address);
	static byte Read_EXP_ROM(ushort address);
	static byte Read_CPU_Test_Mode(ushort address);
	static byte Read_IO_Registers(ushort address);
	static byte Read_PPU_Registers(ushort address);
	static byte Read_RAM(ushort address);

	static void Write_PRGROM(ushort address, byte data);
	static void Write_SRAM(ushort address, byte data);
	static void Write_EXP_Rom(ushort address, byte data);
	static void Write_CPU_Test_Mode(ushort address, byte data);
	static void Write_IO_Registers(ushort address, byte data);
	static void Write_PPU_Registers(ushort address, byte data);
	static void Write_RAM(ushort address, byte data);
protected:
	static void Memory_Mirror(unsigned short address, unsigned short start_address_chunk, unsigned short end_address_chunk, unsigned short start_address_mirror, unsigned short end_address_mirror);

public:
	CPU_Memory();
	~CPU_Memory();
	static byte Read_Memory(ushort address);
	static void Write_Memory(ushort address, byte data);

	static byte memory_bank[0x10000];

	//static byte controller_1_data; // This is collected where you are getting user input by 'Or'ing BIT_0 to BIT_7
	//  7  bit  0
	//	---- ----
	//	ABSS UDLR
	//	|||| ||||
	//	|||| |||+-Right
	//	|||| ||+--Left
	//	|||| |+---Down
	//	|||| +----Up
	//  |||+------Start
	//  ||+-------Select
	//  |+--------B
	//  +---------A

	//static byte controller_2_data; // This is collected where you are getting user input by 'Or'ing BIT_0 to BIT_7
	//  7  bit  0
	//	---- ----
	//	ABSS UDLR
	//	|||| ||||
	//	|||| |||+-Right
	//	|||| ||+--Left
	//	|||| |+---Down
	//	|||| +----Up
	//  |||+------Start
	//  ||+-------Select
	//  |+--------B
	//  +---------A

	static byte controller_1_strobe;
	static byte controller_2_strobe;

};