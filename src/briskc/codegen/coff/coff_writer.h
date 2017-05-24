#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "typedef.h"
#include "coff.h"
#include "byte_buffer.h"

namespace brisk {
	namespace coff {

		struct CoffSection
		{
			SectionHeader header;
			std::vector<RelocationDirective> relocations;
			std::unique_ptr<ByteBuffer> content;
		};

		class CoffWriter
		{
		public:
			CoffWriter(MACHINE machine);

			void add_section(const std::string &name, SectionHeaderFlags flags, std::unique_ptr<ByteBuffer> content);
			void set_section_content(const std::string &section, std::unique_ptr<ByteBuffer> content);
			u32 add_symbol(const std::string &name, i16 scnum, SymbolTableMsEntryType type, SymbolTableEntryClass sclass, u32 value = 0);
			void add_relocation(const std::string &section, const RelocationDirective &relocation);
			void write_to_disk(const std::string &path);

		private:
			FileHeader header_;
			std::unordered_map<std::string, std::unique_ptr<CoffSection>> sections_;
			std::vector<SymbolTableEntry> symbols_;

			ByteBuffer buffer_;
		};

	}
}