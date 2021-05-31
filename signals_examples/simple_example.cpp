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

class CDoubleSlot{
    public : void operator() (int &i) const{
        i*=2;
    }
};

class CAnotherSlot{
    public : void operator()(int &i) const{
        i+=3;
    }
};

int main(){
    boost::signals2::signal<void (int &)> sig;
    sig.connect(CDoubleSlot());
    sig.connect(CAnotherSlot());
    int result = 12;
    sig(result);
    std::cout<<"Result : "<<result<<"\n";
    return 0;
}