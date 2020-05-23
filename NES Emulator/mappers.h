#pragma once

//Data Types
typedef unsigned char byte;
typedef unsigned short ushort;

//Function Prototypes
bool Setup_Mapper();
void Mapper_Write(ushort address, byte data);
void Mapper_1_Write(ushort address, byte data);
void Mapper_2_Write(ushort address, byte data);

extern byte Register[4];
extern int Sequence;
extern int Accumulator_Data;
extern bool High_PRGROM_Switching;
extern bool Low_PRGROM_Switching;
extern int Mapper1_Address;