//
// AtomicCounter64.h
//
// Library: Foundation
// Package: Core
// Module:  AtomicCounter64
//
// Definition of the AtomicCounter64 class.
//


#ifndef Foundation_AtomicCounter64_INCLUDED
#define Foundation_AtomicCounter64_INCLUDED


#include "Poco/Foundation.h"
#if POCO_OS == POCO_OS_WINDOWS_NT
#include "Poco/UnWindows.h"
#elif POCO_OS == POCO_OS_MAC_OS_X
#include <libkern/OSAtomic.h>
#elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2) || __GNUC__ > 4) && (defined(__x86_64__) || defined(__i386__))
#if !defined(POCO_HAVE_GCC_ATOMICS) && !defined(POCO_NO_GCC_ATOMICS)
#define POCO_HAVE_GCC_ATOMICS
#endif
#elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3) || __GNUC__ > 4)
#if !defined(POCO_HAVE_GCC_ATOMICS) && !defined(POCO_NO_GCC_ATOMICS)
#define POCO_HAVE_GCC_ATOMICS
#endif
#endif // POCO_OS
#include "Poco/Mutex.h"


namespace Cumulus {


class Foundation_API AtomicCounter64
	/// This class implements a simple counter, which
	/// provides atomic operations that are safe to
	/// use in a multithreaded environment.
	///
	/// Typical usage of AtomicCounter64 is for implementing
	/// reference counting and similar things.
	///
	/// On some platforms, the implementation of AtomicCounter64
	/// is based on atomic primitives specific to the platform
	/// (such as InterlockedIncrement, etc. on Windows), and
	/// thus very efficient. On platforms that do not support
	/// atomic primitives, operations are guarded by a Poco::FastMutex.
	///
	/// The following platforms currently have atomic
	/// primitives:
	///   - Windows
	///   - Mac OS X
	///   - GCC 4.1+ (Intel platforms only)
{
public:
	typedef Poco::Int64 ValueType; /// The underlying integer type.
	
	AtomicCounter64();
		/// Creates a new AtomicCounter64 and initializes it to zero.
		
	explicit AtomicCounter64(ValueType initialValue);
		/// Creates a new AtomicCounter64 and initializes it with
		/// the given value.
	
	AtomicCounter64(const AtomicCounter64& counter);
		/// Creates the counter by copying another one.
	
	~AtomicCounter64();
		/// Destroys the AtomicCounter64.

	AtomicCounter64& operator = (const AtomicCounter64& counter);
		/// Assigns the value of another AtomicCounter64.
		
	AtomicCounter64& operator = (ValueType value);
		/// Assigns a value to the counter.

	AtomicCounter64& operator += (const AtomicCounter64& counter);
		/// Add the value of another AtomicCounter64.
		
	AtomicCounter64& operator += (ValueType value);
		/// Add a value to the counter.
        
	operator ValueType () const;
		/// Returns the value of the counter.
		
	ValueType value() const;
		/// Returns the value of the counter.
		
	ValueType operator ++ (); // prefix
		/// Increments the counter and returns the result.
		
	ValueType operator ++ (int); // postfix
		/// Increments the counter and returns the previous value.

	ValueType operator -- (); // prefix
		/// Decrements the counter and returns the result.
		
	ValueType operator -- (int); // postfix
		/// Decrements the counter and returns the previous value.
		
	bool operator ! () const;
		/// Returns true if the counter is zero, false otherwise.

    bool CompBigAndSwap(const AtomicCounter64& counter);
		/// keep the bigger value
        
    bool CompBigAndSwap(const ValueType value);
		/// keep the bigger value

private:
#if POCO_OS == POCO_OS_WINDOWS_NT
	typedef volatile Poco::Int64 ImplType;
#elif POCO_OS == POCO_OS_MAC_OS_X
	typedef Poco::Int64 ImplType;
#elif defined(POCO_HAVE_GCC_ATOMICS)
	typedef Poco::Int64 ImplType;
#else // generic implementation based on FastMutex
	struct ImplType
	{
		mutable Poco::FastMutex mutex;
		volatile Poco::Int64 value;
	};
#endif // POCO_OS

	ImplType _counter;
    mutable Poco::FastMutex _mutex;
};


//
// inlines
//


#if POCO_OS == POCO_OS_WINDOWS_NT
//
// Windows
//
inline AtomicCounter64::operator AtomicCounter64::ValueType () const
{
	return _counter;
}

	
inline AtomicCounter64::ValueType AtomicCounter64::value() const
{
	return _counter;
}


inline AtomicCounter64::ValueType AtomicCounter64::operator ++ () // prefix
{
	return InterlockedIncrement64(&_counter);
}

	
inline AtomicCounter64::ValueType AtomicCounter64::operator ++ (int) // postfix
{
	ValueType result = InterlockedIncrement64(&_counter);
	return --result;
}


inline AtomicCounter64::ValueType AtomicCounter64::operator -- () // prefix
{
	return InterlockedDecrement64(&_counter);
}

	
inline AtomicCounter64::ValueType AtomicCounter64::operator -- (int) // postfix
{
	ValueType result = InterlockedDecrement64(&_counter);
	return ++result;
}

	
inline bool AtomicCounter64::operator ! () const
{
	return _counter == 0;
}


#elif POCO_OS == POCO_OS_MAC_OS_X
//
// Mac OS X
//
inline AtomicCounter64::operator AtomicCounter64::ValueType () const
{
	return _counter;
}

	
inline AtomicCounter64::ValueType AtomicCounter64::value() const
{
	return _counter;
}


inline AtomicCounter64::ValueType AtomicCounter64::operator ++ () // prefix
{
	return OSAtomicIncrement64(&_counter);
}

	
inline AtomicCounter64::ValueType AtomicCounter64::operator ++ (int) // postfix
{
	ValueType result = OSAtomicIncrement64(&_counter);
	return --result;
}


inline AtomicCounter64::ValueType AtomicCounter64::operator -- () // prefix
{
	return OSAtomicDecrement64(&_counter);
}

	
inline AtomicCounter64::ValueType AtomicCounter64::operator -- (int) // postfix
{
	ValueType result = OSAtomicDecrement64(&_counter);
	return ++result;
}

	
inline bool AtomicCounter64::operator ! () const
{
	return _counter == 0;
}

#elif defined(POCO_HAVE_GCC_ATOMICS)
//
// GCC 4.1+ atomic builtins.
//
inline AtomicCounter64::operator AtomicCounter64::ValueType () const
{
	return _counter;
}

	
inline AtomicCounter64::ValueType AtomicCounter64::value() const
{
	return _counter;
}


inline AtomicCounter64::ValueType AtomicCounter64::operator ++ () // prefix
{
	return __sync_add_and_fetch(&_counter, 1);
}

	
inline AtomicCounter64::ValueType AtomicCounter64::operator ++ (int) // postfix
{
	return __sync_fetch_and_add(&_counter, 1);
}


inline AtomicCounter64::ValueType AtomicCounter64::operator -- () // prefix
{
	return __sync_sub_and_fetch(&_counter, 1);
}

	
inline AtomicCounter64::ValueType AtomicCounter64::operator -- (int) // postfix
{
	return __sync_fetch_and_sub(&_counter, 1);
}

	
inline bool AtomicCounter64::operator ! () const
{
	return _counter == 0;
}


#else
//
// Generic implementation based on Poco::FastMutex
//
inline AtomicCounter64::operator AtomicCounter64::ValueType () const
{
	ValueType result;
	{
		Poco::FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value;
	}
	return result;
}

	
inline AtomicCounter64::ValueType AtomicCounter64::value() const
{
	ValueType result;
	{
		Poco::FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value;
	}
	return result;
}


inline AtomicCounter64::ValueType AtomicCounter64::operator ++ () // prefix
{
	ValueType result;
	{
		Poco::FastMutex::ScopedLock lock(_counter.mutex);
		result = ++_counter.value;
	}
	return result;
}

	
inline AtomicCounter64::ValueType AtomicCounter64::operator ++ (int) // postfix
{
	ValueType result;
	{
		Poco::FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value++;
	}
	return result;
}


inline AtomicCounter64::ValueType AtomicCounter64::operator -- () // prefix
{
	ValueType result;
	{
		Poco::FastMutex::ScopedLock lock(_counter.mutex);
		result = --_counter.value;
	}
	return result;
}

	
inline AtomicCounter64::ValueType AtomicCounter64::operator -- (int) // postfix
{
	ValueType result;
	{
		Poco::FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value--;
	}
	return result;
}

	
inline bool AtomicCounter64::operator ! () const
{
	bool result;
	{
		Poco::FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value == 0;
	}
	return result;
}


#endif // POCO_OS

} // namespace Cumulus 


#endif // Foundation_AtomicCounter64_INCLUDED
