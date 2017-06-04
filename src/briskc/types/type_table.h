#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "string_view.h"
#include "typedef.h"
#include "type.h"

namespace brisk {

	class TypeTable
	{
	public:
		TypeTable();

		void add(const StringView &name, std::unique_ptr<Type> type);
		void add(const std::string &name, std::unique_ptr<Type> type);
		const Type *get(const StringView &name, bool ptr);
		const Type *get(const std::string &name, bool ptr);

	private:
		std::string get_internal_name(const std::string &name, bool ptr);

		std::unordered_map<std::string, std::unique_ptr<Type>> entries_;
	};

}
