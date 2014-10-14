#ifndef _DUCKS_PLAYER_HPP_
#define _DUCKS_PLAYER_HPP_

#include "Deadline.hpp"
#include "GameState.hpp"
#include "Action.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <limits>

namespace ducks
{


struct HMM {
    int birdID = -1;
    int N = 9;
    int M = 9;
    bool converged = true;
    double**A;
    double**B;
    double*q;
    double*qT;

    HMM()
    {
        //std::cerr << "contructor called" << std::endl;
        //A
        A = (double**)calloc(N,sizeof(double*));
        for(int i=0;i<N;++i)
            A[i] = (double*)calloc(N,sizeof(double));
        //B
        B = (double**)calloc(N,sizeof(double*));
        for(int i=0;i<M;++i)
            B[i] = (double*)calloc(M,sizeof(double));
        //q
        q = (double*)calloc(N,sizeof(double));
        //qT q at last timestep
        qT = (double*)calloc(N,sizeof(double));
    }

    void reset()
    {
        converged = true;
        //A
        for(int i=0;i<N;++i)
        {
            double sum = 0;
            for(int j=0;j<N;++j)
            {
                double r = (rand()%100)+1;
                sum+=r;
                A[i][j] = r;
            }
            for(int j=0;j<N;++j)
            {
                A[i][j] = A[i][j]/sum;
            }
        }

        //B
        for(int i=0;i<M;++i)
        {
            double sum = 0;
            for(int j=0;j<N;++j)
            {
                double r = (rand()%100)+1;
                sum+=r;
                B[i][j] = r;
            }
            for(int j=0;j<N;++j)
            {
                B[i][j] = B[i][j]/sum;
            }
        }

        //q
        double sum = 0;
        for(int j=0;j<N;++j)
        {
            double r = (rand()%100)+1;
            sum+=r;
            q[j] = r;
        }
        for(int j=0;j<N;++j)
        {
            q[j] = q[j]/sum;
        }
    }

    double** initialize(int rows, int cols)
    {
        double** temp;
        temp = (double**)calloc(rows , sizeof(double *));
        for(int i=0 ; i< rows ; ++i)
            temp[i] = (double*)calloc(cols , sizeof(double));
        return temp;
    }

    std::vector<int> Viterbi(std::vector<int> seq)
    {
        int state;

        int T = (int)seq.size();
        for(int t=0;t<(int)seq.size();++t)
            if(seq[t] == -1)
            {
                T = t;
                break;
            }

        double maximum = -std::numeric_limits<double>::max();

        double* tempArray = (double*)calloc(N,sizeof(double));
        double* prob = (double*)calloc(N,sizeof(double));
        int sequences[T][N];

        //t=0;
        for(int i=0;i<N;++i)
        {
            prob[i] = q[i]*B[i][seq[0]];
            int index = 0;
            if(prob[i]> maximum)
            {
                maximum = prob[i];
                index = i;
            }
            sequences[0][i] = index;
        }

        for(int t=1;t<T;++t)
        {
            state = seq[t];
            for(int i=0;i<N;++i)
            {
                maximum = prob[0] * A[0][i]* B[i][state];
                int index = 0;
                for(int k=1;k<N;++k)
                {
                    double te = prob[k] * A[k][i] * B[i][state];
                    if(te > maximum)
                    {
                        maximum = te;
                        index = k;
                    }
                }
                //std::cerr << index <<"  "<< maximum << std::endl;
                tempArray[i] = maximum;
                sequences[t][i] = index;
            }

            for(int i=0;i<N;++i)
                prob[i] = tempArray[i];
        }

        int current = 0;
        maximum = prob[0];
        for(int i=1;i<N;++i)
        {
            if(prob[i]>maximum)
            {
                maximum = prob[i];
                current = i;
            }
        }

        std::vector<int> e(T);
        for(int t = (T-1);t>=0 ;t--)
        {
            e[t] = current;
            current = sequences[t][current];
        }
        return e;
    }

    std::vector<double> probability(int lastState)
    {
        //alpha-0
        //double c;
        std::vector<double> temp(N,0);
        for(int i=0;i<N;++i)
        {

            for(int j=0;j<N;++j)
            {
                temp[i] += qT[j]*A[j][i];
            }
            temp[i] *= B[i][lastState];

            //temp[i] = qT[i]*B[i][lastState];
            //c+=alpha[0][i];
        }
        //c=1./c;
        //for(int i=0;i<N;++i)
            //alpha[0][i] *=c;
        return temp;
    }

    void BaumWelch(std::vector<int> seq)
    {
        //std::cerr << "BAUM" << std::endl;
        int T = (int)seq.size();
        for(int t=0;t<(int)seq.size();++t)
            if(seq[t] == -1)
            {
                T = t;
                break;
            }
        //std::cerr << "seq.size():  "<< seq.size() << " T=" << T << " seq[T-1]=" << seq[T-1] <<std::endl;
        /*
        std::cerr << "BaumWelch \nT = " << T << std::endl;
        std::cerr << "seq: ";
        for(int t=0;t<T;++t)
            std::cerr << seq[t] << " ";
        std::cerr << std::endl;

        std::cerr << "\nA"<<std::endl;
        for(int i=0;i<N;++i)
        {
            for(int j=0;j<N;++j)
                std::cerr << A[i][j] << " ";
            std::cerr << std::endl;
        }

        std::cerr << "\nB"<<std::endl;
        for(int i=0;i<N;++i)
        {
            for(int j=0;j<N;++j)
                std::cerr << B[i][j] << " ";
            std::cerr << std::endl;
        }
        */
        int digits = 5;
        int MaxItter = 200;
        int itter = 0;
        double OldLogProb = -std::numeric_limits<double>::max();

        //variabler som går att återanvända
        double* c = (double*) calloc(T,sizeof(double)); //c[time]
        double** alpha = initialize(T, N);  //alpha[time][index]
        double** beta = initialize(T, N);  //beta[time][index]
        double** Gamma = initialize(T,N);
        double*** diGamma = (double***)calloc(T,sizeof(double**)); //diGamma[time][index1][index2]
        for(int t=0;t<T;++t)
            diGamma[t] = initialize(N,N);

        bool GO = true;
        while((itter < MaxItter) && GO)
        {

            /**----------Alpha-pass------------------------*/
            //std::cerr << "alpha-pass" << std::endl;
            //alpha-0
            c[0] = 0;

            for(int i=0;i<N;++i)
            {
                alpha[0][i] = q[i]*B[i][seq[0]];
                c[0] += alpha[0][i];
            }
            //scale
            c[0] = 1.0/c[0];
            for(int i=0;i<N;++i)
            {
                alpha[0][i] *= c[0];
            }

            //alpha-t
            for(int t=1;t<T;++t)
            {
                c[t] = 0;
                for(int i=0;i<N;++i)
                {
                    alpha[t][i] = 0;
                    for(int j=0;j<N;++j)
                    {
                        alpha[t][i] += alpha[t-1][j]*A[j][i];
                    }
                    alpha[t][i] *= B[i][seq[t]];
                    c[t] += alpha[t][i];
                }

                //scale
                c[t] = 1.0/c[t];
                for(int i=0;i<N;++i)
                {
                    alpha[t][i] *= c[t];
                }
            }

            /**----------Beta-pass------------------------*/
            //std::cerr << "Beta-pass" << std::endl;
            //beta T-1
            for(int i=0;i<N;++i)
                beta[T-1][i] = c[T-1];

            for(int t=T-2; t>=0;--t)
            {
                for(int i=0;i<N;++i)
                {
                    beta[t][i] = 0;
                    for(int j=0;j<N;++j)
                    {
                        beta[t][i] += A[i][j] * B[j][seq[t+1]] * beta[t+1][j];
                    }
                    //scale
                    beta[t][i] *= c[t];
                }
            }


            /**----------diGamma & Gamma----------------*/
            //std::cerr << "diGamma&Gamma" << std::endl;
            for(int t=0;t<(T-1);++t)
            {
                double denom = 0;
                for(int i=0;i<N;++i)
                    for(int j=0;j<N;++j)
                        denom += alpha[t][i] * A[i][j] * B[j][seq[t+1]] * beta[t+1][j];

                for(int i=0;i<N;++i)
                {
                    Gamma[t][i] = 0;
                    for(int j=0;j<N;++j)
                    {
                        diGamma[t][i][j] = (alpha[t][i] * A[i][j] * B[j][seq[t+1]] * beta[t+1][j]) / denom;
                        Gamma[t][i] += diGamma[t][i][j];
                    }
                }
            }

            /**----------Estimate A,b,q------------------*/

            /*q*/
            for(int i=0;i<N;++i)
            {
                q[i] = Gamma[0][i];
                qT[i] = alpha[T-1][i];
            }

            /*A*/
            for(int i=0;i<N;++i)
            {
                for(int j=0;j<N;++j)
                {
                    double numer = 0;
                    double denom = 0;
                    for(int t=0;t<(T-1);++t)
                    {
                        numer += diGamma[t][i][j];
                        denom += Gamma[t][i];
                    }
                //A[i][j] = numer/denom;
                A[i][j] = round((numer/denom)*pow(10,digits))/pow(10,digits);
                //std::cerr << "A : numer=" << numer <<", denom=" << denom << std::endl;
                }
            }

            /*B*/
            for(int i=0;i<N;++i)
            {
                for(int j=0;j<M;++j)
                {
                    double numer = 0;
                    double denom = 0;
                    for(int t=0;t<(T-1);++t)
                    {
                        if(seq[t] == j)
                            numer += Gamma[t][i];
                        denom += Gamma[t][i];
                    }
                //B[i][j] = numer/denom;
                B[i][j] = round((numer/denom)*pow(10,digits))/pow(10,digits);
                //std::cerr << "B : numer=" << numer <<", denom=" << denom << std::endl;
                }
            }

            double logProb = 0;
            for(int t=0;t<T;++t)
                logProb += log(c[t]);
            logProb *= -1;

            //std::cerr << "\nlogProb: " << logProb  << "\nOldLogProb: " << OldLogProb <<std::endl;

            if(logProb <= OldLogProb)
            {
                GO = false;
                //std::cerr << "Convergerad!, itterationer: "<< itter << std::endl;
            }

            OldLogProb = logProb;
            itter++;
        }

        //std::cerr << "BaumWelch Done Itter: "<< itter << std::endl;

        if (itter==MaxItter)
            converged = false;
        /*
        if(itter >150)
        {
            std::cerr << "A" << std::endl;
            for(int i=0;i<N;++i)
            {
                for(int j=0;j<N;++j)
                    std::cerr << A[i][j] << " ";
                std::cerr << std::endl;
            }
            std::cerr << "B" <<std::endl;
            for(int i=0;i<N;++i)
            {
                for(int j=0;j<N;++j)
                    std::cerr<< B[i][j] << " ";
                std::cerr << std::endl;
            }
            std::cerr <<"q"<< std::endl;
            for(int i=0;i<N;++i)
                std::cerr << q[i] << " ";
            std::cerr << std::endl;
        }*/
        //std::cerr << "Helt klar"<<std::endl;
    }
};

class Player
{
public:

    /**
     * Constructor
     * There is no data in the beginning, so not much should be done here.
     */
    Player();

    /**
     * Shoot!
     *
     * This is the function where you start your work.
     *
     * You will receive a variable pState, which contains information about all
     * birds, both dead and alive. Each birds contains all past actions.
     *
     * The state also contains the scores for all players and the number of
     * time steps elapsed since the last time this function was called.
     *
     * @param pState the GameState object with observations etc
     * @param pDue time before which we must have returned
     * @return the prediction of a bird we want to shoot at, or cDontShoot to pass
     */
    Action shoot(const GameState &pState, const Deadline &pDue);

    /**
     * Guess the species!
     * This function will be called at the end of each round, to give you
     * a chance to identify the species of the birds for extra points.
     *
     * Fill the vector with guesses for the all birds.
     * Use SPECIES_UNKNOWN to avoid guessing.
     *
     * @param pState the GameState object with observations etc
     * @param pDue time before which we must have returned
     * @return a vector with guesses for all the birds
     */
    std::vector<ESpecies> guess(const GameState &pState, const Deadline &pDue);

    /**
     * If you hit the bird you were trying to shoot, you will be notified
     * through this function.
     *
     * @param pState the GameState object with observations etc
     * @param pBird the bird you hit
     * @param pDue time before which we must have returned
     */
    void hit(const GameState &pState, int pBird, const Deadline &pDue);

    /**
     * If you made any guesses, you will find out the true species of those
     * birds through this function.
     *
     * @param pState the GameState object with observations etc
     * @param pSpecies the vector with species
     * @param pDue time before which we must have returned
     */
    void reveal(const GameState &pState, const std::vector<ESpecies> &pSpecies, const Deadline &pDue);
};
} /*namespace ducks*/

#endif
