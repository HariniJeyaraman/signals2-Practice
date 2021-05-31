#include<boost/signals2.hpp>
#include<iostream>

int main(){
    boost::signals2::signal <void()> s;
    {
        auto c = s.connect(1, [](){
            std::cout<<"first\n";
        });
        //scoped_connection limits a signal connection lifetime to a particular scope
        //as scoped_connection goes out of scope, it will break up the connection to c
        //in the scope, will disconnect from the first and we only have the second slot connected in the signal
        boost::signals2::scoped_connection sc(c);
        s.connect(0, [](){
            std::cout<<"second\n";
        });
        s();
    }
    s();
}

/*
[harla@harla-arch IntroToBoost]$ c++ -I /home/harla/Downloads/boost_1_76_0 ex3.cpp -o ex -lrt -lpthread
[harla@harla-arch IntroToBoost]$ ./ex
second
first
second
*/

//As the last signal is invoked using s(); without scoped_connection, both "first" and "second" slots will be connected to the signal and both will be invoked.
//But due to scoped_connection on the "first" slot, when sc goes out of scope, "first" slot gets disconnected from the signal and only the "second" slot gets invoked