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

class SomeSlot{
    std::string s_;
    public :
        SomeSlot(const std::string &s) : s_(s) {}
        void operator()() const{
            std::cout<<s_<<"\n";
        }
};

int main(){
    boost::signals2::signal <void()> sig;
    SomeSlot s1("I should be called first !");
    SomeSlot s2("I can be called anytime, order doesn't matter !");
    SomeSlot s3("I should be called second !");
    sig.connect(s2);
    sig.connect(1,s3);
    sig.connect(0,s1);
    sig();
    return 0;
}