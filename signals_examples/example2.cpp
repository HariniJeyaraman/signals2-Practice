#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include<boost/process.hpp>
#include <cassert>
#include<utility>
#include <cstring>
#include <cstdlib>
#include <string>
#include<iostream>
#include<atomic>
#include<vector>
#include<boost/signals2/signal.hpp>

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;
    struct Hello{
        void operator()(){
            std::cout<<"Hello World\n";
        }
    };
    typedef struct Hello MyType;
    boost::signals2::signal<void ()> sig1;
   if(argc == 1){  //Parent process
      //Remove shared memory on construction and destruction
      struct shm_remove
      {
         shm_remove() { shared_memory_object::remove("MySharedMemory"); }
         ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
      } remover;

   
    
    managed_shared_memory segment(create_only,
                             "MySharedMemory", ///Shared memory object name
                              65536);          ///Shared memory object size in bytes

    
    MyType *instance = segment.construct<MyType>("MyType instance")();
    // std::cout<<"Value in process parent before any change : "<< *instance<<"\n";

    namespace bp = boost::process;
    // std::string s = "child_process";
    std::string s = argv[0];
    std::cout<<"Main process : before spawning\n";
    constexpr int num_process = 2;
    std::vector<bp::child> children;
    children.reserve(num_process);
    for(int i=0;i<num_process;i++){
        std::stringstream ss;
        ss<<i;
        std::string is;
        ss>>is;
        children.emplace_back(bp::child(s, std::vector<std::string> {is}));
    }
      
    for(auto& ch: children){
        // std::cout<<ch.id()<<"child finished\n";
        ch.join();
    }
    //child has exited, so print and check the value of the shared instance variable
    // std::cout<<"Value in parent after child process modifies value : "<< *instance<<"\n";
   }
   else{
      //Open already created shared memory object.
      managed_shared_memory segment(open_only, "MySharedMemory");
        //find the MyType instance in the segment
        MyType *instance = segment.find<MyType>("MyType instance").first;
        if(!instance) return 1;
        std::cout<<"Inside Child\n";
        struct Hello *h;
        h = instance;
        sig1.connect(*h);
        sig1();

       
   }
   return 0;
}

//c++ -I /home/harla/Downloads/boost_1_61_0 managed_shared_mem_multi_child.cpp -o example4 -lrt -lpthread
/*
OUTPUT :

*/
