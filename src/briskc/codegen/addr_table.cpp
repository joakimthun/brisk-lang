#include "addr_table.h"

namespace brisk {

	typedef std::pair<std::string, AddrEntry> addr_entry;

	AddrTable::AddrTable(AddrTable *parent)
		:
		parent_(parent)
	{
	}

	bool AddrTable::root()
	{
		return parent_ == nullptr;
	}

	void AddrTable::add(const StringView &name, u64 sp_rel_addr)
	{
		auto entry = AddrEntry{ 0 };
		entry.sp_rel_addr = sp_rel_addr;

		entries_.insert(addr_entry(name.to_string(), entry));
	}

	const AddrEntry &AddrTable::find(const StringView &name)
	{
		auto it = entries_.find(name.to_string());
		if (it != entries_.end())
		{
			return it->second;
		}

		// TODO: Handle this...
		if (root())
			throw "AddrTable::find: Not found!";

		return parent_->find(name);
	}

}
