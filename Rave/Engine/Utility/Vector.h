#pragma once
#include <array>
#include <type_traits>
#include <limits>

namespace rv
{
	template<size_t S, typename T, typename I>
	struct BasicVector
	{
		static constexpr size_t size() { return S; }
	};

	template<size_t S, typename T, typename I>
	concept BasicVectorConcept = requires(const BasicVector<S, T, I>& vector)
	{
		BasicVector<S, T, I>::size();
		vector[0];
		vector.data();
		vector.begin();
		vector.cbegin();
		vector.end();
		vector.cend();
		vector.rbegin();
		vector.crbegin();
		vector.rend();
		vector.crend();
	};

	namespace detail
	{
		struct VectorIdentifier {};
		struct ExtentIdentifier {};

		template <typename T, std::size_t ... Is>
		constexpr std::array<T, sizeof...(Is)>
		create_array(T value, std::index_sequence<Is...>)
		{
			// cast Is to void to remove the warning: unused value
			return { {(static_cast<void>(Is), value)...} };
		}

		template<size_t A, size_t B>
		concept args_comp = A == B;

		template<typename T>
		requires std::is_floating_point_v<T>
		T constexpr sqrtNewtonRaphson(T x, T curr, T prev)
		{
			return curr == prev
				? curr
				: sqrtNewtonRaphson(x, (T)0.5 * (curr + x / curr), curr);
		}

		template<typename T>
		requires std::is_floating_point_v<T>
		T constexpr sqrt(T x)
		{
			return x >= 0 && x < std::numeric_limits<T>::infinity()
				? sqrtNewtonRaphson<T>(x, x, 0)
				: std::numeric_limits<T>::quiet_NaN();
		}
	}

	template<size_t S, typename T>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct BasicVector<S, T, detail::VectorIdentifier>
	{

		constexpr BasicVector(const T& value = 0) : array(detail::create_array(value, std::make_index_sequence<S>())) {}
		template<typename... Args>
		requires detail::args_comp<S, sizeof...(Args) + 2>
		constexpr BasicVector(const T& first, const T& second, const Args&... args) : array({ first, second, static_cast<T>(args)... }) {}

		template<size_t I> constexpr		T& get_element()		{ return array[I]; }
		template<size_t I> constexpr const	T& get_element() const	{ return array[I]; }

		static constexpr size_t size() { return S; }

		constexpr T LengthSQ()			const { return lengthSQ(*this); }
				  T Length()			const { return length(*this); }
		constexpr T LengthConstexpr()	const { return length_constexpr(*this); }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* data()		{ return array.data(); }
		const	T* data() const	{ return array.data(); }

				T* begin()				{ return array.begin(); }
		const	T* begin()		const	{ return array.begin(); }
		const	T* cbegin()		const	{ return array.cbegin(); }
				T* rbegin()				{ return array.rbegin(); }
		const	T* rbegin()		const	{ return array.rbegin(); }
		const	T* crbegin()	const	{ return array.crbegin(); }

				T* end()				{ return array.end(); }
		const	T* end()		const	{ return array.end(); }
		const	T* cend()		const	{ return array.cend(); }
				T* rend()				{ return array.rend(); }
		const	T* rend()		const	{ return array.rend(); }
		const	T* crend()		const	{ return array.crend(); }

		std::array<T, S> array;
	};

	template<typename T>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct BasicVector<2, T, detail::VectorIdentifier>
	{
		constexpr BasicVector(const T& value = 0) : x(value), y(value) {}
		constexpr BasicVector(const T& x, const T& y) : x(x), y(y) {}

		template<size_t I> constexpr		T& get_element();
		template<size_t I> constexpr const	T& get_element() const;

		template<> constexpr		T& get_element<0>()			{ return x; }
		template<> constexpr const	T& get_element<0>() const	{ return x; }
		template<> constexpr		T& get_element<1>()			{ return y; }
		template<> constexpr const	T& get_element<1>() const	{ return y; }

		static constexpr size_t size() { return 2; }

		constexpr T LengthSQ()			const { return lengthSQ(*this); }
				  T Length()			const { return length(*this); }
		constexpr T LengthConstexpr()	const { return length_constexpr(*this); }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* data()		{ return &x; }
		const	T* data() const	{ return &x; }

				T* begin()				{ return data(); }
		const	T* begin()		const	{ return data(); }
		const	T* cbegin()		const	{ return data(); }
				T* rbegin()				{ return &y; }
		const	T* rbegin()		const	{ return &y; }
		const	T* crbegin()	const	{ return &y; }

				T* end()				{ return &y + 1; }
		const	T* end()		const	{ return &y + 1; }
		const	T* cend()		const	{ return &y + 1; }
				T* rend()				{ return &x - 1; }
		const	T* rend()		const	{ return &x - 1; }
		const	T* crend()		const	{ return &x - 1; }

		T x;
		T y;
	};

	template<typename T>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct BasicVector<3, T, detail::VectorIdentifier>
	{
		constexpr BasicVector(const T& value = 0) : x(value), y(value), z(value) {}
		constexpr BasicVector(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}

		template<size_t I> constexpr		T& get_element();
		template<size_t I> constexpr const	T& get_element() const;

		template<> constexpr		T& get_element<0>()			{ return x; }
		template<> constexpr const	T& get_element<0>() const	{ return x; }
		template<> constexpr		T& get_element<1>()			{ return y; }
		template<> constexpr const	T& get_element<1>() const	{ return y; }
		template<> constexpr		T& get_element<2>()			{ return z; }
		template<> constexpr const	T& get_element<2>() const	{ return z; }

		static constexpr size_t size() { return 3; }

		constexpr T LengthSQ()			const { return lengthSQ(*this); }
				  T Length()			const { return length(*this); }
		constexpr T LengthConstexpr()	const { return length_constexpr(*this); }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* data()		{ return &x; }
		const	T* data() const	{ return &x; }

				T* begin()				{ return data(); }
		const	T* begin()		const	{ return data(); }
		const	T* cbegin()		const	{ return data(); }
				T* rbegin()				{ return &z; }
		const	T* rbegin()		const	{ return &z; }
		const	T* crbegin()	const	{ return &z; }

				T* end()				{ return &z + 1; }
		const	T* end()		const	{ return &z + 1; }
		const	T* cend()		const	{ return &z + 1; }
				T* rend()				{ return &x - 1; }
		const	T* rend()		const	{ return &x - 1; }
		const	T* crend()		const	{ return &x - 1; }

		T x;
		T y;
		T z;
	};

	template<typename T>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct BasicVector<4, T, detail::VectorIdentifier>
	{
		constexpr BasicVector(const T& value = 0) : x(value), y(value), z(value), w(value) {}
		constexpr BasicVector(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w) {}

		template<size_t I> constexpr		T& get_element();
		template<size_t I> constexpr const	T& get_element() const;

		template<> constexpr		T& get_element<0>()			{ return x; }
		template<> constexpr const	T& get_element<0>() const	{ return x; }
		template<> constexpr		T& get_element<1>()			{ return y; }
		template<> constexpr const	T& get_element<1>() const	{ return y; }
		template<> constexpr		T& get_element<2>()			{ return z; }
		template<> constexpr const	T& get_element<2>() const	{ return z; }
		template<> constexpr		T& get_element<3>()			{ return w; }
		template<> constexpr const	T& get_element<3>() const	{ return w; }

		static constexpr size_t size() { return 4; }

		constexpr T LengthSQ()			const { return lengthSQ(*this); }
				  T Length()			const { return length(*this); }
		constexpr T LengthConstexpr()	const { return length_constexpr(*this); }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* data()		{ return &x; }
		const	T* data() const	{ return &x; }

				T* begin()				{ return data(); }
		const	T* begin()		const	{ return data(); }
		const	T* cbegin()		const	{ return data(); }
				T* rbegin()				{ return &w; }
		const	T* rbegin()		const	{ return &w; }
		const	T* crbegin()	const	{ return &w; }

				T* end()				{ return &w + 1; }
		const	T* end()		const	{ return &w + 1; }
		const	T* cend()		const	{ return &w + 1; }
				T* rend()				{ return &x - 1; }
		const	T* rend()		const	{ return &x - 1; }
		const	T* crend()		const	{ return &x - 1; }

		T x;
		T y;
		T z;
		T w;
	};

	template<typename T>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct BasicVector<2, T, detail::ExtentIdentifier>
	{
		constexpr BasicVector(const T& value = 0) : width(value), height(value) {}
		constexpr BasicVector(const T& width, const T& height) : width(width), height(height) {}

		template<size_t I> constexpr		T& get_element();
		template<size_t I> constexpr const	T& get_element() const;

		template<> constexpr		T& get_element<0>()			{ return width; }
		template<> constexpr const	T& get_element<0>() const	{ return width; }
		template<> constexpr		T& get_element<1>()			{ return height; }
		template<> constexpr const	T& get_element<1>() const	{ return height; }

		static constexpr size_t size() { return 2; }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* data()		{ return &width; }
		const	T* data() const	{ return &width; }

				T* begin()				{ return data(); }
		const	T* begin()		const	{ return data(); }
		const	T* cbegin()		const	{ return data(); }
				T* rbegin()				{ return &height; }
		const	T* rbegin()		const	{ return &height; }
		const	T* crbegin()	const	{ return &height; }

				T* end()				{ return &height + 1; }
		const	T* end()		const	{ return &height + 1; }
		const	T* cend()		const	{ return &height + 1; }
				T* rend()				{ return &width - 1; }
		const	T* rend()		const	{ return &width - 1; }
		const	T* crend()		const	{ return &width - 1; }

		T width;
		T height;
	};

	template<typename T>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct BasicVector<3, T, detail::ExtentIdentifier>
	{
		constexpr BasicVector(const T& value = 0) : width(value), height(value) {}
		constexpr BasicVector(const T& width, const T& height, const T& depth) : width(width), height(height), depth(depth) {}

		template<size_t I> constexpr		T& get_element();
		template<size_t I> constexpr const	T& get_element() const;

		template<> constexpr		T& get_element<0>()			{ return width; }
		template<> constexpr const	T& get_element<0>() const	{ return width; }
		template<> constexpr		T& get_element<1>()			{ return height; }
		template<> constexpr const	T& get_element<1>() const	{ return height; }
		template<> constexpr		T& get_element<2>()			{ return depth; }
		template<> constexpr const	T& get_element<2>() const	{ return depth; }

		static constexpr size_t size() { return 3; }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* data()		{ return &width; }
		const	T* data() const	{ return &width; }

				T* begin()				{ return data(); }
		const	T* begin()		const	{ return data(); }
		const	T* cbegin()		const	{ return data(); }
				T* rbegin()				{ return &depth; }
		const	T* rbegin()		const	{ return &depth; }
		const	T* crbegin()	const	{ return &depth; }

				T* end()				{ return &depth + 1; }
		const	T* end()		const	{ return &depth + 1; }
		const	T* cend()		const	{ return &depth + 1; }
				T* rend()				{ return &width - 1; }
		const	T* rend()		const	{ return &width - 1; }
		const	T* crend()		const	{ return &width - 1; }

		T width;
		T height;
		T depth;
	};


	template<size_t S, typename T, typename I, size_t D = 0>
	requires BasicVectorConcept<S, T, I>
	static constexpr T lengthSQ(const BasicVector<S, T, I>& vector)
	{
		if constexpr (D < S)
			return vector.get_element<D>() * vector.get_element<D>() + lengthSQ<S, T, I, D + 1>(vector);
		else
			return 0;
	}

	template<size_t S, typename T, typename I>
	requires BasicVectorConcept<S, T, I>
	static T length(const BasicVector<S, T, I>& vector)
	{
		return (T)sqrt((double)lengthSQ(vector));
	}

	template<size_t S, typename T, typename I>
	requires BasicVectorConcept<S, T, I>
	static constexpr T length_constexpr(const BasicVector<S, T, I>& vector)
	{
		return detail::sqrt<T>(lengthSQ(vector));
	}

	template<typename T1, typename I1, typename T2, typename I2>
	static constexpr T1 convert_element(const T2& x)
	{
		return static_cast<T1>(x);
	}

	template<size_t S, typename T1, typename I1, typename T2, typename I2, size_t D = 0>
	requires BasicVectorConcept<S, T1, I1> && BasicVectorConcept<S, T2, I2>
	static constexpr T1 dot(const BasicVector<S, T1, I1>& a, const BasicVector<S, T2, I2>& b)
	{
		if constexpr (D < S)
			return a.get_element<D>() * convert_element<T1, I1, T2, I2>(b.get_element<D>()) + dot<S, T1, I1, T2, I2, D + 1>(a, b);
		else
			return 0;
	}

	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2, size_t D = 0>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr bool operator== (const BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		if constexpr (S1 != S2)
			return false;
		else
			if constexpr (D < S1)
				return a.get_element<D>() == convert_element<T1, I1, T2, I2>(b.get_element<D>()) && operator==<S1, T1, I1, S2, T2, I2, D + 1>(a, b);
			else
				return true;
	}

	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2, size_t D = 0>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr bool operator!= (const BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		if constexpr (S1 != S2)
			return true;
		else
			if constexpr (D < S1)
				return a.get_element<D>() != convert_element<T1, I1, T2, I2>(b.get_element<D>()) || operator!=<S1, T1, I1, S2, T2, I2, D + 1>(a, b);
			else
				return false;
	}

	namespace detail
	{
		enum Operation
		{
			RV_OP_ADD,
			RV_OP_SUB,
			RV_OP_MUL,
			RV_OP_DIV,
		};

		template<Operation O, size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2, size_t D = 0>
		requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
		static constexpr BasicVector<S1, T1, I1>& operate_self(BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
		{
			if constexpr (D < std::min(S1, S2))
			{
				if constexpr (O == RV_OP_ADD)
					a.get_element<D>() += convert_element<T1, I1, T2, I2>(b.get_element<D>());
				if constexpr (O == RV_OP_SUB)
					a.get_element<D>() -= convert_element<T1, I1, T2, I2>(b.get_element<D>());
				if constexpr (O == RV_OP_MUL)
					a.get_element<D>() *= convert_element<T1, I1, T2, I2>(b.get_element<D>());
				if constexpr (O == RV_OP_DIV)
					a.get_element<D>() /= convert_element<T1, I1, T2, I2>(b.get_element<D>());
				operate_self<O, S1, T1, I1, S2, T2, I2, D + 1>(a, b);
			}
			return a;
		}

		template<Operation O, size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2, size_t D = 0>
		requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
		static constexpr void operate(BasicVector<S1, T1, I1>& a, const BasicVector<S1, T1, I1>& b, const BasicVector<S2, T2, I2>& c)
		{
			if constexpr (D < std::min(S1, S2))
			{
				if constexpr (O == RV_OP_ADD)
					a.get_element<D>() = b.get_element<D>() + convert_element<T1, I1, T2, I2>(c.get_element<D>());
				if constexpr (O == RV_OP_SUB)
					a.get_element<D>() = b.get_element<D>() - convert_element<T1, I1, T2, I2>(c.get_element<D>());
				if constexpr (O == RV_OP_MUL)
					a.get_element<D>() = b.get_element<D>() * convert_element<T1, I1, T2, I2>(c.get_element<D>());
				if constexpr (O == RV_OP_DIV)
					a.get_element<D>() = b.get_element<D>() / convert_element<T1, I1, T2, I2>(c.get_element<D>());
				operate<O, S1, T1, I1, S2, T2, I2, D + 1>(a, b, c);
			}
		}
	}

	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr BasicVector<S1, T1, I1>& operator+= (BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		return detail::operate_self<detail::RV_OP_ADD>(a, b);
	}
	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr BasicVector<S1, T1, I1>& operator-= (BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		return detail::operate_self<detail::RV_OP_SUB>(a, b);
	}
	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr BasicVector<S1, T1, I1>& operator*= (BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		return detail::operate_self<detail::RV_OP_MUL>(a, b);
	}
	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr BasicVector<S1, T1, I1>& operator/= (BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		return detail::operate_self<detail::RV_OP_DIV>(a, b);
	}

	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr BasicVector<S1, T1, I1> operator+ (const BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		BasicVector<S1, T1, I1> ret;
		detail::operate<detail::RV_OP_ADD>(ret, a, b);
		return ret;
	}
	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr BasicVector<S1, T1, I1> operator- (const BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		BasicVector<S1, T1, I1> ret;
		detail::operate<detail::RV_OP_SUB>(ret, a, b);
		return ret;
	}
	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr BasicVector<S1, T1, I1> operator* (const BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		BasicVector<S1, T1, I1> ret;
		detail::operate<detail::RV_OP_MUL>(ret, a, b);
		return ret;
	}
	template<size_t S1, typename T1, typename I1, size_t S2, typename T2, typename I2>
	requires BasicVectorConcept<S1, T1, I1> && BasicVectorConcept<S2, T2, I2>
	static constexpr BasicVector<S1, T1, I1> operator/ (const BasicVector<S1, T1, I1>& a, const BasicVector<S2, T2, I2>& b)
	{
		BasicVector<S1, T1, I1> ret;
		detail::operate<detail::RV_OP_DIV>(ret, a, b);
		return ret;
	}

	template<size_t S, typename T>
	using Vector = BasicVector<S, T, detail::VectorIdentifier>;
	typedef Vector<2, float>	Vector2;
	typedef Vector<3, float>	Vector3;
	typedef Vector<4, float>	Vector4;
	typedef Vector<2, int>		Point;

	template<size_t S, typename T>
	using Extent = BasicVector<S, T, detail::ExtentIdentifier>;
	typedef Extent<2, float>	Extent2D;
	typedef Extent<3, float>	Extent3D;
}