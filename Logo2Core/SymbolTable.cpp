#include "pch.h"
#include "SymbolTable.h"

using namespace Logo2;

SymbolTable::SymbolTable(SymbolTable* parent) : m_Parent(parent) {
}

bool SymbolTable::AddSymbol(Symbol sym) {
	return m_Symbols.insert({ sym.Name, std::move(sym) }).second;
}

Symbol const* SymbolTable::FindSymbol(std::string const& name, bool localOnly) const {
	if (auto it = m_Symbols.find(name); it != m_Symbols.end())
		return &(it->second);

	return m_Parent && !localOnly ? m_Parent->FindSymbol(name) : nullptr;
}
