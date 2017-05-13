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

		void write(const std::string &path)
		{
			auto header = FileHeader{ 0 };
			header.magic = 0x14c;
			auto now = std::time(nullptr);
			header.timdat = now;
			header.nscns = 1;
			header.nsyms = 1;
			header.symptr = sizeof(FileHeader) + sizeof(SectionHeader) + 10;

			auto cs = SectionHeader{ 0 };
			cs.name[0] = '.';
			cs.name[1] = 't';
			cs.name[2] = 'e';
			cs.name[3] = 'x';
			cs.name[4] = 't';

			cs.flags = static_cast<u32>(SectionHeaderFlags::STYP_TEXT);
			cs.size = 10;
			cs.scnptr = sizeof(FileHeader) + sizeof(SectionHeader);

			/*auto dir_s = SectionHeader{ 0 };
			dir_s.name[0] = '.';
			dir_s.name[1] = 'd';
			dir_s.name[2] = 'r';
			dir_s.name[3] = 'e';
			dir_s.name[4] = 'c';
			dir_s.name[5] = 't';
			dir_s.name[6] = 'v';
			dir_s.name[7] = 'e';

			auto dir_content = std::string("/DEFAULTLIB:\"LIBCMT\" /DEFAULTLIB:\"OLDNAMES\"");
			ByteBuffer dir_b;
			dir_b.write(dir_content.c_str(), dir_content.size());

			dir_s.flags = 1051136;
			dir_s.size = dir_b.length();
			dir_s.scnptr = sizeof(FileHeader) + sizeof(SectionHeader);*/

			

			ByteBuffer code;
			code.write(0x55);	// push ebp
			
			code.write(0x8B);	// mov ebp, esp
			code.write(0xEC);
			
			code.write(0xB8);	// mov eax, 64h
			code.write(0x64);
			code.write(0x00);
			code.write(0x00);
			code.write(0x00);
			
			code.write(0x5D);	// pop ebp
			code.write(0xC3);	// ret

			

			//auto ste_dir = SymbolTableEntry{ 0 };
			//ste_dir.entry.name[0] = '.';
			//ste_dir.entry.name[1] = 'd';
			//ste_dir.entry.name[2] = 'r';
			//ste_dir.entry.name[3] = 'e';
			//ste_dir.entry.name[4] = 'c';
			//ste_dir.entry.name[5] = 't';
			//ste_dir.entry.name[6] = 'v';
			//ste_dir.entry.name[7] = 'e';
			//ste_dir.scnum = 2;
			//ste_dir.type = 0;
			//ste_dir.sclass = 3;
			////ste_dir.numaux = 1;

			auto ste_main = SymbolTableEntry{ 0 };
			ste_main.entry.name[0] = '_';
			ste_main.entry.name[1] = 'm';
			ste_main.entry.name[2] = 'a';
			ste_main.entry.name[3] = 'i';
			ste_main.entry.name[4] = 'n';
			ste_main.scnum = 1;
			ste_main.type = 32;
			ste_main.sclass = 2;

			ByteBuffer buffer;
			buffer.write(&header, sizeof(FileHeader));
			buffer.write(&cs, sizeof(SectionHeader));
			buffer.write(code.data(), code.length());
			buffer.write(&ste_main, sizeof(SymbolTableEntry));

			buffer.write(0);
			buffer.write(0);
			buffer.write(0);
			buffer.write(0);

			write_file("C:/test/brisk.obj", buffer);

			auto soste = sizeof(SymbolTableEntry);

			read("C:/test/brisk.obj");
		}
	}
}
