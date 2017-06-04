#pragma once

#include "typedef.h"
#include "string_view.h"
#include "token_type.h"

namespace brisk {

	enum class TypeID : u16
	{
		Void = 0,
		U8 = 1,
		I8 = 2,
		U16 = 3,
		I16 = 4,
		U32 = 5,
		I32 = 6,
		U64 = 7,
		I64 = 8,
		Float = 9,
		Double = 10,
		Fn = 11,
		Struct = 12
	};

	class Type
	{
	public:
		inline virtual ~Type() {}

		virtual u64 size() const = 0;
		virtual bool is_ptr() const = 0;
		inline const std::string &name() const { return name_; }
		inline TypeID id() const { return id_; }
		
	protected:
		inline Type(const std::string &name, TypeID id) : name_(name), id_(id) {}

	protected:
		std::string name_;
		TypeID id_;
		bool is_ptr_;
	};

	class PrimitiveType : public Type
	{
	public:
		inline PrimitiveType(const std::string &name, TypeID id) : Type(name, id) {}
		inline u64 size() const override
		{
			switch (id_)
			{
			case brisk::TypeID::Void:
				return 0;
			case brisk::TypeID::U8:
				return 1;
			case brisk::TypeID::I8:
				return 1;
			case brisk::TypeID::U16:
				return 2;
			case brisk::TypeID::I16:
				return 2;
			case brisk::TypeID::U32:
				return 4;
			case brisk::TypeID::I32:
				return 4;
			case brisk::TypeID::U64:
				return 8;
			case brisk::TypeID::I64:
				return 8;
			case brisk::TypeID::Float:
				return 4;
			case brisk::TypeID::Double:
				return 8;
			default:
				return 0;
			}
		}

		inline bool is_ptr() const override { return false; }
	};

	class PtrType : public Type
	{
	public:
		inline PtrType(const Type *type) : Type(type->name(), type->id()) {}
		inline u64 size() const override { return 8; }
		inline bool is_ptr() const override { return true; }

		const Type *type;
	};
}