#pragma once

template <typename T>
class Singleton
{
protected:			Singleton(void)	{ }
protected: virtual ~Singleton(void)	{ }

public: static T& GetInstance(void) 
{ 
	static T pInstance;

	return pInstance;
}
};