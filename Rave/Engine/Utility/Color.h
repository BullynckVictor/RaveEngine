#pragma once
#include "Engine/Utility/Vector.h"
#include "Engine/Utility/Types.h"

namespace rv
{
	namespace detail
	{
		struct ColorIdentifier {};
		template<typename T> struct ColorInfo {};
		template<> struct ColorInfo<u8> { static constexpr u8 max = 255; };
		template<> struct ColorInfo<float> { static constexpr float max = 1.0f; };


	}

	template<typename T>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct BasicVector<4, T, detail::ColorIdentifier>
	{
		constexpr BasicVector(const T& value = 0, const T& alpha = detail::ColorInfo<T>::max) : r(value), g(value), b(value), a(alpha) {}
		constexpr BasicVector(const T& r, const T& g, const T& b, const T& a = detail::ColorInfo<T>::max) : r(r), g(g), b(b), a(a) {}

		template<size_t I> constexpr		T& get_element();
		template<size_t I> constexpr const	T& get_element() const;

		template<> constexpr		T& get_element<0>()			{ return r; }
		template<> constexpr const	T& get_element<0>() const	{ return r; }
		template<> constexpr		T& get_element<1>()			{ return g; }
		template<> constexpr const	T& get_element<1>() const	{ return g; }
		template<> constexpr		T& get_element<2>()			{ return b; }
		template<> constexpr const	T& get_element<2>() const	{ return b; }
		template<> constexpr		T& get_element<3>()			{ return a; }
		template<> constexpr const	T& get_element<3>() const	{ return a; }

		static constexpr size_t size() { return 4; }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* data()		{ return &r; }
		const	T* data() const	{ return &r; }

				T* begin()				{ return data(); }
		const	T* begin()		const	{ return data(); }
		const	T* cbegin()		const	{ return data(); }
				T* rbegin()				{ return &a; }
		const	T* rbegin()		const	{ return &a; }
		const	T* crbegin()	const	{ return &a; }

				T* end()				{ return &a + 1; }
		const	T* end()		const	{ return &a + 1; }
		const	T* cend()		const	{ return &a + 1; }
				T* rend()				{ return &r - 1; }
		const	T* rend()		const	{ return &r - 1; }
		const	T* crend()		const	{ return &r - 1; }

		void Clamp()
		{
			r = std::min(r, detail::ColorInfo<T>::max);
			g = std::min(g, detail::ColorInfo<T>::max);
			b = std::min(b, detail::ColorInfo<T>::max);
			a = std::min(a, detail::ColorInfo<T>::max);
		}

		T r;
		T g;
		T b;
		T a;
	};

	template<typename T>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct BasicVector<3, T, detail::ColorIdentifier>
	{
		constexpr BasicVector(const T& value = 0) : r(value), g(value), b(value) {}
		constexpr BasicVector(const T& r, const T& g, const T& b) : r(r), g(g), b(b) {}

		template<size_t I> constexpr		T& get_element();
		template<size_t I> constexpr const	T& get_element() const;

		template<> constexpr		T& get_element<0>()			{ return r; }
		template<> constexpr const	T& get_element<0>() const	{ return r; }
		template<> constexpr		T& get_element<1>()			{ return g; }
		template<> constexpr const	T& get_element<1>() const	{ return g; }
		template<> constexpr		T& get_element<2>()			{ return b; }
		template<> constexpr const	T& get_element<2>() const	{ return b; }

		static constexpr size_t size() { return 3; }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* data()		{ return &r; }
		const	T* data() const	{ return &r; }

				T* begin()				{ return data(); }
		const	T* begin()		const	{ return data(); }
		const	T* cbegin()		const	{ return data(); }
				T* rbegin()				{ return &b; }
		const	T* rbegin()		const	{ return &b; }
		const	T* crbegin()	const	{ return &b; }

				T* end()				{ return &b + 1; }
		const	T* end()		const	{ return &b + 1; }
		const	T* cend()		const	{ return &b + 1; }
				T* rend()				{ return &r - 1; }
		const	T* rend()		const	{ return &r - 1; }
		const	T* crend()		const	{ return &r - 1; }

		void Clamp()
		{
			r = std::min(r, detail::ColorInfo<T>::max);
			g = std::min(g, detail::ColorInfo<T>::max);
			b = std::min(b, detail::ColorInfo<T>::max);
		}

		T r;
		T g;
		T b;
	};

	template<>
	static constexpr u8 convert_element<u8, detail::ColorIdentifier, float, detail::ColorIdentifier>(const float& c)
	{
		return static_cast<u8>(c / detail::ColorInfo<float>::max * static_cast<float>(detail::ColorInfo<u8>::max));
	}

	template<>
	static constexpr float convert_element<float, detail::ColorIdentifier, u8, detail::ColorIdentifier>(const u8& c)
	{
		return static_cast<float>(c) * detail::ColorInfo<float>::max / static_cast<float>(detail::ColorInfo<u8>::max);
	}

	template<size_t S>
	static constexpr BasicVector<S, u8, detail::ColorIdentifier> convert(const BasicVector<S, float, detail::ColorIdentifier>& color)
	{
		return convert<S, u8, detail::ColorIdentifier>(color);
	}
	template<size_t S>
	static constexpr BasicVector<S, float, detail::ColorIdentifier> convert(const BasicVector<S, u8, detail::ColorIdentifier>& color)
	{
		return convert<S, float, detail::ColorIdentifier>(color);
	}

	typedef BasicVector<3, u8,		detail::ColorIdentifier>	ColorRGB;
	typedef BasicVector<3, float,	detail::ColorIdentifier>	FColorRGB;
	typedef BasicVector<4, u8,		detail::ColorIdentifier>	ColorRGBA;
	typedef BasicVector<4, float,	detail::ColorIdentifier>	FColorRGBA;
	typedef ColorRGBA	Color;
	typedef FColorRGBA	FColor;

	namespace Colors
	{
		static constexpr Color MakeRGBHex(u32 rgb) { return Color((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 0) & 0xFF); }
		static constexpr Color MakeRGB(u8 r, u8 g, u8 b) { return Color(r, g, b); }
		static constexpr Color MakeRGB(u32 rgb) { return MakeRGBHex(rgb); }
		static constexpr Color MakeRGBAHex(u32 rgba) { return Color((rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF, (rgba >> 0) & 0xFF); }
		static constexpr Color MakeRGBA(u8 r, u8 g, u8 b, u8 a) { return Color(r, g, b, a); }
		static constexpr Color MakeRGBA(u32 rgba) { return MakeRGBAHex(rgba); }

		static constexpr Color Transparent	= MakeRGBA(   0,    0,    0,    0);
		static constexpr Color Black		= MakeRGB (   0,    0,    0 );
		static constexpr Color Silver		= MakeRGB ( 192,  192,  192 );
		static constexpr Color Gray			= MakeRGB ( 128,  128,  128 );
		static constexpr Color White		= MakeRGB ( 255,  255,  255 );
		static constexpr Color Maroon		= MakeRGB ( 128,    0,    0 );
		static constexpr Color Red			= MakeRGB ( 255,    0,    0 );
		static constexpr Color Purple		= MakeRGB ( 128,    0,  128 );
		static constexpr Color Fuchsia		= MakeRGB ( 255,    0,  255 );
		static constexpr Color Green		= MakeRGB (   0,  128,    0 );
		static constexpr Color Lime			= MakeRGB (   0,  255,    0 );
		static constexpr Color Olive		= MakeRGB ( 128,  128,    0 );
		static constexpr Color Yellow		= MakeRGB ( 255,  255,    0 );
		static constexpr Color Navy			= MakeRGB (   0,    0,  128 );
		static constexpr Color Blue			= MakeRGB (   0,    0,  255 );
		static constexpr Color Teal			= MakeRGB (   0,  128,  128 );
		static constexpr Color Aqua			= MakeRGB (   0,  255,  255 );
	}

	namespace FColors
	{
		static constexpr FColor MakeRGBHex(u32 rgb) { return convert(Colors::MakeRGBHex(rgb)); }
		static constexpr FColor MakeRGB(u8 r, u8 g, u8 b) { return convert(Colors::MakeRGB(r, g, b)); }
		static constexpr FColor MakeRGB(u32 rgb) { return convert(Colors::MakeRGBHex(rgb)); }
		static constexpr FColor MakeRGBAHex(u32 rgba) { return convert(Colors::MakeRGBAHex(rgba)); }
		static constexpr FColor MakeRGBA(u8 r, u8 g, u8 b, u8 a) { return convert(Colors::MakeRGBA(r, g, b, a)); }
		static constexpr FColor MakeRGBA(u32 rgba) { return convert(Colors::MakeRGBAHex(rgba)); }

		static constexpr FColor Transparent		= FColor(  0.0f,   0.0f,   0.0f,   0.0f );
		static constexpr FColor Black			= FColor(  0.0f,   0.0f,   0.0f );
		static constexpr FColor Silver			= FColor( 0.75f,  0.75f,  0.75f );
		static constexpr FColor Gray			= FColor(  0.5f,   0.5f,   0.5f );
		static constexpr FColor White			= FColor(  1.0f,   1.0f,   1.0f );
		static constexpr FColor Maroon			= FColor(  0.5f,   0.0f,   0.0f );
		static constexpr FColor Red				= FColor(  1.0f,   0.0f,   0.0f );
		static constexpr FColor Purple			= FColor(  0.5f,   0.0f,   0.5f );
		static constexpr FColor Fuchsia			= FColor(  1.0f,   0.0f,   1.0f );
		static constexpr FColor Green			= FColor(  0.0f,   0.5f,   0.0f );
		static constexpr FColor Lime			= FColor(  0.0f,   1.0f,   0.0f );
		static constexpr FColor Olive			= FColor(  0.5f,   0.5f,   0.0f );
		static constexpr FColor Yellow			= FColor(  1.0f,   1.0f,   0.0f );
		static constexpr FColor Navy			= FColor(  0.0f,   0.0f,   0.5f );
		static constexpr FColor Blue			= FColor(  0.0f,   0.0f,   1.0f );
		static constexpr FColor Teal			= FColor(  0.0f,   0.5f,   0.5f );
		static constexpr FColor Aqua			= FColor(  0.0f,   1.0f,   1.0f );
	}
}