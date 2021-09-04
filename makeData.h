#ifndef MAKEDATA_H
#define MAKEDATA_H

#include "mydef.h"

void make_csv(int64 my_cards, int out_cards[8][15]);
void makeMoveData(int64 myCards, bitValidHandsArray vha, double vha_scores[], const fieldInfo *finfo, int64 OppCards);
void makeGuessInputData(const fieldInfo *finfo, int64 submitCards, int64 oppCards, int64 myCards);
void makeGuessLabelData(int64 myCards, int64 outCards);
void makeCheckData(int64 myCards, const fieldInfo *finfo, bitValidHandsArray vha);

void setPlayerNum(int pnum);
void outputFileReset(char *fname);
void initialDataEnd();
void gameBegin();
void gameRunning();

void simuVH(const bitValidHand *vh, fieldInfo *simField);
void reverseCards(int cards[5][14]);
void reverseArray(int array[53]);

void outCards2cards(int cards[5][14], int outCards[8][15]);
void bit2cards(int cards[5][14], int64 bitCards);
void bit2array(int array[53], int64 bitCards);
void finfo2states(int state[24], const fieldInfo *finfo);
void finfo2array(int array[53], const fieldInfo *finfo);
void finfoChange(FILE *fp, const fieldInfo *finfo, int64 submit, int fchange[5]);

void cardsWrite(FILE *fp, int array[5][14]);
void arrayWrite(FILE *fp, int array[], int size);
void visualWrite(FILE *fp, int64 cards);
void cardWrite(FILE *fp, int suit, int num);
void baCardsWrite(FILE *fp, const fieldInfo *finfo);
void outputCards(int64 bitCards);

#endif