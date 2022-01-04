#pragma once
#include <utility>
#include "Engine/Utility/Hash.h"

namespace rv
{
	template<typename T, size_t I>
	struct PrimitiveType
	{
	public:
		PrimitiveType() = default;
		PrimitiveType(const PrimitiveType&) = default;
		PrimitiveType(PrimitiveType&&) = default;
		PrimitiveType(const T& value) : value(value) {}
		PrimitiveType(T&& value) : value(std::move(value)) {}

		PrimitiveType& operator= (const PrimitiveType& rhs) { value = rhs.value; return *this; }
		PrimitiveType& operator= (PrimitiveType&& rhs) { value = std::move(rhs.value); return *this; }

		operator T& () { return value; }
		operator const T& () const { return value; }

		T& get() { return value; }
		const T& get() const { return value; }

	private:
		T value;
	};
}

#define rv_def_primitive(className, type) typedef rv::PrimitiveType<type, rv::hash(#className)> className