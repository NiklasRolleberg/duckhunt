#include "Player.hpp"
#include <cstdlib>
#include <iostream>

namespace ducks
{

HMM Bird01;

Player::Player()
{

    std::cerr <<"A"<<std::endl;
    for(int i=0;i<Bird01.N;++i)
    {
        for(int j=0;j<Bird01.N;++j)
            std::cerr << Bird01.A[i][j] << " ";
        std::cerr << std::endl;
    }

    std::cerr <<"B"<<std::endl;
    for(int i=0;i<Bird01.N;++i)
    {
        for(int j=0;j<Bird01.M;++j)
            std::cerr << Bird01.B[i][j] << " ";
        std::cerr << std::endl;
    }

    std::cerr <<"q"<<std::endl;
    for(int i=0;i<Bird01.N;++i)
        std::cerr << Bird01.q[i] << " ";
    std::cerr<<std::endl;
}

Action Player::shoot(const GameState &pState, const Deadline &pDue)
{
    //std::cerr << "Shoot" << std::endl;
    /*
     * Here you should write your clever algorithms to get the best action.
     * This skeleton never shoots.
     */

    // This line choose not to shoot
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

    std::cerr << "\nGuess" << std::endl;
    std::vector<ESpecies> lGuesses(pState.getNumBirds(), SPECIES_PIGEON);//SPECIES_UNKNOWN);
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
    /*
     * If you made any guesses, you will find out the true species of those birds in this function.
     */
     std::cerr << "REVEAL" << std::endl;
     int n = pState.getBird(0).getSeqLength();
     int seq[n];
     for(int i=0;i<n;++i)
     {
         int temp = pState.getBird(0).getObservation(i);
         if(temp < 0)
         {
            n = i;
            break;
         }
         seq[i] = temp;
     }

    Bird01.BaumWelch(seq,n);

    std::cerr <<"A"<<std::endl;
    for(int i=0;i<Bird01.N;++i)
    {
        for(int j=0;j<Bird01.N;++j)
            std::cerr << Bird01.A[i][j] << " ";
        std::cerr << std::endl;
    }

    std::cerr <<"B"<<std::endl;
    for(int i=0;i<Bird01.N;++i)
    {
        for(int j=0;j<Bird01.M;++j)
            std::cerr << Bird01.B[i][j] << " ";
        std::cerr << std::endl;
    }

    std::cerr <<"q"<<std::endl;
    for(int i=0;i<Bird01.N;++i)
        std::cerr << Bird01.q[i] << " ";
    std::cerr<<std::endl;

    /*
     std::cerr <<"Det var:"<< std::endl;

     for(int i=0;i<(int)pSpecies.size();++i)
     {
        if(pSpecies[i] == SPECIES_PIGEON)
            std::cerr << "det var en PIGEON ";
        else if(pSpecies[i] == SPECIES_RAVEN)
            std::cerr << "det var en RAVEN ";
        else if(pSpecies[i] == SPECIES_SKYLARK)
            std::cerr << "det var en SKYLARK ";
        else if(pSpecies[i] == SPECIES_SWALLOW)
            std::cerr << "det var en SWALLOW ";
        else if(pSpecies[i] == SPECIES_SNIPE)
            std::cerr << "det var en SNIPE ";
        else if(pSpecies[i] == SPECIES_BLACK_STORK)
            std::cerr << "det var en BLACK_STORK ";
        else
            std::cerr << "fel fel fel fel ";

        int N = pState.getBird(0).getSeqLength();
        for(int j=0; j<N;++j)
            std::cerr << pState.getBird(0).getObservation(j) <<" ";
        std::cerr << std::endl;
    }
    */
}


} /*namespace ducks*/
