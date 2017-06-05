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

		template<class TType>
		inline const TType *get(const StringView &name, bool ptr)
		{
			static_assert(std::is_base_of<Type, TType>::value, "TType must inherit from Type");
			auto type = get(name, ptr);
			if (type != nullptr)
				return static_cast<const TType*>(type);

			return nullptr;
		}

	private:
		std::string get_internal_name(const std::string &name, bool ptr);

		std::unordered_map<std::string, std::unique_ptr<Type>> entries_;
	};

}
