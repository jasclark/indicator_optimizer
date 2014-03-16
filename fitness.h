#include <stdio.h>
#include <cstdlib> 
/* @return - returns total pip gain/loss
 * @param pHigh - array of high prices for each timestep
 * @param pLow - array of low prices for each timestep
 * @param pClose - array of close prices at each timestep
 * @param actions - array of buy, sell, or hold actions at each timestep
 * @param n - length of price and action arrays
 * @param stopProf - number of pips to make on a trade before exiting
 * @param stopLoss - number of pips to lose on a trade before exiting
 */
  double fitness_func(double *pHigh, double *pLow, double *pClose, 
	       	  double *actions, int n, double sProf, double sLoss) {
    bool haveBuy = false,
         haveSell = false;
    int stopProf = (int)(sProf + 0.5);
    int stopLoss = (int)(sLoss + 0.5);
    double pipVal = .0001;
    //pBuy and pSell are prices at which an open buy and sell were executed
    double pBuy = 0, pSell = 0, profit = 0, spread = 2.0;
    int numTrades = 0;

    for (int i = 0; i < n; i++) {
      double pH = pHigh[i],
             pL = pLow[i],
             pC = pClose[i],
             action = actions[i],
             pipGain = 0,
	           pipLoss = 0;
      bool openTrade = (haveBuy || haveSell);
      //if a trade is open, check if exit condition has been met
      //use high/low prices to measure gain/loss because exit happens in real time
      if (openTrade) {  
        if (haveBuy) {
          pipGain = (pH - pBuy)/pipVal;
          pipLoss = (pBuy - pL)/pipVal;
        }else if (haveSell) {
          pipGain = (pSell - pL)/pipVal;
          pipLoss = (pH - pSell)/pipVal;
        }
        //since no real-time data, pessimistically check stop-loss first
        if (pipLoss >= stopLoss) {
          profit -= stopLoss;
          numTrades++;
          haveBuy = haveSell = false;
        }else if (pipGain >= stopProf) {
          profit += stopProf;
	        numTrades++;
          haveBuy = haveSell = false;
        }
      }else {  //no open trade
        if (action > 0) {       //buy condition
          pBuy = pC;            //algorithm runs at close of time step so use pC
          haveBuy = true;                       
        }else if (action < 0) { //sell condition
          pSell = pC; 
          haveSell = true;                          
        }
      }
    }    
    return profit - numTrades * spread;//account for the spread for each trade
  }
