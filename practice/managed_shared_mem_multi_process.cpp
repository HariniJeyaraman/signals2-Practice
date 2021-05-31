/**Boost.Interprocess offers some basic classes to create shared memory objects and file mappings and map those mappable classes to the process' address space. 
 *A mapped region is a fixed-length memory buffer and creating and destroying objects of any type dynamically, requires a lot of work,
 *since it would require programming a memory management algorithm to allocate portions of that segment. 
 *Many times, we also want to associate names to objects created in shared memory, so all the processes can find the object using the name.

Constructing Managed Shared Memory

Managed shared memory is an advanced class that combines a shared memory object and a mapped region that covers all the shared memory object. 
That means that when we create a new managed shared memory:

    A new shared memory object is created.
    The whole shared memory object is mapped in the process' address space.
    Some helper objects are constructed (name-object index, internal synchronization objects, internal variables...) in the mapped region to implement 
    managed memory segment features.

When we open a managed shared memory

    A shared memory object is opened.
    The whole shared memory object is mapped in the process' address space.

*/
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cassert>
#include<utility>
#include <cstring>
#include <cstdlib>
#include <string>
#include<iostream>

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;
   typedef int MyType;

   if(argc == 1){  //Parent process
      //Remove shared memory on construction and destruction
      struct shm_remove
      {
         shm_remove() { shared_memory_object::remove("MySharedMemory"); }
         ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
      } remover;

    //Create a shared memory object.
    //1.  Creates a new shared memory object
    //    called "MySharedMemory".
    //2.  Maps the whole object to this
    //    process' address space.
    //3.  Constructs some objects in shared memory
    //    to implement managed features.
    //!!  If anything fails, throws interprocess_exception
    //
    managed_shared_memory segment(create_only,
                             "MySharedMemory", ///Shared memory object name
                              65536);          ///Shared memory object size in bytes

    
    //When constructing objects in a managed memory segment (managed shared memory, managed mapped files...) associated with a name, 
    //the user has a varied object construction family to "construct" or to "construct if not found". 
    //Boost.Interprocess can construct a single object or an array of objects. 
    //The array can be constructed with the same parameters for all objects or we can define each parameter from a list of iterators:
    //Allocates and constructs an object of type MyType (throwing version) 
    MyType *instance = segment.construct<MyType>("MyType instance")(15);
    std::cout<<"Value in process parent before any change : "<< *instance<<"\n";

      //Launch child processes
      for(int num_child = 0; num_child<3; num_child++){
          std::string s(argv[0]); s += " child ";
          int pid = std::system(s.c_str());
          std::cout<<"Child number : "<<pid<<"\n";
            if(0 != pid)
                return 1;
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
        *instance += 1;

        std::cout<<"Value of variable in child process after modification : "<<*instance<<"\n";
      
   }
   return 0;
}

//c++ -I /home/harla/Downloads/boost_1_61_0 managed_shared_mem.cpp -o example2 -lrt -lpthread
/*
OUTPUT :
[harla@harla-arch Boost_Pub_Sub]$ c++ -I /home/harla/Downloads/boost_1_61_0 managed_shared_mem.cpp -o example2 -lrt -lpthread
[harla@harla-arch Boost_Pub_Sub]$ ./example2
Value in process parent before any change : 15
Value of variable in child process without modification : 15
Value of variable in child process after modification : 16
Value in parent after child process modifies value : 16
*/