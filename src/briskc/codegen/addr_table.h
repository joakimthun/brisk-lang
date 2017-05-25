#pragma once

#include <string>
#include <unordered_map>

#include "typedef.h"
#include "string_view.h"

namespace brisk {

	struct AddrEntry
	{
		u64 sp_rel_addr;
	};

	class AddrTable
	{
	public:
		AddrTable(AddrTable *parent);

		bool root();
		void add(const StringView &name, u64 sp_rel_addr);
		const AddrEntry &find(const StringView &name);

	private:
		std::unordered_map<std::string, AddrEntry> entries_;
		AddrTable *parent_;
	};

}
