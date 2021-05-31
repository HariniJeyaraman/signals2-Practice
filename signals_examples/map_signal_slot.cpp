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

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;
     typedef int    KeyType;
   typedef float  MappedType;
   typedef std::pair<const int, float> ValueType;
    typedef allocator<ValueType, managed_shared_memory::segment_manager>
      ShmemAllocator;

   typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MyMap;
   constexpr int num_process = 3;
   std::map<bool, std::vector<void(*)()> > map_str_fp;

   boost::signals2::signal<void ()> sig1;

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

   //Initialize the shared memory STL-compatible allocator
   ShmemAllocator alloc_inst (segment.get_segment_manager());

   MyMap *mymap =
      segment.construct<MyMap>("MyMap")      //object name
                                 (std::less<int>() //first  ctor parameter
                                 ,alloc_inst);     //second ctor parameter

   //Insert data in the map
   for(int i = 0; i < 5; ++i){
      mymap->insert(std::pair<const int, float>(1, 111.111));
   }

   for(int i = 0; i < 5; ++i){
      mymap->insert(std::pair<const int, float>(0, 111.000));
   }

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
        std::cout<<(*instance)[1]<<std::endl;
      
   }

   return 0;
}

//c++ -I /home/harla/Downloads/boost_1_76_0 map_signal_slot.cpp -o map -lrt -lpthread
