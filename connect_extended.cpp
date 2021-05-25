#include <bits/c++config.h>
#include<iostream>
#include<boost/signals2.hpp>

using namespace std;
using namespace boost::signals2;
//functor
//to return the average of the values of both the slots.
//avg of 3+4 and 3*4 which is (7+12)/2 = 9.5
template<typename T> struct average{
    typedef T result_type;
    //Input Iterator
    template<typename II> T operator()(II first, II last) const{
        if(first == last) return T();
        float sum=0.0;
        int count = 0;
        while(first!=last){
            sum += *first++;
            count++;
        }
        return sum/count;
    }
};

void f(){
    signal<float(float, float), average<float>> s;
    s.connect([](float a, float b){
        return a+b;
    });
    s.connect([](float a, float b){
        return a*b;
    });
    cout<<s(3,4)<<"\n";
}

int main(int argc, char *argv[]){
    signal<void(int)> values;
    //disconnecting after 5 times
    //signal will get disconnected even though the signal is being called a 100 times using a foor loop
    values.connect_extended([](const connection& conn, int value){
        static int count = 0;
        if(++count>5){
            conn.disconnect();
        }
        else{
            cout<<"got a value of "<<value<<"\n";
        }
    });

    srand((unsigned)time(nullptr));
    for(size_t i = 0; i < 100; i++){
        values(rand());
    }
}

/*
[harla@harla-arch IntroToBoost]$ c++ -I /home/harla/Downloads/boost_1_76_0 connect_extended.cpp -o ex -lrt -lpthread
[harla@harla-arch IntroToBoost]$ ./ex
got a value of 1109463953
got a value of 234555407
got a value of 2016758240
got a value of 510510536
got a value of 42147100
*/

//We get just the 5 values and disconnect entirely from that signal