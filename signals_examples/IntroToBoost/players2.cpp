#include<iostream>
// #include<boost/signals2/signal.hpp>
// #include<boost/signals2/shared_connection_block.hpp>
#include<boost/signals2.hpp>
#include <string>
//in this example we are not invoking the signal directly
//we do so using the function Score
class Player{
    int numGoals;
    public : std::string name;
             boost::signals2::signal<void(std::string, int)> scores;
             Player(std::string s) : name(s) {
                 numGoals = 0;
             }
             void Score(){
                 numGoals++;
                 scores(name, numGoals);
             }
};


int main(){
   Player p("John");
   boost::signals2::connection c;
   c = p.scores.connect([](std::string name, int numGoals){
        std::cout<<"Well done "<<name<<" You have scored "<<numGoals<<" goals!\n";
    });  //lambda
    p.Score();
    p.Score();
    //Shared connection block 
    //will only live within the scope of these braces
    //temporarily block the slots
    {
        boost::signals2::shared_connection_block b{c};
        p.Score();
        p.Score();
    }
    p.Score();
    p.Score();
    p.Score();
    p.Score();
    return 0;
}

/*
[harla@harla-arch IntroToBoost]$ c++ -I /home/harla/Downloads/boost_1_76_0 players2.cpp -o ex -lrt -lpthread
[harla@harla-arch IntroToBoost]$ ./ex
Well done John You have scored 1 goals!
Well done John You have scored 2 goals!
Well done John You have scored 5 goals!
Well done John You have scored 6 goals!
Well done John You have scored 7 goals!
Well done John You have scored 8 goals!
*/

//Note that the 2 slots after the shared_connection_block are not invoked
//The remaining 4 slots are invoked because the block goes out of scope