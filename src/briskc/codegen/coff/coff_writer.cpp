#include "coff_writer.h"

#include <cstring>
#include <ctime>

#include "file.h"

namespace brisk {
	namespace coff {

		CoffWriter::CoffWriter(MACHINE machine)
			:
			header_({0}),

			// At the beginning of the COFF string table are 4 bytes containing the total size (in bytes) of
			// the rest of the string table.This size includes the size field itself, so that the value in this
			// location would be 4 if no strings were present.
			string_table_size_(4)
		{
			header_.machine = static_cast<u16>(machine);
		}

		void CoffWriter::add_section(const std::string& name, SectionHeaderFlags flags, std::unique_ptr<ByteBuffer> content)
		{
			auto sh = SectionHeader{ 0 };

			for (auto i = 0u; i < name.size(); i++)
			{
				if (i > 7)
					break;

				sh.name[i] = name[i];
			}

			sh.flags = static_cast<u32>(flags);

			auto section = std::make_unique<CoffSection>();
			section->header = sh;

			if (content != nullptr)
			{
				sh.size = content->length();
				section->content = std::move(content);
			}

			sections_.insert(std::pair<std::string, std::unique_ptr<CoffSection>>(name, std::move(section)));
		}

		void CoffWriter::set_section_content(const std::string &section, std::unique_ptr<ByteBuffer> content)
		{
			auto it = sections_.find(section);
			if (it != sections_.end())
			{
				auto s = it->second.get();
				s->content.reset(content.release());
			}
		}

		u32 CoffWriter::add_symbol(const std::string &name, i16 scnum, SymbolTableMsEntryType type, SymbolTableEntryClass sclass, u32 value)
		{
			auto ste = SymbolTableEntry{ 0 };

			// The Name field in a symbol table consists of eight bytes that contain the name itself, if not
			// too long, or else give an offset into the String Table.To determine whether the name itself
			// or an offset is given, test the first four bytes for equality to zero.
			if (name.size() >= 8)
			{
				ste.entry.entry.offset = add_to_string_table(name);
			}
			else
			{
				for (auto i = 0u; i < name.size(); i++)
				{
					ste.entry.name[i] = name[i];
				}
			}

			ste.value = value;
			ste.scnum = scnum;
			ste.type = static_cast<u16>(type);
			ste.sclass = static_cast<u8>(sclass);

			symbols_.push_back(ste);
			return static_cast<u32>(symbols_.size() - 1);
		}

		void CoffWriter::add_relocation(const std::string &section, const RelocationDirective &relocation)
		{
			auto it = sections_.find(section);
			if (it != sections_.end())
			{
				auto s = it->second.get();
				s->relocations.push_back(relocation);
			}
		}

		void CoffWriter::write_to_disk(const std::string& path)
		{
			ByteBuffer buffer;

			header_.timdat = static_cast<u32>(std::time(nullptr));
			header_.nscns = static_cast<u16>(sections_.size());
			header_.nsyms = static_cast<u16>(symbols_.size());

			auto section_content_size = 0u;
			for (const auto& s : sections_)
			{
				s.second->header.size = s.second->content->length();

				section_content_size += s.second->content->length();
				section_content_size += s.second->relocations.size() * sizeof(RelocationDirective);
			}

			header_.symptr = sizeof(FileHeader) + (sizeof(SectionHeader) * header_.nscns) + section_content_size;

			buffer.write(&header_, sizeof(FileHeader));

			// Section headers
			auto scnptr = sizeof(FileHeader) + (sizeof(SectionHeader) * header_.nscns);
			for (const auto& s : sections_)
			{
				s.second->header.scnptr = scnptr;
				scnptr += s.second->content->length();

				s.second->header.nreloc = s.second->relocations.size();

				if (s.second->header.nreloc > 0)
				{
					s.second->header.relptr = scnptr;
					scnptr += s.second->header.nreloc * sizeof(RelocationDirective);
				}

				buffer.write(&s.second->header, sizeof(SectionHeader));
			}

			// Section data
			for (const auto& s : sections_)
			{
				buffer.write(s.second->content->data(), s.second->content->length());
				
				for (auto& r : s.second->relocations)
				{
					buffer.write(&r, sizeof(RelocationDirective));
				}
			}

			// Symbols
			for (const auto& s : symbols_)
			{
				buffer.write(&s, sizeof(SymbolTableEntry));
			}

			// String table
			buffer.write(&string_table_size_, 4);
			
			for (const auto& str : string_table_)
			{
				buffer.write(str.value.c_str(), str.value.size());
				buffer.write(static_cast<u8>('\0'));
			}

			write_file(path, buffer);
		}

		u32 CoffWriter::add_to_string_table(const std::string &name)
		{
			const auto offset = string_table_size_;

			string_table_.push_back(CoffString{
				name, 
				offset
			});

			// + 1 for the null-terminator
			string_table_size_ += name.size() + 1;

			return offset;
		}

	}
}
