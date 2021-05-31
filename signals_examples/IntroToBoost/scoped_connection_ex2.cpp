#include<boost/signals2.hpp>
#include<iostream>
using namespace boost::signals2;
using namespace std;

void third(){
    cout<<"third\n";
}

int main(){
    signal<void()> s;
    s.connect(third); //no priority or group number given
    {
        auto c = s.connect(1, [](){
            cout<<"first\n";
        });
        scoped_connection sc(c);
        s.connect(0, [](){
            cout<<"second\n";
        });
        s();
    }

    cout<<"==============================\n";
    s.disconnect(third);
    s();

}

/*
[harla@harla-arch IntroToBoost]$ c++ -I /home/harla/Downloads/boost_1_76_0 scoped_connection_ex2.cpp -o ex -lrt -lpthread
[harla@harla-arch IntroToBoost]$ ./ex
second
first
third
==============================
second
*/

//after the scoped connection goes out of scope, "first" slot is disconnected. 
//then, the third slot gets disconnected using the s.disconnect()
//when signal is invoked using s() at the end, only second slot is still connected