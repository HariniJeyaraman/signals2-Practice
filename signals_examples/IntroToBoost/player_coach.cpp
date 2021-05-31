#include<boost/signals2.hpp>
#include<boost/bind/bind.hpp>
#include<iostream>
using namespace std;
using namespace boost::signals2;
using namespace boost::placeholders;
class Player{
    int numGoals;
    public : string name;
        Player(string s) : name(s) { numGoals = 0; }
        signal<void(string, int)> scores;
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
    Coach coach;
    p.scores.connect(boost::bind(&Coach::PlayerScore, &coach,_1));
    p.Score();
    p.Score();
}

/*
[harla@harla-arch IntroToBoost]$ c++ -I /home/harla/Downloads/boost_1_76_0 player_coach.cpp -o ex -lrt -lpthread
[harla@harla-arch IntroToBoost]$ ./ex
Well done John
Well done John
*/