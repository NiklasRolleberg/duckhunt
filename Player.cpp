#include "Player.hpp"
#include <cstdlib>
#include <iostream>

namespace ducks
{

HMM BirdModel[6];

Player::Player()
{
    std::cerr << "Start:" << std::endl;
    std::cerr <<"A"<<std::endl;
    for(int i=0;i<BirdModel[0].N;++i)
    {
        for(int j=0;j<BirdModel[0].N;++j)
            std::cerr << BirdModel[0].A[i][j] << " ";
        std::cerr << std::endl;
    }

    std::cerr <<"B"<<std::endl;
    for(int i=0;i<BirdModel[0].N;++i)
    {
        for(int j=0;j<BirdModel[0].M;++j)
            std::cerr << BirdModel[0].B[i][j] << " ";
        std::cerr << std::endl;
    }

    std::cerr <<"q"<<std::endl;
    for(int i=0;i<BirdModel[0].N;++i)
        std::cerr << BirdModel[0].q[i] << " ";
    std::cerr<<std::endl;
}

Action Player::shoot(const GameState &pState, const Deadline &pDue)
{
    /**TEST*/

    HMM test;
    test.A = test.initialize(4,4);
    test.B = test.initialize(4,4);
    test.q = (double*)calloc(4,sizeof(double));
    test.N = 4;
    test.M = 4;
















    /**</TEST>*/
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

    std::vector<ESpecies> lGuesses(pState.getNumBirds(),SPECIES_PIGEON);//SPECIES_UNKNOWN);
    std::cerr << "\nGuess" << std::endl;

    //std::cerr << "\Birds: " << lGuesses.size() << std::endl;
    /*
    for(int i=0;i<(int)pState.getNumBirds();++i)
    {
        int N = pState.getBird(i).getSeqLength();
        std::vector<int> seq;
        for(int j=0;j<N;++j)
        {
            seq.push_back(pState.getBird(i).getObservation(j));
        }
        int index = -1;
        double maximum = -std::numeric_limits<double>::max();;
        for(int bird=0;bird<6;++bird)
        {
            if(BirdModel[bird].done)
            {
                double prob = BirdModel[bird].probability(seq);
                if(prob > maximum)
                {
                    index = bird;
                    maximum = prob;
                }
            }
        }

        switch (index)
        {
        case 0: lGuesses[i]=SPECIES_PIGEON;
            break;
        case 1: lGuesses[i]=SPECIES_RAVEN;
            break;
        case 3: lGuesses[i]=SPECIES_SKYLARK;
            break;
        case 4: lGuesses[i]=SPECIES_SNIPE;
            break;
        case 5: lGuesses[i]=SPECIES_BLACK_STORK;
            break;
        default: lGuesses[i] = SPECIES_PIGEON; //SPECIES_UNKNOWN;
            break;
        }
        std::cerr << "det är nog en: " << index << " med " << maximum << " sannolikhet"<< std::endl;
        //break;
    }
    std::cerr << "guess done" << std::endl;*/
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

     for(int i=0;i<(int)pSpecies.size();++i)
     {
         std::cerr << "Det var en " << pSpecies[i] << std::endl;
         /*
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
        */
         int N = pState.getBird(i).getSeqLength();
         int bird = (int)pSpecies[i];
         std::vector<int> seq;
         for(int j=0;j<N;++j)
         {
             seq.push_back(pState.getBird(i).getObservation(j));
         }
        /*
        for(int j=0;j<(int)seq.size();++j)
            std::cerr << seq[j] << " ";
        std::cerr << std::endl;
        */
        //std::cerr << "\nBird " << bird << std::endl;
        //BirdModel[bird].BaumWelch(seq);

        if(bird==0)
        {
            BirdModel[bird].BaumWelch(seq);
            std::cerr << "Seq: \n";
            for(int i=0;i<(int)seq.size();++i)
                std::cerr << seq[i] << " ";

            std::cerr <<"\nProbability of seq: " << BirdModel[bird].probability(seq);

            //seq.push_back(0);
            std::vector<int> ny = BirdModel[bird].Viterbi(seq);
            std::cerr << "\nViterbi: \n";
            for(int i=0;i<(int)ny.size();++i)
                std::cerr << ny[i] << " ";
            std::cerr <<"\nProbability for Viterbi seq: " << BirdModel[bird].probability(ny) << std::endl;

            exit(0); //<--------------------------------------------SKA BORT!
        }

        /*
        if(i==6)
        {
            BirdModel[6].BaumWelch(seq);
            std::cerr <<"A"<<std::endl;
            for(int i=0;i<BirdModel[0].N;++i)
            {
                for(int j=0;j<BirdModel[0].N;++j)
                    std::cerr << BirdModel[0].A[i][j] << " ";
                std::cerr << std::endl;
            }

            std::cerr <<"B"<<std::endl;
            for(int i=0;i<BirdModel[0].N;++i)
            {
                for(int j=0;j<BirdModel[0].M;++j)
                    std::cerr << BirdModel[0].B[i][j] << " ";
                std::cerr << std::endl;
            }

            std::cerr <<"q"<<std::endl;
            for(int i=0;i<BirdModel[0].N;++i)
                std::cerr << BirdModel[0].q[i] << " ";
            std::cerr<<std::endl;

        }*/
     }
}


} /*namespace ducks*/
