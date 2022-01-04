#include "Engine/Utility/Multimap.h"

rv::MultiMap::MultiMap(const MultiMap& rhs)
{
}

rv::MultiMap::MultiMap(MultiMap&& rhs) noexcept
	:
	first(rhs.first)
{
	rhs.first = nullptr;
}

rv::MultiMap::~MultiMap()
{
	if (first)
	{
		destroy(first);
		delete first;
	}
}

rv::MultiMap::Header* rv::MultiMap::get_header(size_t key)
{
	return get_header_const(key);
}

const rv::MultiMap::Header* rv::MultiMap::get_header(size_t key) const
{
	return get_header_const(key);
}

rv::MultiMap::Header* rv::MultiMap::get_header_const(size_t key) const
{
	for (Header* header = first; header; )
	{
		if (key < header->key)
			header = header->left;
		else if (key > header->key)
			header = header->right;
		else
			return header;
	}
	return nullptr;
}

void rv::MultiMap::destroy(Header* header)
{
	if (header)
	{
		if (header->destructor)
			header->destructor(data<void>(header));
		destroy(header->next);
		destroy(header->left);
		destroy(header->right);
	}
}

void rv::MultiMap::destroy(SequentialHeader* header)
{
	if (header)
	{
		if (header->destructor)
			header->destructor(data(header));
		destroy(header->next);
	}
}

rv::MultiMap::Header::~Header()
{
	if (next)
		delete next;

	if (left)
		delete left;

	if (right)
		delete right;
}

rv::MultiMap::SequentialHeader::~SequentialHeader()
{
	if (next)
		delete next;
}
