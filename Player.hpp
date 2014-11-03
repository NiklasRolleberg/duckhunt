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

struct data {
    Bird bird;
    ESpecies birdID;

    data(Bird b, ESpecies id)
    {
        bird = b;
        birdID = id;
    }
};


struct HMM {
    ESpecies birdID = SPECIES_UNKNOWN;
    const int N = 9;
    const int M = 9;
    //int MaxItter = 400;
    double**A;
    double**B;
    double*q;

    bool Converged = false;
    int tTrain = 0;

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

            /*for(int j=0;j<N;++j)
            {
                if(i==j)
                    A[i][j] = 0.6;
                else
                    A[i][j] = 0;
            }*/
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

    void scale()
    {
        double sum = 0;
        for(int i=0;i<N;++i)
            sum += q[i];
        for(int i=0;i<N;++i)
            q[i] = q[i]/sum;
    }

    void reset()
    {
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

    EMovement nextMove(Bird b)
    {
        //find most probable hidden state
        int T = b.getSeqLength();
        int currState = 0;
        float maxGamma = 0;
        float denom = 0;

        double**alpha = initialize(T,N);
        double**beta = initialize(T,N);
        double* c = (double*)calloc(N,sizeof(double));
        double maximum = 0;

        alphaPass(alpha,c,T,b,true);
        betaPass(beta,c,T,b,true);


        for(int j = 0; j < N; ++ j)
        {
            denom += alpha[T - 1][j];
        }

        for(int i = 0; i < N; ++ i)
        {
            float res = alpha[T-1][i] * beta[T-1][i] / denom;
            if(res > maxGamma)
            {
                maxGamma = res;
                currState = i;
            }
        }

        //find the observation for that state
        int index = -1;

        std::cout << "currentState: " << currState << std::endl;

        for(int t = 0; t < M; ++t)
        {
            if(B[currState][t] > maximum)
            {
                maximum = B[currState][t];
                index = t;
            }
        }

        std::cerr << "max: " << maximum << std::endl;

        if(index == -1 || maximum < 0.95)
            return MOVE_DEAD;


        switch(index)
        {
            case 0: return MOVE_UP_LEFT;
            break;case 1: return MOVE_UP;
            break;case 2: return MOVE_UP_RIGHT;
            break;case 3: return MOVE_LEFT;
            break;case 4: return MOVE_STOPPED;
            break;case 5: return MOVE_RIGHT;
            break;case 6: return MOVE_DOWN_LEFT;
            break;case 7: return MOVE_DOWN;
            break;case 8: return MOVE_DOWN_RIGHT;
            break;
            default: return MOVE_DEAD;
        }
        std::cerr << "Fel i nextMove" << std::endl;
        return MOVE_DEAD;
    }

    double probability(Bird b)
    {
        int T = b.getSeqLength()-1;
        if(b.isDead())
        {
            for(int i=0;i<(int)b.getSeqLength();++i)
                if(b.getObservation(i) == -1)
                {
                    T = i;
                    break;
                }
        }

        //if(T>tTrain)
            //T = tTrain;

        std::cerr << "T= " << T << std::endl;

        double** alpha = initialize(T,N);
        double* c = (double*)calloc(N,sizeof(double));

        double sum = 0;
        alphaPass(alpha,c,T,b,true);

        for(int i = 0; i < N; ++ i)
        {
            std::cerr << i << " ";
            sum += (alpha[T-1][i]);
        }
        std::cerr << std::endl;
        return sum;
    }

    double** initialize(int rows, int cols)
    {
        double** temp;
        temp = (double**)calloc(rows , sizeof(double *));
        for(int i=0 ; i< rows ; ++i)
            temp[i] = (double*)calloc(cols , sizeof(double));
        return temp;
    }

    void alphaPass(double** alpha, double* c, int T, Bird &b, bool sc)
    {
        /**----------Alpha-pass------------------------*/
        //std::cerr << "alpha-pass" << std::endl;
        //alpha-0
        c[0] = 0;

        for(int i=0;i<N;++i)
        {
            alpha[0][i] = q[i]*B[i][b.getObservation(0)];
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
                alpha[t][i] *= B[i][b.getObservation(t)];
                c[t] += alpha[t][i];
            }

            //scale
            if(sc)
            {
                c[t] = 1.0/c[t];
                for(int i=0;i<N;++i)
                {
                    alpha[t][i] *= c[t];
                }
            }
        }

    }

    void betaPass(double** beta,double* c,int T, Bird &b, bool scale)
    {
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
                    beta[t][i] += A[i][j] * B[j][b.getObservation(t+1)] * beta[t+1][j];
                }
                //scale
                if(scale)
                    beta[t][i] *= c[t];
            }
        }
    }

    void BaumWelch(Bird b,int MaxItter)
    {
        //std::cerr << "BAUM" << std::endl;
        int T = b.getSeqLength();
        if(b.isDead())
            for(int t=0;t<(int)b.getSeqLength();++t)
                if(b.getObservation(t) == -1)
                {
                    T = t;
                    break;
                }

        tTrain = T;
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

            alphaPass(alpha,c,T,b,true);
            betaPass(beta,c,T,b,true);

            /**----------diGamma & Gamma----------------*/
            //std::cerr << "diGamma&Gamma" << std::endl;
            for(int t=0;t<(T-1);++t)
            {
                double denom = 0;
                for(int i=0;i<N;++i)
                    for(int j=0;j<N;++j)
                        denom += alpha[t][i] * A[i][j] * B[j][b.getObservation(t+1)] * beta[t+1][j];

                for(int i=0;i<N;++i)
                {
                    Gamma[t][i] = 0;
                    for(int j=0;j<N;++j)
                    {
                        diGamma[t][i][j] = (alpha[t][i] * A[i][j] * B[j][b.getObservation(t+1)] * beta[t+1][j]) / denom;
                        Gamma[t][i] += diGamma[t][i][j];
                    }
                }
            }

            /**----------Estimate A,b,q------------------*/

            /*q*/
            for(int i=0;i<N;++i)
            {
                q[i] = Gamma[0][i];
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
                A[i][j] = numer/denom;
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
                        if(b.getObservation(t) == j)
                            numer += Gamma[t][i];
                        denom += Gamma[t][i];
                    }
                B[i][j] = numer/denom;
                }
            }

            double logProb = 0;
            for(int t=0;t<T;++t)
                logProb += log(c[t]);
            logProb *= -1;

            if(logProb <= OldLogProb)
            {
                GO = false;
            }

            OldLogProb = logProb;
            itter++;
        }

        bool batman = false;
        for(int i=0;i<N && !batman;++i)
            for(int j=0;j<N && !batman;++j)
                if(std::isnan(A[i][j]))
                   batman = true;
        for(int i=0;i<N && !batman;++i)
            for(int j=0;j<M && !batman;++j)
                if(std::isnan(B[i][j]))
                   batman = true;
        if(batman)
            std::cerr << "NaN" << std::endl;


        Converged = true;
        if(itter == MaxItter || batman)
            Converged = false;


        //std::cerr << "BaumWelch Done Itter: "<< itter << std::endl;
        /*
        if(itter == MaxItter || true)
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

    ESpecies IDbird(Bird bird);

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
