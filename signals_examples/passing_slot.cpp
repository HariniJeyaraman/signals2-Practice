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

class Button{
    typedef boost::signals2::signal <void (int x, int y)> OnClick;
    OnClick onclick;
    public : typedef OnClick::slot_type OnClickSlotType;
    //should add "const"
    boost::signals2::connection doOnClick(const OnClickSlotType &slot){
        return onclick.connect(slot);
    } 
    void simulateOnClick(){
        onclick(52, 38);
    }
};

void printCoord(int x, int y){
        std::cout<<" x : "<<x<<" y : "<<y<<"\n";
    }

int main(){
    Button b;
    b.doOnClick(&printCoord);
    b.simulateOnClick();
}