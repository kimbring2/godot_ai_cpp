#include "cSharedMemory.hpp"

#include <cstdlib> //std::system
#include <cstddef>
#include <cassert>
#include <utility>
#include <typeinfo>
#include <iostream>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/exception/all.hpp>

using namespace boost::interprocess;

cSharedMemory::cSharedMemory() {
    godot::Godot::print("cSharedMemory::cSharedMemory()");
	segment_name = new std::string("environment");
    //found = true;
    try {
		segment = new managed_shared_memory(open_only, segment_name->c_str());
		if (segment == NULL) {
            std::cout << "cSharedMemory Memory segment not found" << std::endl;
		} else {
            std::cout << "cSharedMemory Memory segment found" << std::endl;
        }
	} catch (boost::interprocess::interprocess_exception &e) {
		//print_line(String("cSharedMemory") + String(boost::diagnostic_information(e).c_str()));
        //std::string t((boost::diagnostic_information(e), (boost::diagnostic_information(e));
        std::string arg_s;
        arg_s = boost::diagnostic_information(e).c_str();
        godot::Godot::print(arg_s.c_str());
        //godot::Godot::print("cSharedMemory" + boost::diagnostic_information(e));
	} catch(const char *s) {
		//print_line(String("cSharedMemory") + String(s));
        std::cout << s << std::endl;
        //godot::Godot::print("cSharedMemory" + std::string(s));
	}
};


cSharedMemory::~cSharedMemory() {
    godot::Godot::print("cSharedMemory::~cSharedMemory()");
	//shared_memory_object::remove(segment_name->c_str());
    delete segment;
    delete segment_name;
};


bool cSharedMemory::exists() {
	return found;
}


cPersistentIntTensor* cSharedMemory::findIntTensor(const std::string &s_name) {
	godot::Godot::print("findIntTensor");
    godot::Godot::print("s_name: " + godot::String((s_name).c_str()));

    //segment_name = new std::string("env_action");
	IntVector *shared_vector = segment->find<IntVector>(s_name.c_str()).first;
	if (shared_vector == NULL) {
        //print_line(String("Not found:") + String(s_name.c_str()));
        std::cout << "findIntTensor Not found:" << std::endl;
	} else {
        //print_line(String("Found:") + String(s_name.c_str()));
       std::cout << "findIntTensor Found:" << std::endl;
	}

    cPersistentIntTensor* tensor = new cPersistentIntTensor(shared_vector);

	return tensor;
}


cPersistentFloatTensor* cSharedMemory::findFloatTensor(const std::string &s_name) {
    godot::Godot::print("findFloatTensor");
    godot::Godot::print("s_name: " + godot::String((s_name).c_str()));

    //segment_name = new std::string("env_action");
    FloatVector *shared_vector = segment->find<FloatVector>(s_name.c_str()).first;
    if (shared_vector == NULL) {
        //print_line(String("Not found:") + String(s_name.c_str()));
        std::cout << "findFloatTensor Not found:" << std::endl;
    } else {
        //print_line(String("Found:") + String(s_name.c_str()));
       std::cout << "findFloatTensor Found:" << std::endl;
    }

    cPersistentFloatTensor* tensor = new cPersistentFloatTensor(shared_vector);

    return tensor;
}


cPersistentUintTensor* cSharedMemory::findUintTensor(const std::string &s_name) {
	godot::Godot::print("findUintTensor");
    godot::Godot::print("s_name: " + godot::String((s_name).c_str()));

    //segment_name = new std::string("env_action");
	UintVector *shared_vector = segment->find<UintVector>(s_name.c_str()).first;
	if (shared_vector == NULL) {
        //print_line(String("Not found:") + String(s_name.c_str()));
        std::cout << "findUintTensor Not found:" << std::endl;
	} else {
        //print_line(String("Found:") + String(s_name.c_str()));
       std::cout << "findUintTensor Found:" << std::endl;
	}

    cPersistentUintTensor* tensor = new cPersistentUintTensor(shared_vector);

	return tensor;
}


cSharedMemorySemaphore::cSharedMemorySemaphore() {
    godot::Godot::print("cSharedMemorySemaphore::cSharedMemorySemaphore()");
};


cSharedMemorySemaphore::~cSharedMemorySemaphore() {
    godot::Godot::print("cSharedMemorySemaphore::~cSharedMemorySemaphore()");
    delete name;
    delete region;
    delete mutex;
};


void cSharedMemorySemaphore::init(const std::string &sem_name) {
    try{
        shared_memory_object object(open_only, sem_name.c_str(), read_write);
        region = new mapped_region(object, read_write);
    } catch(interprocess_exception &e) {
        //print_line(String("cSharedMemorySemaphore:init:") + String((*name).c_str()) + String(":") + String(boost::diagnostic_information(e).c_str()));
        //shared_memory_object::remove(name->c_str());
        std::cout << "cSharedMemorySemaphore:init: " << (*name).c_str() << boost::diagnostic_information(e).c_str() << std::endl;
        //godot::Godot::print("cSharedMemorySemaphore:init: " + godot::String((*name).c_str()) + godot::String(boost::diagnostic_information(e).c_str()));
    }

    std::cout << "Constructed semaphore " << sem_name << std::endl;
};


void cSharedMemorySemaphore::post(){
    //std::cout<<"Post semaphore "<<*name<<std::endl;
    mutex = static_cast<interprocess_semaphore*>(region->get_address());
    mutex->post();
    //std::cout<<"Posted semaphore "<<*name<<std::endl;
};


void cSharedMemorySemaphore::wait(){
    // std::cout<<"Wait semaphore "<<*name<<std::endl;
    mutex = static_cast<interprocess_semaphore*>(region->get_address());
    mutex->wait();
    // std::cout<<"Waited semaphore "<<*name<<std::endl;
};


cPersistentIntTensor::cPersistentIntTensor(IntVector *_vector) {
    std::cout << "cPersistentIntTensor::cPersistentIntTensor()" << std::endl;
    vector = _vector;
    size = _vector->size();
}


cPersistentIntTensor::~cPersistentIntTensor() {
    std::cout << "cPersistentIntTensor::~cPersistentIntTensor" << std::endl;
}


void cPersistentIntTensor::write(std::vector<int> &array) {
    for(int i = 0; i < size; i++)
	    (*vector)[i] = array[i];
}


std::vector<int> cPersistentIntTensor::read() {
    //godot::Godot::print("cPersistentIntTensor::read()");
    std::vector<int> data;
    for(int i = 0; i < size; i++) {
        //godot::Godot::print("i: " + godot::String(std::to_string(i).c_str()));
        data.push_back((*vector)[i]);
    }

    return data;
}


cPersistentFloatTensor::cPersistentFloatTensor(FloatVector *_vector) {
    std::cout << "cPersistentFloatTensor::cPersistentFloatTensor()" << std::endl;
    vector = _vector;
    size = _vector->size();
}


cPersistentFloatTensor::~cPersistentFloatTensor() {
    std::cout << "cPersistentFloatTensor::~cPersistentFloatTensor" << std::endl;
}


void cPersistentFloatTensor::write(std::vector<float> &array) {
    for(int i = 0; i < size; i++)
        (*vector)[i] = array[i];
}


std::vector<float> cPersistentFloatTensor::read() {
    //godot::Godot::print("cPersistentIntTensor::read()");
    std::vector<float> data;
    for(int i = 0; i < size; i++) {
        //godot::Godot::print("i: " + godot::String(std::to_string(i).c_str()));
        data.push_back((*vector)[i]);
    }

    return data;
}


cPersistentUintTensor::cPersistentUintTensor(UintVector *_vector) {
    std::cout << "cPersistentUintTensor::cPersistentUintTensor()" << std::endl;
    vector = _vector;
    size = _vector->size();
}


cPersistentUintTensor::~cPersistentUintTensor() {
    std::cout << "cPersistentUintTensor::~cPersistentUintTensor" << std::endl;
}


void cPersistentUintTensor::write(godot::PoolByteArray &array) {
   //for(int i = 0; i < size; i++)
   //    (*vector)[i] = array[i];
   int size_channel = int(size / 3);
   int offset_channel_1 = size_channel * 1;
   int offset_channel_2 = size_channel * 2;
   for (int i = 0; i < size_channel; i++) {
       (*vector)[i] = array[i];
       (*vector)[i + offset_channel_1] = array[i + offset_channel_1];
       (*vector)[i + offset_channel_2] = array[i + offset_channel_2];
   }
   
   //vector = array.ptr();
   //memcpy(vector, array.write().ptr(), array.size());
   //vector = array.write().ptr();
   //vector->assign(array.write().ptr(), array.write().ptr() + size);
}


godot::PoolByteArray cPersistentUintTensor::read() {
    //godot::Godot::print("cPersistentUintTensor::read()");
    godot::PoolByteArray data;
    for(int i = 0; i < size; i++) {
        //godot::Godot::print("i: " + godot::String(std::to_string(i).c_str()));
        data.push_back((*vector)[i]);
    }

    return data;
}