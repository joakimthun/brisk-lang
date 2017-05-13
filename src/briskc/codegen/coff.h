#pragma once

#include <string>
#include <vector>

#include "typedef.h"

namespace brisk {
	namespace coff {

#pragma pack(push, 1)
		
		enum class FileHeaderFlags : u16
		{
			RELFLG = 0x0001,	// If set, there is no relocation information in this file. This is usually clear for objects and set for executables.
			EXEC = 0x0002,		// If set, all unresolved symbols have been resolved and the file may be considered executable.
			LNNO = 0x0004,		// If set, all line number information has been removed from the file (or was never added in the first place).
			LSYMS = 0x0008,		// If set, all the local symbols have been removed from the file (or were never added in the first place).
			AR32WR = 0x0100		// Indicates that the file is 32-bit little endian
		};

		struct FileHeader
		{
			u16 magic;			// magic number
			u16 nscns;			// number of sections
			u32 timdat;			// time & date stamp, time_t
			u32 symptr;			// file pointer to symtab
			u32 nsyms;			// number of symtab entries
			u16 opthdr;			// sizeof(optional hdr)
			u16 flags;			// flags                    
		};

		enum class SectionHeaderFlags : u32
		{
			STYP_TEXT = 0x0020, // If set, indicates that this section contains only executable code.
			STYP_DATA = 0x0040, // If set, indicates that this section contains only initialized data.
			STYP_BSS = 0x0080	// If set, indicates that this section defines uninitialized data, and has no data stored in the coff file for it.
		};

		struct SectionHeader
		{
			char name[8]; // section name
			u32 paddr;    // physical address, aliased s_nlib
			u32 vaddr;    // virtual address
			u32 size;     // section size
			u32 scnptr;   // file ptr to raw data for section
			u32 relptr;   // file ptr to relocation
			u32 lnnoptr;  // file ptr to line numbers
			u16 nreloc;   // number of relocation entries
			u16 nlnno;    // number of line number entries
			u32 flags;    // flags
		};

		enum class RelocationType : u16
		{
			RELOC_ADDR32 = 6,	// Relocate a 32-bit absolute reference
			RELOC_REL32 = 20	// Relocate a 32-bit relative reference
		};

		struct RelocationDirective
		{
			u32 vaddr;   // address of relocation
			u32 symndx;  // symbol we're adjusting for
			u16 type;    // type of relocation
		};

		struct Section
		{
			SectionHeader header;
			std::vector<RelocationDirective> relocations;
			u8 *content;
		};

		struct SymbolTableEntry
		{
			union {
				char name[8];	// If the symbol's name is eight characters or less, it is stored in this field
				struct {
					u32 zeroes;	// If this field is zero, then the symbol name is found by using e_offset as an offset into the string table. If it is nonzero, then the name is in the e_name field.
					u32 offset;	// If e_zeroes is zero, this field contains the offset of the symbol name in the string table.
				} entry;
			} entry;
			u32 value;		// e value of the symbol. For example, if the symbol represents a function, this contains the address of the function.
			i16 scnum;		// The number of the section that this symbol belongs to. The first section in the section table is section one.
			u16 type;		// The type of the symbol. This is made up of a base type and a derived type. For example, "pointer to int" is "pointer to T" and "int".
			u8 sclass;		// This tells where and what the symbol represents.
			u8 numaux;		// Each symbol is allowed to have additional data that follows it in the symbol table. This field tells how many equivalent SYMENTs are used for aux entries. For most symbols, this is zero. 
		};

#pragma pack(pop)

		void read(const std::string &path);
		void write(const std::string &path);
	}
}
