#include "type_table.h"

#include <sstream>

#include "ast/ast.h"
#include "file.h"
#include "brisk_exception.h"

namespace brisk {

	TypeTable::TypeTable()
	{
		add("void", std::make_unique<PrimitiveType>("void", TypeID::Void));
		add("void", std::make_unique<PtrType>(get("void", false)));

		add("u8", std::make_unique<PrimitiveType>("u8", TypeID::U8));
		add("i8", std::make_unique<PrimitiveType>("i8", TypeID::I8));
		add("u8", std::make_unique<PtrType>(get("u8", false)));
		add("i8", std::make_unique<PtrType>(get("i8", false)));

		add("u16", std::make_unique<PrimitiveType>("u16", TypeID::U16));
		add("i16", std::make_unique<PrimitiveType>("i16", TypeID::I16));
		add("u16", std::make_unique<PtrType>(get("u16", false)));
		add("i16", std::make_unique<PtrType>(get("i16", false)));

		add("u32", std::make_unique<PrimitiveType>("u32", TypeID::U32));
		add("i32", std::make_unique<PrimitiveType>("i32", TypeID::I32));
		add("u32", std::make_unique<PtrType>(get("u32", false)));
		add("i32", std::make_unique<PtrType>(get("i32", false)));

		add("u64", std::make_unique<PrimitiveType>("u64", TypeID::U64));
		add("i64", std::make_unique<PrimitiveType>("i64", TypeID::I64));
		add("u64", std::make_unique<PtrType>(get("u64", false)));
		add("i64", std::make_unique<PtrType>(get("i64", false)));

		add("float", std::make_unique<PrimitiveType>("float", TypeID::Float));
		add("float", std::make_unique<PtrType>(get("float", false)));

		add("double", std::make_unique<PrimitiveType>("double", TypeID::Double));
		add("double", std::make_unique<PtrType>(get("double", false)));
	}

	void TypeTable::add(const StringView &name, std::unique_ptr<Type> type)
	{
		add(name.to_string(), std::move(type));
	}

	void TypeTable::add(const std::string &name, std::unique_ptr<Type> type)
	{
		auto duplicate = get(name, type->is_ptr());
		if (duplicate != nullptr)
		{
			const auto message = "A " + type->type_name() + " with the name '" + name + "' has already been declared";

			if (duplicate->expr() != nullptr)
			{
				
				std::ostringstream ss;

				if (type->expr() != nullptr)
				{
					const auto &this_location = type->expr()->start;
					ss << "Row: " << this_location.row << " Column: " << this_location.column_start << "-" << this_location.column_end << std::endl;
					ss << "File: '" << this_location.file->path << "'" << std::endl;
				}

				const auto &duplicate_location = duplicate->expr()->start;
				ss << message << " at:" << std::endl;
				ss << "Row: " << duplicate_location.row << " Column: " << duplicate_location.column_start << "-" << duplicate_location.column_end << std::endl;
				ss << "File: '" << duplicate_location.file->path << "'" << std::endl;

				throw BriskException(ss.str());
			}

			throw BriskException("A type with the name '" + name + "' has already been declared");
		}

		entries_.insert(std::pair<std::string, std::unique_ptr<Type>>(get_internal_name(name, type->is_ptr()), std::move(type)));
	}

	const Type *TypeTable::get(const StringView &name, bool ptr)
	{
		return get(name.to_string(), ptr);
	}

	const Type *TypeTable::get(const std::string &name, bool ptr)
	{
		auto& it = entries_.find(get_internal_name(name, ptr));
		if (it != entries_.end())
		{
			return it->second.get();
		}

		return nullptr;
	}

	std::string TypeTable::get_internal_name(const std::string &name, bool ptr)
	{
		auto internal_name = name;
		if (ptr)
			internal_name = internal_name + "_ptr";

		return internal_name;
	}

}
