#pragma once

#include "Logo2Core.h"

namespace Logo2 {
	enum class SymbolType {
		Variable,
		Function,
		Argument,
	};

	enum class SymbolFlags {
		None = 0,
		Const = 1,
	};
	DEFINE_ENUM_FLAG_OPERATORS(SymbolFlags);

	struct Symbol {
		std::string Name;
		SymbolType Type;
		SymbolFlags Flags;
	};

	class SymbolTable {
	public:
		explicit SymbolTable(SymbolTable* parent = nullptr);
		bool AddSymbol(Symbol sym);
		Symbol const* FindSymbol(std::string const& name, bool localOnly = false) const;

	private:
		std::unordered_map<std::string, Symbol> m_Symbols;
		SymbolTable* m_Parent;
	};
}

