#pragma once

namespace rv
{
	template<typename T>
	class HeapBuffer
	{
	public:
		HeapBuffer() : m_data(nullptr), m_size(0) {}
		HeapBuffer(size_t size) : m_data(new T[size]), m_size(size) {}
		HeapBuffer(const T* begin, const T* end) : m_data((end - begin > 0) ? new T[end - begin] : nullptr), m_size(end - begin) {}
		HeapBuffer(const HeapBuffer& rhs) : m_data((rhs.size() > 0) ? new T[rhs.size()] : nullptr), m_size(rhs.size()) {}
		HeapBuffer(HeapBuffer&& rhs) noexcept : m_data(rhs.data()), m_size(rhs.size) { rhs.m_data = nullptr, rhs.m_size = 0; }
		~HeapBuffer() { clear(); }

		size_t size() const { return m_size; }

				T* data()		{ return m_data; }
		const	T* data() const	{ return m_data; }

				T& operator[] (size_t index)		{ return data()[index]; }
		const	T& operator[] (size_t index) const	{ return data()[index]; }

				T* begin()				{ return data(); }
		const	T* begin()		const	{ return data(); }
		const	T* cbegin()		const	{ return data(); }
				T* rbegin()				{ return data() + size() - 1; }
		const	T* rbegin()		const	{ return data() + size() - 1; }
		const	T* crbegin()	const	{ return data() + size() - 1; }

				T* end()				{ return data() + size(); }
		const	T* end()		const	{ return data() + size(); }
		const	T* cend()		const	{ return data() + size(); }
				T* rend()				{ return data() - 1; }
		const	T* rend()		const	{ return data() - 1; }
		const	T* crend()		const	{ return data() - 1; }

		void clear() { if (m_data) delete[] m_data; }
		bool empty() const { return m_size; }

	private:
		T* m_data;
		size_t m_size;
	};
}