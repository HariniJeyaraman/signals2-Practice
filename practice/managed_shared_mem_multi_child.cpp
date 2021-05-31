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

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;
   typedef std::atomic<int> MyType;
// typedef int MyType;
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

    
    MyType *instance = segment.construct<MyType>("MyType instance")(15);
    std::cout<<"Value in process parent before any change : "<< *instance<<"\n";

      //Launch child processes
    //   for(int num_child = 0; num_child<3; num_child++){
    //       std::string s(argv[0]); s += " child ";
    //       int pid = std::system(s.c_str());
    //       std::cout<<"Child number : "<<pid<<"\n";
    //         if(0 != pid)
    //             return 1;
    //   }
    namespace bp = boost::process;
    // std::string s = "child_process";
    std::string s = argv[0];
    std::cout<<"Main process : before spawning\n";
    constexpr int num_process = 15;
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
    std::cout<<"Value in parent after child process modifies value : "<< *instance<<"\n";
   }
   else{
      //Open already created shared memory object.
      managed_shared_memory segment(open_only, "MySharedMemory");

        //find the MyType instance in the segment
        MyType *instance = segment.find<MyType>("MyType instance").first;
        if(!instance) return 1;
        std::cout<<"Value of variable in child process without modification : "<<*instance<<"\n";
        //modifying variable value
        //  sleep(1);
        *instance += 1;
       
        std::cout<<"Value of variable in child process after modification : "<<*instance<<"\n";
      
   }
   return 0;
}

//c++ -I /home/harla/Downloads/boost_1_61_0 managed_shared_mem_multi_child.cpp -o example4 -lrt -lpthread
/*
OUTPUT :

*/