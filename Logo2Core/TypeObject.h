#pragma once

#include "Value.h"

namespace Logo2 {
	enum class TypeObjectType {
		Enum,
		Class,
		Struct,
		Union,
	};

	enum class MemberType {
		Method,
		Field,
		Event,
		Ctor,
		Operator,
	};

	enum class MemberFlags {
		None = 0,
		Const = 1,
		Static = 2,
		ReadOnly = 4,
	};
	DEFINE_ENUM_FLAG_OPERATORS(MemberFlags);

	struct MemberInfo {
		std::string Name;
		MemberType Type;
		MemberFlags Flags;
	};

	struct FieldInfo : MemberInfo {
	};

	class TypeObject {
	public:
		TypeObject(std::string fullName, TypeObjectType type);

		FieldInfo const* AddField(std::string name, MemberFlags flags, Value = {});

	private:
		std::string m_Name;
		TypeObjectType m_Type;
		std::unordered_map<std::string, std::unique_ptr<MemberInfo>> m_Members;
	};

}
