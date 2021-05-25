#include<boost/signals2.hpp>
#include<iostream>
using namespace std;
using namespace boost::signals2;

int main(int argc, char* argv[]){
    signal<float(float, float)> s;
    s.connect([](float a, float b){
        return a+b;
    });
    s.connect([](float a, float b){
        return a*b;
    });
    //The default behavior of the signal is to return the pointer to the last returning value
    cout<<*s(3,4)<<"\n"; //deferencing the pointer
}