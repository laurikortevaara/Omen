#pragma once

#include <sys/types.h> 

namespace omen {
	class IMemoryManager
	{
	public:
		virtual void* allocate(size_t) = 0;
		virtual void   free(void*) = 0;
	};

	struct FreeStore
	{
		FreeStore* next;
	};

	class MemoryManager : public IMemoryManager
	{
		struct FreeStore
		{
			FreeStore *next;
		};
		void expandPoolSize();
		void cleanUp();
		FreeStore* freeStoreHead;
	public:
		MemoryManager() {
			freeStoreHead = 0;
			expandPoolSize();
		}
		virtual ~MemoryManager() {
			cleanUp();
		}
		virtual void* allocate(size_t);
		virtual void   free(void*);
	};

}