#include "coff_writer.h"

#include <cstring>
#include <ctime>

#include "file.h"

namespace brisk {
	namespace coff {

		CoffWriter::CoffWriter(MACHINE machine)
			:
			header_({0})
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

		u32 CoffWriter::add_symbol(const std::string &name, i16 scnum, SymbolTableMsEntryType type, SymbolTableEntryClass sclass)
		{
			auto ste = SymbolTableEntry{ 0 };
			for (auto i = 0u; i < name.size(); i++)
			{
				if (i > 7)
					break;

				ste.entry.name[i] = name[i];
			}

			ste.scnum = scnum;
			ste.type = static_cast<u16>(type);
			ste.sclass = static_cast<u8>(sclass);

			symbols_.push_back(ste);
			return symbols_.size() - 1;
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

			header_.timdat = std::time(nullptr);
			header_.nscns = static_cast<u16>(sections_.size());
			header_.nsyms = static_cast<u16>(symbols_.size());

			auto section_content_size = 0u;
			for (auto& s : sections_)
			{
				s.second->header.size = s.second->content->length();

				section_content_size += s.second->content->length();
				section_content_size += s.second->relocations.size() * sizeof(RelocationDirective);
			}

			header_.symptr = sizeof(FileHeader) + (sizeof(SectionHeader) * header_.nscns) + section_content_size;

			buffer.write(&header_, sizeof(FileHeader));

			auto scnptr = sizeof(FileHeader) + (sizeof(SectionHeader) * header_.nscns);
			for (auto& s : sections_)
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

			for (auto& s : sections_)
			{
				buffer.write(s.second->content->data(), s.second->content->length());
				
				for (auto& r : s.second->relocations)
				{
					buffer.write(&r, sizeof(RelocationDirective));
				}
			}

			for (auto& s : symbols_)
			{
				buffer.write(&s, sizeof(SymbolTableEntry));
			}

			//for (auto& s : sections_)
			//{
				buffer.write(0);
				buffer.write(0);
				buffer.write(0);
				buffer.write(0);
			//}

			write_file(path, buffer);
		}

	}
}
