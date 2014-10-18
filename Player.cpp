#include "Player.hpp"
#include <cstdlib>
#include <iostream>
#include <queue>

namespace ducks
{
std::vector<HMM> BirdModels;

std::queue<data> Q;

Player::Player()
{
}

ESpecies Player::IDbird(Bird bird)
{
    double prob = 0;
    ESpecies id = SPECIES_UNKNOWN;
    for(int i=0;i<(int)BirdModels.size();++i)
    {
        //std::cerr << i<< std::endl;
        double p = BirdModels[i].probability(bird);
        if(p>prob)
        {
            prob = p;
            id = BirdModels[i].birdID;
        }
    }
    return id;
}

Action Player::shoot(const GameState &pState, const Deadline &pDue)
{

    //if(pState.getRound()<2 && pState.getBird(0).getSeqLength() < 40)
        return cDontShoot;

    /*
    * Here you should write your clever algorithms to get the best action.
    * This skeleton never shoots.
    */

    //HMM hmm;
    //hmm.reset();

    int i=0;
    double p = 0;
    EMovement next = MOVE_STOPPED;
    ESpecies sp = SPECIES_UNKNOWN;

    while((i<(int)pState.getNumBirds()) && (pDue.remainingMs() > 100))
    {
        Bird bird = pState.getBird(i);
        //std::cerr << "GetBird" << std::endl;
        if(bird.isAlive())
        {
            ESpecies tempSP = IDbird(bird);
            for(int j=0;j<(int)BirdModels.size();++j)
            {
                double temp = BirdModels[j].probability(bird);
                //std::cerr << temp << std::endl;
                if(temp > p)
                {
                    p=temp;
                    next = BirdModels[j].nextMove(bird);
                    std::cerr << next << std::endl;
                    sp = tempSP;
                }
            }
        }
        ++i;
    }
    std::cerr << "next" << std::endl;
    if(next != MOVE_DEAD)
        return Action(sp, next);
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
    ESpecies sp = SPECIES_PIGEON;
    if(pState.getRound()>1  )
        sp = SPECIES_UNKNOWN;

    std::vector<ESpecies> lGuesses(pState.getNumBirds(),sp);//SPECIES_UNKNOWN);

    for(int bird=0;bird<(int)pState.getNumBirds() && pState.getRound()>1 ;++bird)
    {
        //Guess the species of birds by looking at old HMM models
        Bird cBird = pState.getBird(bird);
        lGuesses[bird]=IDbird(cBird);
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
}
} /*namespace ducks*/
