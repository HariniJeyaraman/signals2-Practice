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

int main(int argc, char *argv[]){
    signal<float(float, float), average<float>> s;
    s.connect([](float a, float b){
        return a+b;
    });
    s.connect([](float a, float b){
        return a*b;
    });
    cout<<s(3,4)<<"\n";
}

/*
[harla@harla-arch IntroToBoost]$ c++ -I /home/harla/Downloads/boost_1_76_0 slot_return_value_ex2.cpp -o ex -lrt -lpthread
[harla@harla-arch IntroToBoost]$ ./ex
9.5
*/