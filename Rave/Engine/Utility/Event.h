#pragma once
#include "Engine/Utility/Identifier.h"
#include <deque>
#include <mutex>
#include <memory>

namespace rv
{
	struct EventData
	{
		EventData() = default;
		EventData(const Identifier& id);
		virtual ~EventData() = default;

		Identifier id;
	};

	template<typename E>
	concept EventConcept = requires { E::type_id; } && std::is_base_of_v<EventData, E>;


	class EventQueue
	{
	public:
		EventQueue();
		~EventQueue();

	protected:
		void PostEvent(EventData* event);
		template<EventConcept E>
		void PostEvent(const E& event)
		{
			PostEvent(new E(event));
		}
		template<EventConcept E>
		void PostEvent(E&& event)
		{
			PostEvent(new E(std::move(event)));
		}

		template<EventConcept E, typename... Args>
		void EmplaceEvent(const Args&... args)
		{
			PostEvent(new E(args...));
		}

	private:
		struct ListenerQueue
		{
			ListenerQueue() = default;
			std::deque<std::unique_ptr<EventData>> events;
		};
		std::deque<ListenerQueue> queues;
		std::mutex mutex;
		std::shared_ptr<bool> alive;

		friend class EventListener;
	};

	struct Event
	{
	public:
		Event() = default;

		operator bool() const;
		bool Valid() const;

		template<typename E>			E& get()		{ return *reinterpret_cast<E*>(data.get()); }
		template<typename E>	const	E& get() const	{ return *reinterpret_cast<const E*>(data.get()); }

		Identifier ID() const;
		bool IsType(const Identifier& type) const;
		template<EventConcept E>
		bool IsType() const
		{ 
			return IsType(E::type_id);
		}

	private:
		Event(std::unique_ptr<EventData>&& data);
		std::unique_ptr<EventData> data;

		friend class EventListener;
	};

	class EventListener
	{
	public:
		EventListener() = default;
		~EventListener();

		bool Active() const;
		void Listen(EventQueue& queue);
		void StopListening();

		Event GetEvent();

	private:
		std::deque<EventQueue::ListenerQueue>::iterator events;
		EventQueue* queue = nullptr;
		std::shared_ptr<bool> alive;
	};
}