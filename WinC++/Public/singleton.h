#pragma once

template<class T>
class singleton
{
public:
	static T* instance() {
		if (m_t) return m_t;
		m_t = new T;
		return m_t;
	}
	static void destroy() { 
		if(m_t)
			delete m_t; 
		m_t = nullptr;
	};
private:
	singleton() {};
	virtual ~singleton() {};
	singleton(const singleton&) {};
	singleton& operator=(const singleton&) {};
private:
	static T* m_t;
};

template<class T> 
T* singleton<T>::m_t = nullptr;