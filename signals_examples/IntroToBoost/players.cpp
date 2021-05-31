#include<iostream>
#include<cstdlib>
#include<boost/signals2/signal.hpp>
#include <string>

class Player{
    public : std::string name;
             boost::signals2::signal<void()> scores;
             Player(std::string s) : name(s) {}
};


int main(){
    Player p("John");
   auto c = p.scores.connect([&p](){
        std::cout<<"Well done "<<p.name<<"!\n";
    });  //lambda
    p.scores();
    p.scores();
    // c.disconnect();
    p.scores.disconnect_all_slots();
    p.scores();
    p.scores();
    p.scores();
    p.scores();
    p.scores();
}