#include "Player.hpp"
#include <cstdlib>
#include <iostream>

namespace ducks
{

std::vector<HMM> OldModels;
std::vector<HMM> newModels;

Player::Player()
{
}

Action Player::shoot(const GameState &pState, const Deadline &pDue)
{
    /*
    * Here you should write your clever algorithms to get the best action.
    * This skeleton never shoots.
    */
    if(pState.getBird(0).getSeqLength()<40)// || pState.getBird(0).getSeqLength()%5!=0)
        return cDontShoot;

    int nBirds = pState.getNumBirds();
    std::cerr << "Birds: " << nBirds;
    std::cerr << "  Time: " << pState.getBird(0).getSeqLength() << std::endl;
    if (newModels.size() == 0)
        newModels = std::vector<HMM>(nBirds);
    for(int i=0;(i<nBirds) && (pDue.remainingMs() > 100) ;++i)
    {
        int nStates = pState.getBird(i).getSeqLength();
        std::vector<int> seq(nStates);
        bool go = true;
        for(int j=0;j<nStates;++j)
        {
            seq[i] = pState.getBird(i).getObservation(j);
            if(seq[i] == -1)
            go=false;
            break;
        }
        if(go)
        {
            newModels[i].reset();
            newModels[i].BaumWelch(seq);

            if(newModels[i].converged)
            {
                /*figure out where the bird will be*/
                std::vector<double> P = newModels[i].probability(seq[seq.size()-1]);

                double maximum = 0;
                int dir = -1;
                for(int j=0;j<newModels[i].N;++j)
                {
                    double tempMax = 0;
                    int tempdir = -1;
                    for(int k=0;k<(int)P.size();++k)
                    {
                        if(P[k]>tempMax)
                        {
                            tempMax = P[i];
                            tempdir = k;
                        }
                    }
                    if (tempMax > maximum)
                    {
                        maximum = tempMax;
                        dir = tempdir;
                    }
                }
                if(dir != -1 && maximum > 0.3)
                {
                    std::cerr << "Bird: " << i << " Probability:" << maximum << std::endl;
                    std::cerr << "Shoot!!! Time left:" << pDue.remainingMs() << std::endl;

                    switch ( dir )
                    {
                    case 0:
                        return Action(SPECIES_UNKNOWN, MOVE_UP_LEFT);
                        break;
                    case 1:
                        return Action(SPECIES_UNKNOWN, MOVE_UP);
                        break;
                    case 2:
                        return Action(SPECIES_UNKNOWN, MOVE_UP_RIGHT);
                        break;
                    case 3:
                        return Action(SPECIES_UNKNOWN, MOVE_LEFT);
                        break;
                    case 4:
                        return Action(SPECIES_UNKNOWN, MOVE_STOPPED);
                        break;
                    case 5:
                        return Action(SPECIES_UNKNOWN, MOVE_RIGHT);
                        break;
                    case 6:
                        return Action(SPECIES_UNKNOWN, MOVE_DOWN_LEFT);
                        break;
                    case 7:
                        return Action(SPECIES_UNKNOWN, MOVE_DOWN);
                        break;
                    case 8:
                        return Action(SPECIES_UNKNOWN, MOVE_DOWN_RIGHT);
                        break;

                    default:
                        std::cerr << "Nu blev det fel" << std::endl;
                        return cDontShoot;
                    }
                }
            }
        }
    }

    //std::cerr << "Birds: " << pState.getNumBirds() << std::endl;
    //std::cerr << "Time: " << pState.getBird(0).getSeqLength() << std::endl;
    // This line choose not to shoot
    std::cerr << "--------" << pDue.remainingMs() << std::endl;
    return cDontShoot;

    //This line would predict that bird 0 will move right and shoot at it
    //return Action(0, MOVE_RIGHT);

    //return Action(SPECIES_PIGEON, MOVE_RIGHT);
}

std::vector<ESpecies> Player::guess(const GameState &pState, const Deadline &pDue)
{
    /*
     * Here you should write your clever algorithms to guess the species of each bird.
     * This skeleton makes no guesses, better safe than sorry!
     */
    std::vector<ESpecies> lGuesses(pState.getNumBirds(),SPECIES_PIGEON);//SPECIES_UNKNOWN);
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
    for(int i=0;i<(int)newModels.size();++i)
        OldModels.push_back(newModels[i]);
    newModels.clear();
    std::cerr << "Old models: " << OldModels.size() << std::endl;
    /*
     * If you made any guesses, you will find out the true species of those birds in this function.
     */
}


} /*namespace ducks*/
