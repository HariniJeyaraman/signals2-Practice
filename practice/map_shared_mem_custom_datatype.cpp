#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include<boost/signals2/signal.hpp>
#include<boost/interprocess/containers/map.hpp>
#include<boost/interprocess/allocators/allocator.hpp>
#include<boost/process.hpp>
#include <cassert>
#include<utility>
#include <cstring>
#include <cstdlib>
#include <string>
#include<iostream>
#include<atomic>
#include<vector>
#include<map>

//In order to differentiate the child processes
std::vector<std::string> numArgs(int i){
    std::string is;
    std::vector<std::string> res;
    for(int k=0;k<i;k++){ 
       res.push_back(std::to_string(k));
    }
    return res;
}

struct Signals{
    boost::signals2::signal<void ()> sig1;
    boost::signals2::signal<void ()> sig2;
};

struct CFunction{
    void operator()(){
        std::cout<<"helloo\n";
    }
};

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;
     typedef bool    KeyType;
   typedef void*  MappedType;
   typedef std::pair<const bool, CFunction> ValueType;
    typedef allocator<ValueType, managed_shared_memory::segment_manager>
      ShmemAllocator;


   typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MyMap;
   constexpr int num_process = 3;


   if(argc == 1){  //Parent process
      //Remove shared memory on construction and destruction
      struct shm_remove
      {
         shm_remove() { shared_memory_object::remove("MySharedMemory"); }
         ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
      } remover;

    
    //
    managed_shared_memory segment(create_only,
                             "MySharedMemory", ///Shared memory object name
                              65536);          ///Shared memory object size in bytes

    

   //Note that map<Key, MappedType>'s value_type is std::pair<const Key, MappedType>,
   //so the allocator must allocate that pair.

   //Alias an STL compatible allocator of for the map.
   //This allocator will allow to place containers
   //in managed shared memory segments

   //Initialize the shared memory STL-compatible allocator
   ShmemAllocator alloc_inst (segment.get_segment_manager());

   //Construct a shared memory map.
   //Note that the first parameter is the comparison function,
   //and the second one the allocator.
   //This the same signature as std::map's constructor taking an allocator
   MyMap *mymap =
      segment.construct<MyMap>("MyMap")      //object name
                                 (std::less<bool>() //first  ctor parameter
                                 ,alloc_inst);     //second ctor parameter

   //Insert data in the map
//    for(int i = 0; i < 2; ++i){
//       mymap->insert(std::pair<const bool, CFunction>(true, c.hello));
//    }
    mymap->insert(std::pair<const bool, CFunction>(true, CFunction()));
//    for(int i = 0; i < 2; ++i){
//       mymap->insert(std::pair<const bool, CFunction>(false, c.bye));
//    }

    namespace bp = boost::process;
    // std::string s = "child_process";
    std::string s = argv[0];
    std::cout<<"Main process : before spawning\n";
    
    std::vector<bp::child> children;
    children.reserve(num_process);
    for(int i=1;i<=num_process;i++){

        std::vector<std::string> v = numArgs(i);
        children.emplace_back(bp::child(s, v));
    }
      
    for(auto& ch: children){
        // std::cout<<ch.id()<<"child finished\n";
        ch.join();
    }

   }
   for(int ind = 2;ind<=num_process+1;ind++)
   if(argc == ind){
      //Open already created shared memory object.
      managed_shared_memory segment(open_only, "MySharedMemory");

        //find the MyType instance in the segment
        MyMap *instance = segment.find<MyMap>("MyMap").first;
        if(!instance) return 1;
        std::cout<<"Inside Child : "<<argc<<"\n";
        // (*instance)[true]();
        // sig1.connect((*instance)[true]);
        // sig1.connect((*instance)[false]);
        // sig1();
      
   }

   return 0;
}

//c++ -I /home/harla/Downloads/boost_1_76_0 map_signal_slot.cpp -o map -lrt -lpthread
