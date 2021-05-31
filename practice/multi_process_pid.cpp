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
   typedef std::atomic<int> MyType;
   constexpr int num_process = 3;
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
    //child has exited, so print and check the value of the shared instance variable
    std::cout<<"Value in parent after child process modifies value : "<< *instance<<"\n";
   }
   for(int ind = 2;ind<=num_process+1;ind++)
   if(argc == ind){
      //Open already created shared memory object.
      managed_shared_memory segment(open_only, "MySharedMemory");

        //find the MyType instance in the segment
        MyType *instance = segment.find<MyType>("MyType instance").first;
        if(!instance) return 1;
        std::cout<<"Value of variable in child process without modification : "<<*instance<<"\n";
        //modifying variable value
        //  sleep(1);
        *instance += 1;
       
        std::cout<<"Value of variable in child process : " <<argc<<"  after modification : "<<*instance<<"\n";
      
   }

   return 0;
}

//c++ -I /home/harla/Downloads/boost_1_61_0 managed_shared_mem_multi_child.cpp -o example4 -lrt -lpthread
/*
OUTPUT :

*/