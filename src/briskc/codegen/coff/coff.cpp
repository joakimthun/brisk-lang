#include "coff.h"

#include <cstring>
#include <ctime>

#include "file.h"
#include "byte_buffer.h"
#include "util.h"

namespace brisk {
	namespace coff {

		void read(const std::string &path)
		{
			auto file = open_file(path);
			auto content = file->content.get();
			auto header = FileHeader{ 0 };
			std::memcpy(&header, content, sizeof(FileHeader));
			content += sizeof(FileHeader);
			content += header.opthdr;
			
			std::vector<Section> sections;
			for (auto s = 0u; s < header.nscns; s++)
			{
				auto sh = Section{ 0 };
				std::memcpy(&sh.header, content, sizeof(SectionHeader));
				content += sizeof(SectionHeader);
				sh.content = file->content.get() + sh.header.scnptr;
				
				auto reloc_ptr = file->content.get() + sh.header.relptr;
				for (auto r = 0u; r < sh.header.nreloc; r++)
				{
					auto rd = RelocationDirective{ 0 };
					std::memcpy(&rd, reloc_ptr, sizeof(RelocationDirective));
					reloc_ptr += sizeof(RelocationDirective);
					sh.relocations.push_back(rd);
				}

				sections.push_back(sh);
			}

			std::vector<SymbolTableEntry> symbols;
			auto sym_ptr = file->content.get() + header.symptr;
			for (auto s = 0u; s < header.nsyms; s++)
			{
				auto se = SymbolTableEntry{ 0 };
				std::memcpy(&se, sym_ptr, sizeof(SymbolTableEntry));
				sym_ptr += sizeof(SymbolTableEntry);
				symbols.push_back(se);
			}

			auto brk = 0;
		}

		void write(const ByteBuffer& code, const std::string &path)
		{
			/*auto header = FileHeader{ 0 };
			header.machine = static_cast<u16>(MACHINE::IMAGE_FILE_MACHINE_AMD64);
			auto now = std::time(nullptr);
			header.timdat = now;
			header.nscns = 1;
			header.nsyms = 1;
			header.symptr = sizeof(FileHeader) + sizeof(SectionHeader) + code.length();*/

			/*auto cs = SectionHeader{ 0 };
			cs.name[0] = '.';
			cs.name[1] = 't';
			cs.name[2] = 'e';
			cs.name[3] = 'x';
			cs.name[4] = 't';

			cs.flags = static_cast<u32>(SectionHeaderFlags::STYP_TEXT);
			cs.size = code.length();
			cs.scnptr = sizeof(FileHeader) + sizeof(SectionHeader);*/

			/*auto ste_main = SymbolTableEntry{ 0 };
			ste_main.entry.name[0] = 'm';
			ste_main.entry.name[1] = 'a';
			ste_main.entry.name[2] = 'i';
			ste_main.entry.name[3] = 'n';
			ste_main.scnum = 1;
			ste_main.type = 32;
			ste_main.sclass = 2;*/

			/*ByteBuffer buffer;
			buffer.write(&header, sizeof(FileHeader));
			buffer.write(&cs, sizeof(SectionHeader));
			buffer.write(code.data(), code.length());
			buffer.write(&ste_main, sizeof(SymbolTableEntry));

			buffer.write(0);
			buffer.write(0);
			buffer.write(0);
			buffer.write(0);

			write_file(path, buffer);*/
		}
	}
}
