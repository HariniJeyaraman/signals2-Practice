#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include<boost/signals2/signal.hpp>
#include<boost/interprocess/containers/map.hpp>
#include<boost/interprocess/allocators/allocator.hpp>
#include<boost/process.hpp>
#include<sys/types.h>
#include<unistd.h>
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

void func1_for_signal1(std::string s){
    std::cout<<"Func 1 from signal 1 : "<<s<<"\n";;
}

void func2_for_signal1(std::string s){
    std::cout<<"Func 2 from signal 1 : "<<s<<"\n";
}

void func1_for_signal2(std::string s){
    std::cout<<"Func1 subscribed to signal 2"<<s<<"\n";
}

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;
    
   constexpr int num_process = 10;
   boost::signals2::signal<void (std::string)> sig1;
   boost::signals2::signal<void (std::string)> sig2;
   std::map<std::string, std::vector<void(*)(std::string)> > map_str_ptr;
    map_str_ptr["Signal1"].push_back(func1_for_signal1);
    map_str_ptr["Signal1"].push_back(func2_for_signal1);
    map_str_ptr["Signal2"].push_back(func1_for_signal2);

   if(argc == 1){  //Parent process

    namespace bp = boost::process;
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

   if(argc!=1 && argc%2==0){
        std::cout<<"Inside Even Child invoking signal 2: "<<argc<<"\n";
        for(auto ch : map_str_ptr["Signal2"]){
            sig2.connect(ch);
        }
        sig2("S2");
        sleep(10);    
   }
 
  if(argc!=1 && argc%2==1){
      sleep(10);
      
      std::cout<<"Inside Odd Child invoking signal 1 : "<<argc<<"\n";//<<argc<<"\n";
        for(auto ch : map_str_ptr["Signal1"]){
            sig1.connect(ch);
        }
            if(argc==5){
        //   pid_t pid = getpid();
        //   std::cout<<pid<<std::endl;
        float crash = 1/0;
        std::cout<<crash<<std::endl;
        sig1("S1");
        }

        sig1("S1");  
        
  }
   return 0;
}

/*
[harla@harla-arch signals_examples]$ c++ -I /home/harla/Downloads/boost_1_76_0 map_signal_slot_without_sharedmem.cpp -o map_no_mem -lrt -lpthread
[harla@harla-arch signals_examples]$ ./map_no_mem
Main process : before spawning
Inside Even Child Implementing invoking signal 2: 2
Func1 subscribed to signal 2S2
Inside Even Child Implementing invoking signal 2: 4
Func1 subscribed to signal 2S2
Inside Even Child Implementing invoking signal 2: 6
Func1 subscribed to signal 2S2
Inside Even Child Implementing invoking signal 2: 8
Func1 subscribed to signal 2S2
Inside Even Child Implementing invoking signal 2: 10
Func1 subscribed to signal 2S2
Inside Odd Child Implementing invoking signal 1: 
Func 1 from signal 1 : S1
Func 2 from signal 1 : S1
Inside Odd Child Implementing invoking signal 1: 
Func 1 from signal 1 : S1
Func 2 from signal 1 : S1
Inside Odd Child Implementing invoking signal 1: 
Func 1 from signal 1 : S1
Func 2 from signal 1 : S1
Inside Odd Child Implementing invoking signal 1: 
Func 1 from signal 1 : S1
Func 2 from signal 1 : S1
Inside Odd Child Implementing invoking signal 1: 
Func 1 from signal 1 : S1
Func 2 from signal 1 : S1
*/

