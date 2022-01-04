#pragma once
#include <stddef.h>
#include <iterator>
#include "Engine/Utility/Hash.h"

namespace rv
{
	class MultiMap
	{
	public:
		MultiMap() = default;
		MultiMap(const MultiMap& rhs);
		MultiMap(MultiMap&& rhs) noexcept;
		~MultiMap();


	private:
		typedef void(*Destructor)(void*);

		template<typename T>
		static void make_destructor(void* object)
		{
			if (object)
				reinterpret_cast<T*>(object)->~T();
		}

		struct SequentialHeader
		{
			SequentialHeader() = default;
			~SequentialHeader();

			size_t type = 0;
			Destructor destructor = nullptr;
			SequentialHeader* next = nullptr;
		};

		struct Header
		{
			Header() = default;
			~Header();

			size_t key = 0;
			size_t type = 0;
			Destructor destructor = nullptr;
			Header* left = nullptr;
			Header* right = nullptr;
			SequentialHeader* next = nullptr;
		};
		template<typename T>
		struct Entry
		{
			Entry(size_t key)
				:
				value()
			{
				header.key = key;
				header.type = typeid(T).hash_code();
				if constexpr (std::is_class_v<T> || std::is_union_v<T>)
				header.destructor = make_destructor<T>;
			}
			Entry(size_t key, const T& value)
				:
				value(value)
			{
				header.key = key;
				header.type = typeid(T).hash_code();
				if constexpr (std::is_class_v<T> || std::is_union_v<T>)
				header.destructor = make_destructor<T>;
			}
			Entry(size_t key, T&& value)
				:
				value(std::move(value))
			{
				header.key = key;
				header.type = typeid(T).hash_code();
				if constexpr (std::is_class_v<T> || std::is_union_v<T>)
				header.destructor = make_destructor<T>;
			}

			Header header;
			T value;
		};
		template<typename T>
		struct SequentialEntry
		{
			SequentialHeader header;
			T value;
		};

		template<typename T>
		static Entry<T>* as_entry(Header* header) { return reinterpret_cast<Entry<T>*>(header); }
		template<typename T>
		static const Entry<T>* as_entry(const Header* header) { return reinterpret_cast<Entry<T>*>(header); }

		template<typename T>
		static SequentialEntry<T>* as_entry(SequentialHeader* header) { return reinterpret_cast<SequentialEntry<T>*>(header); }
		template<typename T>
		static const SequentialEntry<T>* as_entry(const SequentialHeader* header) { return reinterpret_cast<SequentialEntry<T>*>(header); }

		template<typename T>
		static T* data(Header* header)
		{
			return reinterpret_cast<T*>(reinterpret_cast<unsigned char*>(header) + offsetof(Entry<T>, value));
		}
		template<typename T>
		static const T* data(const Header* header)
		{
			return reinterpret_cast<const T*>(reinterpret_cast<const unsigned char*>(header) + offsetof(Entry<T>, value));
		}
		template<>
		static void* data(Header* header)
		{
			return reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(header) + sizeof(Header));
		}
		template<>
		static const void* data(const Header* header)
		{
			return reinterpret_cast<const void*>(reinterpret_cast<const unsigned char*>(header) + sizeof(Header));
		}

		template<typename T = void>
		static T* data(SequentialHeader* header)
		{
			return reinterpret_cast<T*>(reinterpret_cast<unsigned char*>(header) + offsetof(SequentialEntry<T>, value));
		}
		template<typename T = void>
		static const T* data(const SequentialHeader* header)
		{
			return reinterpret_cast<const T*>(reinterpret_cast<const unsigned char*>(header) + offsetof(SequentialEntry<T>, value));
		}
		template<>
		static void* data(SequentialHeader* header)
		{
			return reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(header) + sizeof(SequentialHeader));
		}
		template<>
		static const void* data(const SequentialHeader* header)
		{
			return reinterpret_cast<const void*>(reinterpret_cast<const unsigned char*>(header) + sizeof(SequentialHeader));
		}

		Header* get_header(size_t key);
		const Header* get_header(size_t key) const;
		Header* get_header_const(size_t key) const;

		static void destroy(Header* header);
		static void destroy(SequentialHeader* header);

		Header* first = nullptr;


	public:
		template<typename T, typename... K>
		T& get(const K&... keys)
		{
			size_t key = hash(typeid(T).hash_code(), keys...);
			Header* header = get_header(key);

			if (header)
			{
				if (header->type == typeid(T).hash_code())
					return *data<T>(header);
				for (SequentialHeader* sequential = header->next; sequential; sequential = sequential->next)
					if (sequential->type == typeid(T).hash_code())
						return *data<T>(sequential);
			}

			if (first)
			{
				header = first;
				while (true)
				{
					if (key < header->key)
					{
						if (header->left)
						{
							header = header->left;
						}
						else
						{
							header->left = reinterpret_cast<Header*>(new Entry<T>(key));
							return *data<T>(header->left);
						}
					}
					else
					{
						if (header->right)
						{
							header = header->right;
						}
						else
						{
							header->right = reinterpret_cast<Header*>(new Entry<T>(key));
							return *data<T>(header->right);
						}
					}
				}
			}
			else
			{
				first = reinterpret_cast<Header*>(new Entry<T>(key));
				return *data<T>(first);
			}
		}
	};
}