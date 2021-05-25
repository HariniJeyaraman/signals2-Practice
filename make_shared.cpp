#include<boost/signals2.hpp>
#include<boost/bind/bind.hpp>
#include<boost/smart_ptr.hpp>
#include<iostream>
using namespace std;
using namespace boost::signals2;
using namespace boost::placeholders;
class Player{
    int numGoals;
    public : string name;
        Player(string s) : name(s) { numGoals = 0; }
        typedef signal<void(string, int)> SignalType;
        SignalType scores;
        void Score(){
            numGoals++;
            scores(name, numGoals);
        }
};

class Coach{
    public: void PlayerScore(string name){
        cout<<"Well done "<<name<<"\n";
    }
};

int main(){
    Player p("John");
    {
    auto coach = boost::make_shared<Coach>();
    //as soon as the shared pointer goes out of scope, we notify the connection mechanism that the object that was listening to the signal has been destroyed
    p.scores.connect(Player::SignalType::slot_type(&Coach::PlayerScore, coach.get(), _1).track(coach)); //track changes to actual Player
    p.Score(); //should get output only here
    }
    // delete coach;
    p.Score(); //should NOT get output here
}

/*
[harla@harla-arch IntroToBoost]$ c++ -I /home/harla/Downloads/boost_1_76_0 make_shared.cpp -o ex -lrt -lpthread
[harla@harla-arch IntroToBoost]$ ./ex
Well done John
*/
//Printed only once, since coach goes out of scope