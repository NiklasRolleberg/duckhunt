#include "Player.hpp"
#include <cstdlib>
#include <iostream>

namespace ducks
{
std::vector<HMM> BirdModels;

Player::Player()
{
}

ESpecies Player::IDbird(Bird bird)
{
    std::vector<int> seq((int)bird.getSeqLength());
    for(int i=0;i<(int)bird.getSeqLength();++i)
        seq[i] = bird.getObservation(i);
    double probability = 0;
    int id = -1;
    for(int i=0;i<(int)BirdModels.size();++i)
    {
        //std::cerr << i<< std::endl;
        double p = BirdModels[i].probability(seq);
        if(p>probability)
        {
            probability = p;
            id = BirdModels[i].birdID;
        }
    }
    std::cerr << "Probability "<< probability  <<"bird " << id << std::endl;
    switch(id)
    {
    case 0: return SPECIES_PIGEON;
        break;
    case 1: return SPECIES_RAVEN;
        break;
    case 2: return SPECIES_SKYLARK;
        break;
    case 3: return SPECIES_SWALLOW;
        break;
    case 4: return SPECIES_SNIPE;
        break;
    case 5: return SPECIES_BLACK_STORK;
        break;
    default:
        std::cerr << "Default" << std::endl;
    }

    return SPECIES_PIGEON;
}

Action Player::shoot(const GameState &pState, const Deadline &pDue)
{
    /*
    * Here you should write your clever algorithms to get the best action.
    * This skeleton never shoots.
    */

    return cDontShoot;

    //This line would predict that bird 0 will move right and shoot at it
    //return Action(0, MOVE_RIGHT);
    //return Action(SPECIES_PIGEON, MOVE_RIGHT);
}

std::vector<ESpecies> Player::guess(const GameState &pState, const Deadline &pDue)
{
    std::cerr << "Guess" << std::endl;
    ESpecies sp = SPECIES_UNKNOWN;
    if(pState.getRound()<2)
        sp = SPECIES_PIGEON;

    std::vector<ESpecies> lGuesses(pState.getNumBirds(),sp);//SPECIES_UNKNOWN);

    for(int bird=0;bird<(int)pState.getNumBirds();++bird)
    {
        //Guess the species of birds by looking at old HMM models
        Bird cBird = pState.getBird(bird);
        lGuesses[bird]=IDbird(cBird);
    }

    std::cerr << "Guesses: ";
    for(int i=0;i<(int)pState.getNumBirds();++i)
        std::cerr << lGuesses[i] << " ";
    std::cerr << std::endl;

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

    for(int b=0;b<(int)pState.getNumBirds() && pDue.remainingMs() > 300;++b)
    {
        Bird bird = pState.getBird(b);
        std::vector<int> seq((int)bird.getSeqLength());
        for(int i=0;i<(int)bird.getSeqLength();++i)
            seq[i] = bird.getObservation(i);
        HMM model;
        model.reset();
        model.BaumWelch(seq);
        model.birdID = pSpecies[b];
        if(model.Converged)
        {
            //std::cerr << "Converged" << std::endl;
            BirdModels.push_back(model);
        }
        else
            std::cerr << "Not converged, Bird:" << pSpecies[b] << std::endl;
    }
    std::cerr << "BirdModels: " << BirdModels.size() << " Time left: "<< pDue.remainingMs() << std::endl;
}


} /*namespace ducks*/
