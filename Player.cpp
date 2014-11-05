#include "Player.hpp"
#include <cstdlib>
#include <iostream>
#include <queue>

namespace ducks
{
std::vector<HMM> BirdModels;

std::queue<data> Q;

int kills;
int shots;
int lastBird = 0;


Player::Player()
{
}

ESpecies Player::IDbird(Bird bird)
{
    //std::cerr << "IDBIRD , models: " << BirdModels.size() << std::endl;
    double prob = 0;
    ESpecies id = SPECIES_UNKNOWN;
    for(int i=0;i<(int)BirdModels.size();++i)
    {
        //std::cerr << i<< std::endl;
        double p = BirdModels[i].probability(bird);
        //std::cerr << "prob: " << p << std::endl;
        if(p>prob)
        {
            prob = p;
            id = BirdModels[i].birdID;
        }
    }
    //std::cerr << "done" << std::endl;
    //std::cerr << "ID: " << id << "prob" << prob << std::endl;
    //if(prob < 0.2)
        //return SPECIES_UNKNOWN;
    return id;
}

Action Player::shoot(const GameState &pState, const Deadline &pDue)
{

    //if(pState.getRound()<2 && pState.getBird(0).getSeqLength() < 40)
        return cDontShoot;

    if(pState.getBird(0).getSeqLength() < 70)
        return cDontShoot;


    /*
    * Here you should write your clever algorithms to get the best action.
    * This skeleton never shoots.
    */

    //Build hmm models and calc next move
    std::vector<ESpecies> sp;
    std::vector<EMovement> mo;
    std::vector<double> pr;

    int i = 0;
    while(i<(int)pState.getNumBirds() && pDue.remainingMs() > 500)
    {
        ESpecies species = IDbird(pState.getBird(i));
        if(species != SPECIES_UNKNOWN && species != SPECIES_BLACK_STORK && pState.getBird(i).isAlive())
        {
            HMM temp;
            temp.reset();
            temp.BaumWelch(pState.getBird(i),200);
            if(temp.Converged)
            {
                EMovement m = temp.nextMove(pState.getBird(i));
                sp.push_back(species);
                mo.push_back(m);
            }
        }
    }
    srand(time(NULL));
    if(sp.size()!=0)
    {
        int r = rand()%sp.size();
        return Action(sp[i],mo[r]);
    }


    /*
    for(int b=0;b<(int)pState.getNumBirds() && pDue.remainingMs() > 500;++b)
    {
        Bird bird = pState.getBird((b+lastBird)%pState.getNumBirds());
        if(bird.isAlive())
        {
            ESpecies species = IDbird(bird);
            if(species != SPECIES_BLACK_STORK && species != SPECIES_UNKNOWN)
            {
                HMM temp;
                temp.reset();
                temp.BaumWelch(bird,100);
                if(temp.Converged)
                {
                    //std::cerr << "Shoot converged" << std::endl;
                    EMovement next = temp.nextMove(bird);
                    if(next != MOVE_DEAD)
                    {
                        std::cerr << "SHOOT" << std::endl;
                        shots++;
                        lastBird = (b+lastBird)%pState.getNumBirds();
                        return Action(species, next);
                    }
                }
            }
        }
    }*/
    std::cerr << "dontShoot" << std::endl;
    return cDontShoot;

    //This line would predict that bird 0 will move right and shoot at it
    //return Action(0, MOVE_RIGHT);
    //return Action(SPECIES_PIGEON, MOVE_RIGHT);
}

std::vector<ESpecies> Player::guess(const GameState &pState, const Deadline &pDue)
{

    while(pDue.remainingMs()>10*BirdModels.size() +100 && !Q.empty())
    {
        data d = Q.front();
        Q.pop();
        HMM model;
        model.reset();
        model.BaumWelch(d.bird,600);
        model.birdID = d.birdID;
        if(model.Converged)
        {
            std::cerr << "old model Converged, Bird: " << d.birdID << std::endl;
            BirdModels.push_back(model);
        }
        else
            std::cerr << "old model Not converged, Bird:" << d.birdID << std::endl;
    }


    std::cerr << "Guess  Round " << pState.getRound() << std::endl;
    //ESpecies sp = SPECIES_PIGEON;
    //if(pState.getRound()>1  )
        //sp = SPECIES_UNKNOWN;

    std::vector<ESpecies> lGuesses(pState.getNumBirds(),SPECIES_UNKNOWN);//SPECIES_UNKNOWN);

    for(int bird=0;bird<(int)pState.getNumBirds();++bird)
    {
        //Guess the species of birds by looking at old HMM models
        Bird cBird = pState.getBird(bird);
        ESpecies esp = IDbird(cBird);
        if(esp == SPECIES_UNKNOWN && pState.getRound() < 3)
            esp = SPECIES_PIGEON;
        lGuesses[bird]=esp;
    }

    std::cerr << "Guesses: ";
    for(int i=0;i<(int)pState.getNumBirds();++i)
        std::cerr << lGuesses[i] << " ";
    std::cerr << " Time left: " << pDue.remainingMs() << std::endl;

    return lGuesses;
}

void Player::hit(const GameState &pState, int pBird, const Deadline &pDue)
{
    /*
     * If you hit the bird you are trying to shoot, you will be notified through this function.
     */
    kills++;
    std::cerr << "HIT BIRD!!!" << std::endl;
}

void Player::reveal(const GameState &pState, const std::vector<ESpecies> &pSpecies, const Deadline &pDue)
{
    std::cerr << "Correct: ";
    for(int i=0;i<(int)pSpecies.size();++i)
        std::cerr << pSpecies[i] << " ";
    std::cerr << std::endl;

    int last=0;
    for(int b=0;b<(int)pState.getNumBirds() && pDue.remainingMs() > 400;++b)
    {
        if(pSpecies[b] != -1)
        {
            Bird bird = pState.getBird(b);
            HMM model;
            model.reset();
            model.birdID = pSpecies[b];
            model.BaumWelch(bird,300);
            if(model.Converged)
            {
                //std::cerr << "Converged" << std::endl;
                BirdModels.push_back(model);
            }
            else
                std::cerr << "Not converged, Bird:" << pSpecies[b] << std::endl;
            last = b;
        }
    }
    if(last<(int) pState.getNumBirds())
    {
        for(int i=last;i<(int)pState.getNumBirds();++i)
        {
            if(pSpecies[i] != -1)
                Q.push(data(pState.getBird(i),pSpecies[i]));
        }
    }
    std::cerr << "Time remaining: " << pDue.remainingMs() << " BirdModels: " << BirdModels.size() << " in Queue: " << Q.size() << std::endl;
    std::cerr << "Shots: " << shots << " Kills: " << kills << std::endl;
}
} /*namespace ducks*/
