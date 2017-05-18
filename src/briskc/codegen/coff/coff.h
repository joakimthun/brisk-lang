#pragma once

#include <string>
#include <vector>

#include "typedef.h"

namespace brisk {

	class ByteBuffer;

	namespace coff {

#pragma pack(push, 1)

		enum class MACHINE : u16
		{
			IMAGE_FILE_MACHINE_I386 = 0x014c, // x86
			IMAGE_FILE_MACHINE_IA64 = 0x0200, // Intel Itanium
			IMAGE_FILE_MACHINE_AMD64 = 0x8664 // x64
		};

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
			u16 machine;		// machine
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
			IMAGE_REL_AMD64_REL32 = 4,	
			RELOC_ADDR32 = 6,			// Relocate a 32-bit absolute reference
			RELOC_REL32 = 20			// Relocate a 32-bit relative reference
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
							// A number representing type. Microsoft tools set this field to 0x20 (function) or 0x0 (not a function).

			u8 sclass;		// This tells where and what the symbol represents.
			u8 numaux;		// Each symbol is allowed to have additional data that follows it in the symbol table. This field tells how many equivalent SYMENTs are used for aux entries. For most symbols, this is zero. 
		};

		enum class SymbolTableEntryType : u16
		{
			IMAGE_SYM_TYPE_NULL = 0,	// No type information or unknown base type. Microsoft tools use this setting.
			IMAGE_SYM_TYPE_VOID = 1,	// No valid type; used with void pointers and functions.
			IMAGE_SYM_TYPE_CHAR = 2,	// Character(signed byte).
			IMAGE_SYM_TYPE_SHORT = 3,	// Two - byte signed integer.
			IMAGE_SYM_TYPE_INT = 4,		// Natural integer type(normally four bytes in Windows NT).
			IMAGE_SYM_TYPE_LONG = 5,	// Four - byte signed integer.
			IMAGE_SYM_TYPE_FLOAT = 6,	// Four - byte floating - point number.
			IMAGE_SYM_TYPE_DOUBLE = 7,	// Eight - byte floating - point number.
			IMAGE_SYM_TYPE_STRUCT = 8,	//Structure.
			IMAGE_SYM_TYPE_UNION = 9,	// Union.
			IMAGE_SYM_TYPE_ENUM = 10,	// Enumerated type.
			IMAGE_SYM_TYPE_MOE = 11,	// Member of enumeration(a specific value).
			IMAGE_SYM_TYPE_BYTE = 12,	// Byte; unsigned one - byte integer.
			IMAGE_SYM_TYPE_WORD = 13,	// Word; unsigned two - byte integer.
			IMAGE_SYM_TYPE_UINT = 14,	// Unsigned integer of natural size(normally, four bytes).
			IMAGE_SYM_TYPE_DWORD = 15	// Unsigned four - byte integer.
		};

		// The most significant byte specifies whether the symbol is a pointer to, function returning,
		// or array of the base type specified in the least significant byte.Microsoft tools use this
		// field only to indicate whether or not the symbol is a function, so that the only two
		// resulting values are 0x0 and 0x20 for the Type field.However, other tools can use this
		// field to communicate more information.
		enum class SymbolTableMsEntryType : u16
		{
			IMAGE_SYM_TYPE_NULL = 0,
			IMAGE_SYM_DTYPE_FUNCTION = 0x20
		};

		enum class SymbolTableEntryClass : u8
		{
			IMAGE_SYM_CLASS_NULL = 0,				// No storage class assigned.
			IMAGE_SYM_CLASS_AUTOMATIC = 1,			// Automatic(stack) variable.The Value field specifies stack frame offset.

			// Used by Microsoft tools for external symbols.The Value field indicates the size if the section number is IMAGE_SYM_UNDEFINED(0).
			// If the section number is not 0, then the Value field specifies the offset within the section.
			IMAGE_SYM_CLASS_EXTERNAL = 2, 
			
			// The Value field specifies the offset of the symbol within the section.If the Value is 0, then the
			// symbol represents a section name.
			IMAGE_SYM_CLASS_STATIC = 3, 
			IMAGE_SYM_CLASS_REGISTER = 4,			// Register variable.The Value field specifies register number.
			IMAGE_SYM_CLASS_EXTERNAL_DEF = 5,		// Symbol is defined externally.
			IMAGE_SYM_CLASS_LABEL = 6,				// Code label defined within the module.The Value field specifies the offset of the symbol within the section.
			IMAGE_SYM_CLASS_UNDEFINED_LABEL = 7,	// Reference to a code label not defined.
			IMAGE_SYM_CLASS_MEMBER_OF_STRUCT = 8,	// Structure member.The Value field specifies nth member.
			IMAGE_SYM_CLASS_ARGUMENT = 9,			// Formal argument (parameter)of a function. The Value field specifies nth argument.
			IMAGE_SYM_CLASS_STRUCT_TAG = 10,		// Structure tag - name entry.
			IMAGE_SYM_CLASS_MEMBER_OF_UNION = 11,	// Union member.The Value field specifies nth member.
			IMAGE_SYM_CLASS_UNION_TAG = 12,			// Union tag - name entry.
			IMAGE_SYM_CLASS_TYPE_DEFINITION = 13,	// Typedef entry.
			IMAGE_SYM_CLASS_UNDEFINED_STATIC = 14,	// Static data declaration.
			IMAGE_SYM_CLASS_ENUM_TAG = 15,			// Enumerated type tagname entry.
			IMAGE_SYM_CLASS_MEMBER_OF_ENUM = 16,	// Member of enumeration. Value specifies nth member.
			IMAGE_SYM_CLASS_REGISTER_PARAM = 17,	// Register parameter.
			IMAGE_SYM_CLASS_BIT_FIELD = 18,			// Bit - field reference. Value specifies nth bit in the bit field.
			IMAGE_SYM_CLASS_BLOCK = 100,			// A.bb(beginning of block) or .eb(end of block)record.Value is the relocatable address of the code location.
			
			// Used by Microsoft tools for symbol records that define the extent of a function :
			// begin function(named.bf), end function(.ef), and lines in function(.lf).
			// For.lf records, Value gives the number of source lines in the function.For.ef records, Value gives the
			// size of function code.
			IMAGE_SYM_CLASS_FUNCTION = 101,			
		
			IMAGE_SYM_CLASS_END_OF_STRUCT = 102,	// End of structure entry.

			// Used by Microsoft tools, as well as traditional COFF format, for the source - file symbol record.
			// The symbol is followed by auxiliary records that name the file.
			IMAGE_SYM_CLASS_FILE = 103,				

			IMAGE_SYM_CLASS_SECTION = 104,			// Definition of a section (Microsoft tools use STATIC storage class instead).
			IMAGE_SYM_CLASS_WEAK_EXTERNAL = 105		// Weak external.
		};

#pragma pack(pop)

		void read(const std::string &path);
		void write(const ByteBuffer& code, const std::string &path);
	}
}
