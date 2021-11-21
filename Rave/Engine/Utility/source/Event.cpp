#include "Engine/Utility/Event.h"

rv::EventData::EventData(const Identifier& id)
	:
	id(id)
{
}

rv::EventQueue::EventQueue()
	:
	alive(std::make_shared<bool>(true))
{
}

rv::EventQueue::~EventQueue()
{
	*alive = false;
}

void rv::EventQueue::PostEvent(EventData* event)
{
	for (auto& queue : queues)
	{
		std::lock_guard<std::mutex> guard(mutex);
		queue.events.emplace_back(event);
	}
}

rv::Event::operator bool() const
{
	return Valid();
}

bool rv::Event::Valid() const 
{ 
	return data.get(); 
}

rv::Identifier rv::Event::ID() const
{ 
	return data->id; 
}

bool rv::Event::IsType(const Identifier& type) const
{
	return data->id == type;
}

rv::Event::Event(std::unique_ptr<EventData>&& data) 
	: 
	data(std::move(data)) 
{}

rv::EventListener::EventListener(EventQueue& queue)
{
	Listen(queue);
}

rv::EventListener::~EventListener()
{
	StopListening();
}

bool rv::EventListener::Active() const
{
	return queue && events != queue->queues.end() && alive.get() && *alive;
}

void rv::EventListener::Listen(EventQueue& queue)
{
	StopListening();
	this->queue = &queue;
	std::lock_guard<std::mutex> guard(queue.mutex);
	queue.queues.emplace_back();
	events = queue.queues.end() - 1;
	alive = queue.alive;
}

void rv::EventListener::StopListening()
{
	if (Active())
	{
		std::lock_guard<std::mutex> guard(queue->mutex);
		queue->queues.erase(events);

		queue = nullptr;
		events = {};
		alive.reset();
	}
}

rv::Event rv::EventListener::GetEvent()
{
	if (!Active())
		return {};
	std::lock_guard<std::mutex> guard(queue->mutex);
	if (events->events.empty())
		return {};
	std::unique_ptr<EventData> event = std::move(events->events.back());
	events->events.pop_back();
	return event;
}

bool rv::EventListener::Empty() const
{
	if (Active())
		return events->events.empty();
	return false;
}
