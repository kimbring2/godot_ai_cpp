#include <Godot.hpp>

#include <string>
#include <vector>
#include <exception>
#include <iostream>
#include <istream>
#include <streambuf>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/exception/all.hpp>

#include "PoolArrays.hpp"

using namespace boost::interprocess;

typedef allocator<int, managed_shared_memory::segment_manager> ShmemAllocatorInt;
typedef allocator<float, managed_shared_memory::segment_manager> ShmemAllocatorFloat;
typedef allocator<uint8_t, managed_shared_memory::segment_manager> ShmemAllocatorUint;

typedef std::vector<int, ShmemAllocatorInt> IntVector;
typedef std::vector<float, ShmemAllocatorFloat> FloatVector;
typedef std::vector<uint8_t, ShmemAllocatorUint> UintVector;


class cPersistentIntTensor {
	private:
		IntVector *vector = NULL;
		int size;

	public:
		cPersistentIntTensor(IntVector *_vector);
		~cPersistentIntTensor();
		void write(std::vector<int> &array);
		std::vector<int> read();
};


class cPersistentFloatTensor {
    private:
        FloatVector *vector = NULL;
        int size;

    public:
        cPersistentFloatTensor(FloatVector *_vector);
        ~cPersistentFloatTensor();
        void write(std::vector<float> &array);
        std::vector<float> read();
};


class cPersistentUintTensor {
	private:
		UintVector *vector = NULL;
		int size;

	public:
		cPersistentUintTensor(UintVector *_vector);
		~cPersistentUintTensor();
		void write(godot::PoolByteArray &array);
		godot::PoolByteArray read();
};


class cSharedMemory {
    private:
        std::string *segment_name = NULL;
        managed_shared_memory *segment = NULL;

    public:
        cSharedMemory();
        ~cSharedMemory();

        //Ref<cPersistentFloatTensor> findFloatTensor(const String &name);
        //cPersistentIntTensor* findIntTensor(const std::string &name);
        cPersistentUintTensor* findUintTensor(const std::string &s_name);
        cPersistentFloatTensor* findFloatTensor(const std::string &s_name);
        cPersistentIntTensor* findIntTensor(const std::string &s_name);

        bool found;
        bool exists();
};


class cSharedMemorySemaphore {
    private:
        std::string *name;
        mapped_region *region;
        interprocess_semaphore *mutex;
    
    public:
        cSharedMemorySemaphore();
        ~cSharedMemorySemaphore();

        void init(const std::string &sem_name);
        void post();
        void wait();
};
