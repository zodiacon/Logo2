#include "pch.h"
#include "SymbolTable.h"

SymbolTable::SymbolTable(SymbolTable* parent) : m_Parent(parent) {
}

bool SymbolTable::AddSymbol(Symbol sym) {
	return m_Symbols.insert({ sym.Name, std::move(sym) }).second;
}

Symbol const* SymbolTable::FindSymbol(std::string const& name) const {
	if (auto it = m_Symbols.find(name); it != m_Symbols.end())
		return &(it->second);

	return m_Parent ? m_Parent->FindSymbol(name) : nullptr;
}
