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

	struct Expr;

	class Type
	{
	public:
		inline virtual ~Type() {}

		virtual u64 size() const = 0;
		virtual bool is_ptr() const = 0;
		inline std::string name() const 
		{ 
			if(is_ptr())
				return name_ + "*";

			return name_; 
		}
		virtual std::string type_name() const = 0;
		inline TypeID id() const { return id_; }
		bool equals(const Type *other) const;
		bool can_convert_to(const Type *target) const;
		virtual const Expr *expr() const { return nullptr; };
		template<class T>
		inline const T *as_const() const
		{
			static_assert(std::is_base_of<Type, T>::value, "T must inherit from Type");
			return dynamic_cast<const T*>(this);
		}
	protected:
		inline Type(const std::string &name, TypeID id) : name_(name), id_(id) {}

	protected:
		std::string name_;
		TypeID id_;
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
		inline std::string type_name() const override { return "primitive"; };

		inline bool is_integral() const
		{
			switch (id_)
			{
			case brisk::TypeID::U8:
			case brisk::TypeID::I8:
			case brisk::TypeID::U16:
			case brisk::TypeID::I16:
			case brisk::TypeID::U32:
			case brisk::TypeID::I32:
			case brisk::TypeID::U64:
			case brisk::TypeID::I64:
				return true;
			default:
				return false;
			}
		}
	};

	struct FnDeclExpr;

	class FnType : public Type
	{
	public:
		inline FnType(const std::string &name, const FnDeclExpr &decl_expr) : Type(name, TypeID::Fn), decl_expr(decl_expr) {}
		inline FnType(const StringView &name, const FnDeclExpr &decl_expr) : Type(name.to_string(), TypeID::Fn), decl_expr(decl_expr) {}
		inline u64 size() const override { return 8; }
		inline bool is_ptr() const override { return false; }
		const Expr *expr() const override;
		inline std::string type_name() const override { return "function"; };

		const FnDeclExpr &decl_expr;
	};

	class PtrType : public Type
	{
	public:
		inline PtrType(const Type *type) : Type(type->name(), type->id()) {}
		inline u64 size() const override { return 8; }
		inline bool is_ptr() const override { return true; }
		const Expr *expr() const override { return type->expr(); };
		inline std::string type_name() const override { return type->type_name(); };

		const Type *type;
	};
}