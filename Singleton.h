#pragma once
#pragma once

template <typename T> class Singleton
{
public:
	Singleton()
	{
		assert(!mSingleton);
		mSingleton = static_cast<T*>(this);
	}

	~Singleton()
	{
		assert(mSingleton);
		mSingleton = nullptr;
	}

	Singleton(const Singleton<T>&) = delete;
	Singleton& operator=(const Singleton<T>&) = delete;


	static T& GetSingleton()
	{
		return *mSingleton;
	}
	static T* Get()
	{
		return mSingleton;
	}


protected:
	static T* mSingleton;
};

template<typename T> T* Singleton<T>::mSingleton = nullptr;