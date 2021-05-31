#include <cassert>
#include<utility>
#include <cstring>
#include <cstdlib>
#include <string>
#include<iostream>
#include<boost/signals2/signal.hpp>

struct Hello{
    void operator()(){
        std::cout<<"Hello World\n";
    }
};

struct AddNum{
    void operator()(int a, int b){
        std::cout<<"Sum is : "<<a+b<<"\n";
    }
};

struct MultNum{
    void operator()(int a, int b){
        std::cout<<"Product : "<<a*b<<"\n";
    }
};

void multiply_func(int a, int b){
    if(b==0) std::cout<< -1;
    else std::cout<<a*b<<std::endl;
}

int main(){
    boost::signals2::signal<void ()> sig1;
    boost::signals2::signal<void (int, int)> sig2;
    Hello hello;
    AddNum addnum;
    sig1.connect(hello);
    sig2.connect(addnum);
    sig2.connect(MultNum());
    sig2.connect(multiply_func);
    sig1();
    sig2(2, 3);
}

/*
[harla@harla-arch signals_examples]$ c++ -I /home/harla/Downloads/boost_1_61_0 example1.cpp -lrt
[harla@harla-arch signals_examples]$ ./a.out
Hello World
*/