#pragma once

#include "SDLGlobal.h"

namespace SDL3 {
	template<typename T, typename Free>
	class SDLObject abstract {
	public:
		SDLObject(const SDLObject&) = delete;
		SDLObject& operator=(const SDLObject&) = delete;

		SDLObject(SDLObject&& other) : m_p(other.m_p), m_Owner(other.m_Owner) {
			other.m_p = nullptr;
		}
		SDLObject& operator=(SDLObject&& other) {
			if (this != &other) {
				if (m_p && m_Owner)
					Free(m_p);
				m_p = other.m_p;
				m_Owner = other.m_Owner;
				other.m_p = nullptr;
			}
			return *this;
		}

		SDLObject(T* p = nullptr, bool owner = true) : m_p(p), m_Owner(owner) {}

		~SDLObject() {
			if(m_Owner && m_p)
				Free(m_p);
		}

		T* Get() const {
			return m_p;
		}

		operator T const* () const {
			return Get();
		}

		operator T* () {
			return Get();
		}

		operator bool() const {
			return m_p != nullptr;
		}

	protected:
		void Set(T* p) {
			if (p == nullptr && SDL::s_EnableExceptions)
				throw SDLException();
			m_p = p;
			m_Owner = true;
		}


	private:
		T* m_p;
		bool m_Owner;
	};
}

