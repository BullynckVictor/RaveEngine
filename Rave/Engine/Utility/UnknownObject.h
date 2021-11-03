#pragma once
#include <memory>

namespace rv
{
	class UnknownObject
	{
	private:
		template<typename T>
		requires std::is_array_v<T> || std::is_pointer_v<T>
		struct Destructor
		{
			void operator()(void* p) const
			{
				typename std::decay<T>::type object = reinterpret_cast<std::decay<T>::type>(p);

				if constexpr (std::is_pointer_v<T>)
					delete object;
				else
					delete[] object;
			}
		};

	public:
		template<typename T>
		UnknownObject(T object) requires std::is_array_v<T> || std::is_pointer_v<T>
			:
			object(object, Destructor<T>())
		{}

		template<typename T>
		static UnknownObject make(T object) requires std::is_array_v<T> || std::is_pointer_v<T>
		{
			return std::shared_ptr<void>(object, Destructor<T>());
		}

		void* data() { return object.get(); }
		const void* data() const { return object.get(); }
		template<typename T> T& get() { return *reinterpret_cast<T*>(data()); }
		template<typename T> const T& get() const { return *reinterpret_cast<const T*>(data()); }

	private:
		UnknownObject(const std::shared_ptr<void>& object) : object(object){}
		UnknownObject(std::shared_ptr<void>&& object) : object(std::move(object)){}

		std::shared_ptr<void> object;
	};

	template<typename T>
	requires std::is_array_v<T> || std::is_pointer_v<T>
	UnknownObject make_unknown(T object)
	{
		return UnknownObject::make<T>(object);
	}
}