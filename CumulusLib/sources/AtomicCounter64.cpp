//
// AtomicCounter64.cpp
//
// Library: Foundation
// Package: Core
// Module:  AtomicCounter64
//

#include "AtomicCounter64.h"


namespace Cumulus {


#if POCO_OS == POCO_OS_WINDOWS_NT
//
// Windows
//
AtomicCounter64::AtomicCounter64():
	_counter(0)
{
}

	
AtomicCounter64::AtomicCounter64(AtomicCounter64::ValueType initialValue):
	_counter(initialValue)
{
}


AtomicCounter64::AtomicCounter64(const AtomicCounter64& counter):
	_counter(counter.value())
{
}


AtomicCounter64::~AtomicCounter64()
{
}


AtomicCounter64& AtomicCounter64::operator = (const AtomicCounter64& counter)
{
	InterlockedExchange64(&_counter, counter.value());
	return *this;
}

	
AtomicCounter64& AtomicCounter64::operator = (AtomicCounter64::ValueType value)
{
	InterlockedExchange64(&_counter, value);
	return *this;
}

AtomicCounter64& AtomicCounter64::operator += (const AtomicCounter64& counter)
{
	InterlockedExchangeAdd64(&_counter, counter.value());
	return *this;
}

	
AtomicCounter64& AtomicCounter64::operator += (AtomicCounter64::ValueType value)
{
	InterlockedExchangeAdd64(&_counter, value);
	return *this;
}

#elif POCO_OS == POCO_OS_MAC_OS_X
//
// Mac OS X
//
AtomicCounter64::AtomicCounter64():
	_counter(0)
{
}

	
AtomicCounter64::AtomicCounter64(AtomicCounter64::ValueType initialValue):
	_counter(initialValue)
{
}


AtomicCounter64::AtomicCounter64(const AtomicCounter64& counter):
	_counter(counter.value())
{
}


AtomicCounter64::~AtomicCounter64()
{
}


AtomicCounter64& AtomicCounter64::operator = (const AtomicCounter64& counter)
{
	_counter = counter.value();
	return *this;
}

	
AtomicCounter64& AtomicCounter64::operator = (AtomicCounter64::ValueType value)
{
	_counter = value;
	return *this;
}

AtomicCounter64& AtomicCounter64::operator += (const AtomicCounter64& counter)
{
    OSAtomicAdd64(&_counter, counter.value());
	return *this;
}

	
AtomicCounter64& AtomicCounter64::operator += (AtomicCounter64::ValueType value)
{
    OSAtomicAdd64(&_counter, value);
	return *this;
}


#elif defined(POCO_HAVE_GCC_ATOMICS)
//
// GCC 4.1+ atomic builtins.
//
AtomicCounter64::AtomicCounter64():
	_counter(0)
{
}

	
AtomicCounter64::AtomicCounter64(AtomicCounter64::ValueType initialValue):
	_counter(initialValue)
{
}


AtomicCounter64::AtomicCounter64(const AtomicCounter64& counter):
	_counter(counter.value())
{
}


AtomicCounter64::~AtomicCounter64()
{
}


AtomicCounter64& AtomicCounter64::operator = (const AtomicCounter64& counter)
{
	__sync_lock_test_and_set(&_counter, counter.value());
	return *this;
}

	
AtomicCounter64& AtomicCounter64::operator = (AtomicCounter64::ValueType value)
{
	__sync_lock_test_and_set(&_counter, value);
	return *this;
}

AtomicCounter64& AtomicCounter64::operator += (const AtomicCounter64& counter)
{
	return __sync_add_and_fetch(&_counter, counter.value());
}

	
AtomicCounter64& AtomicCounter64::operator += (AtomicCounter64::ValueType value)
{
	return __sync_add_and_fetch(&_counter, value);
}


#else
//
// Generic implementation based on FastMutex
//
AtomicCounter64::AtomicCounter64()
{
	_counter.value = 0;
}

	
AtomicCounter64::AtomicCounter64(AtomicCounter64::ValueType initialValue)
{
	_counter.value = initialValue;
}


AtomicCounter64::AtomicCounter64(const AtomicCounter64& counter)
{
	_counter.value = counter.value();
}


AtomicCounter64::~AtomicCounter64()
{
}


AtomicCounter64& AtomicCounter64::operator = (const AtomicCounter64& counter)
{
	Poco::FastMutex::ScopedLock lock(_counter.mutex);
	_counter.value = counter.value();
	return *this;
}

	
AtomicCounter64& AtomicCounter64::operator = (AtomicCounter64::ValueType value)
{
	Poco::FastMutex::ScopedLock lock(_counter.mutex);
	_counter.value = value;
	return *this;
}

AtomicCounter64& AtomicCounter64::operator += (const AtomicCounter64& counter)
{
	Poco::FastMutex::ScopedLock lock(_counter.mutex);
	_counter.value += counter.value();
	return *this;
}

	
AtomicCounter64& AtomicCounter64::operator += (AtomicCounter64::ValueType value)
{
	Poco::FastMutex::ScopedLock lock(_counter.mutex);
	_counter.value += value;
	return *this;
}

#endif // POCO_OS

#if (POCO_OS == POCO_OS_WINDOWS_NT) || (POCO_OS == POCO_OS_MAC_OS_X) || defined(POCO_HAVE_GCC_ATOMICS)

inline bool AtomicCounter64::CompBigAndSwap(const AtomicCounter64& counter)
{
    Poco::FastMutex::ScopedLock lock(_mutex);
	bool result = false;
    if(_counter < counter.value()){
        _counter = counter.value();
        result = true;
    }
	return result;
}

inline bool AtomicCounter64::CompBigAndSwap(const ValueType value)
{
    Poco::FastMutex::ScopedLock lock(_mutex);
	bool result = false;
    if(_counter < value){
        _counter = value;
        result = true;
    }
	return result;
}

#else

bool AtomicCounter64::CompBigAndSwap(const AtomicCounter64& counter) 
{
    Poco::FastMutex::ScopedLock lock(_counter.mutex);
	bool result = false;
    if(_counter.value < counter.value()){
        _counter.value = counter.value();
        result = true;
    }
	return result;
}

bool AtomicCounter64::CompBigAndSwap(const ValueType value) 
{
    Poco::FastMutex::ScopedLock lock(_counter.mutex);
	bool result = false;
    if(_counter.value < value){
        _counter.value = value;
        result = true;
    }
	return result;
}

#endif // POCO_OS

} // namespace Cumulus
