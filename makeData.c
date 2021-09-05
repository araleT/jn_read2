#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "makeData.h"
#include "mydef.h"

int initial_write = 1; // 0: 出力ファイルに追加で書き込む
int game_begin = 1; // 改行のタイミングをみる
int player_num;


/**
 * 学習には用いない
 * 以下の２つのデータを moveData/ のファイルに書き込む
 * 1. 自分の手札
 * 2. 出したカード
*/
void make_csv(int64 my_cards, int out_cards[8][15])
{
    FILE *fp;
    char *fn_myCards  = "checkData/myCards.csv";
    char *fn_outCards = "checkData/outCards.csv";

    if (initial_write == 1) {
        outputFileReset(fn_myCards);
        outputFileReset(fn_outCards);
    }

    // myCards
    fp = fopen(fn_myCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_myCards); return;
    }

    int my[5][14];
    bit2cards(my, my_cards);
    cardsWrite(fp, my);
    fprintf(fp, "\n");

    fclose(fp);

    // outCards
    fp = fopen(fn_outCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_outCards);
        return;
    }

    int opp[5][14];
    outCards2cards(opp, out_cards);
    cardsWrite(fp, opp);
    fprintf(fp, "\n");

    fclose(fp);

}


/**
 * 指し手の評価モデルの学習に用いる
 * 以下の４つのデータ moveData/ のファイルに書き込む
 * 1. 指した後の自分の残りの手札
 * 2. 指した後の場の状態
 * 3. 相手４人の残りの手札
 * 4. 指し手のモンテカルロ法の評価値
*/
void makeMoveData(int64 myCards, bitValidHandsArray vha, double vha_scores[], const fieldInfo *finfo, int64 oppCards)
{
    FILE *fp;
    char *fn_restCards  = "moveData/restCards.txt";
    char *fn_nextStates = "moveData/nextStates.txt";
    char *fn_oppCards   = "moveData/oppCards.txt";
    char *fn_vhaScores  = "moveData/vhaScores.txt";

    if (initial_write == 1) {
        outputFileReset(fn_restCards);
        outputFileReset(fn_nextStates);
        outputFileReset(fn_oppCards);
        outputFileReset(fn_vhaScores);
    }


    // nextStates
    fp = fopen(fn_nextStates, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_nextStates); return;
    }
    if (initial_write == 0) {
        fprintf(fp, "\n");
    }
    
    fieldInfo simField;
    int array[24], revs[vha.size];
    for (int i = 0; i < vha.size; i++) {
        simField = *finfo;
        simuVH(&vha.hands[i], &simField);
        finfo2states(array, &simField);
        arrayWrite(fp, array, 24);
        fprintf(fp, "\n");
        
        revs[i] = simField.rev;
    }
    fclose(fp);


    // restCards
    fp = fopen(fn_restCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_restCards); return;
    }
    if (initial_write == 0) {
        fprintf(fp, "\n");
    }
    
    int restCards[5][14];
    for (int i = 0; i < vha.size; i++) {
        bit2cards(restCards, myCards ^ vha.hands[i].hands);
        if (revs[i] == 1)
            reverseCards(restCards);
        cardsWrite(fp, restCards);
    }
    fclose(fp);


    // oppCards
    fp = fopen(fn_oppCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_oppCards); return;
    }

    if (initial_write == 0) {
        fprintf(fp, "\n");
    }
    int opp[5][14];
    bit2cards(opp, oppCards);
    for (int i = 0; i < vha.size; i++) {
        if (revs[i] == 1)
            reverseCards(opp);
        cardsWrite(fp, opp);
    }
    fclose(fp);


    // vhaScores
    fp = fopen(fn_vhaScores, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_vhaScores); return;
    }
    
    for (int i = 0; i < vha.size; i++) {
        fprintf(fp, "%lf,", vha_scores[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);

}


/**
 * 推測モデルの学習の入力データに用いる
 * 以下の４つのデータ GuessInputData/ のファイルに書き込む
 * 1. 場に出ていたカード
 * 2. 提出されたカード
 * 3. まだ見えていないカード
 * 4. 場の変化
*/
void makeGuessInputData(const fieldInfo *finfo, int64 submitCards, int64 oppCards, int64 myCards)
{
    FILE *fp;
    char fn_baCard[40], fn_submitCard[40], fn_states[40], fn_oppCards[40];
    sprintf(fn_baCard,     "guessData/P%d_baCard.txt",     player_num);
    sprintf(fn_submitCard, "guessData/P%d_submitCard.txt", player_num);
    sprintf(fn_oppCards,   "guessData/P%d_oppCards.txt",   player_num);
    sprintf(fn_states,     "guessData/P%d_states.txt",     player_num);

    if (initial_write == 1) {
        outputFileReset(fn_baCard);
        outputFileReset(fn_submitCard);
        outputFileReset(fn_oppCards);
        outputFileReset(fn_states);
    }

    // baCard
    fp = fopen(fn_baCard, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_baCard); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }
    
    int array[53];
    finfo2array(array, finfo);
    arrayWrite(fp, array, 53);
    fprintf(fp, "\n");
    fclose(fp);


    // submitCard
    fp = fopen(fn_submitCard, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_submitCard); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }
    
    bit2array(array, submitCards);
    arrayWrite(fp, array, 53);
    fprintf(fp, "\n");
    fclose(fp);


    // oppCards
    fp = fopen(fn_oppCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_oppCards); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }
    
    bit2array(array, oppCards | myCards);
    arrayWrite(fp, array, 53);
    fprintf(fp, "\n");
    fclose(fp);


    // states
    fp = fopen(fn_states, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_states); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }
    
    int fchange[5];
    finfoChange(fp, finfo, submitCards, fchange);
    arrayWrite(fp, fchange, 5);
    fprintf(fp, "\n");
    fclose(fp);
}


/**
 * 推測モデルの学習の正解ラベルに用いる
 * 以下の１つのデータ GuessLabelData/ のファイルに書き込む
 * 1. 推測する対象の残りの手札
*/
void makeGuessLabelData(int64 myCards, int64 outCards)
{
    FILE *fp;
    char fn_restCards[40];
    sprintf(fn_restCards,  "guessData/P%d_restCards.txt",  player_num);

    if (initial_write == 1) {
        outputFileReset(fn_restCards);
    }

    // restCards
    fp = fopen(fn_restCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_restCards); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }
    
    int array[53];
    bit2array(array, myCards ^ outCards);
    arrayWrite(fp, array, 53);
    fprintf(fp, "\n");
    fclose(fp);
}


/**
 * 検証に用いる
 * 以下の３つのデータを checkData/ のファイルに書き込む
 * 1. 自分の手札
 * 2. 場に出ているカード
 * 3. すべての有効手
*/
void makeCheckData(int64 myCards, const fieldInfo *finfo, bitValidHandsArray vha)
{
    FILE *fp;
    char *fn_myCards = "checkData/myCards.txt";
    char *fn_baCards = "checkData/baCards.txt";
    char *fn_vCards  = "checkData/vCards.txt";
    int i, j;

    if (initial_write == 1) {
        outputFileReset(fn_myCards);
        outputFileReset(fn_baCards);
        outputFileReset(fn_vCards);
    }

    // myCards ==============================
    fp = fopen(fn_myCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_myCards); return;
    }

    if (initial_write == 0) {
        fprintf(fp, "\n");
    }
    visualWrite(fp, myCards);
    fclose(fp);

    // baCards ==============================
    fp = fopen(fn_baCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_baCards); return;
    }

    if (initial_write == 0) {
        fprintf(fp, "\n");
    }
    baCardsWrite(fp, finfo);
    fclose(fp);

    // vCards ==============================
    fp = fopen(fn_vCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_vCards); return;
    }

    if (initial_write == 0) {
        fprintf(fp, "\n");
    }
    for (i = 0; i < vha.size; i++) {
        visualWrite(fp, vha.hands[i].hands);
        if (i != vha.size-1) fprintf(fp, ", ");
    }
    if (finfo->onset == 0) fprintf(fp, "OO ");
    fclose(fp);
}




// 出力ファイル名を区別するためにプレイヤー番号を記録する
void setPlayerNum(int pnum)
{
    player_num = pnum;
}


// 書き込むファイルをcleanする
void outputFileReset(char *fname)
{
    FILE *fp;
    fp = fopen(fname, "w");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fname);
        return;
    }
    fclose(fp);
}


// 書き込むファイルのcleanやめる
void initialDataEnd()
{
    initial_write = 0;
}


// gameの初めを表すフラグを立てる
void gameBegin()
{
    game_begin = 1;
}


// gameの初めを表すフラグを０にする
void gameRunning()
{
    game_begin = 0;
}




// 合法手vhを出したときに simFirld を更新する
void simuVH(const bitValidHand *vh, fieldInfo *simField)
{
    if (vh->qty == 0) {
        // simField->onset = 1;
        simField->qty = 0;
        simField->suit = 0;
        simField->ord = -1;
        simField->seq = 0;
		simField->lock = 0;
		// if (checkRev(vh))
		// 	simField->rev = !(simField->rev);
        simField->pass |= 1 << simField->mypos;
        simField->lest[simField->mypos] -= vh->qty;
        // simField->rank = [];
    }
	else if (vh->ord == 6 || (vh->seq == 1 && vh->ord <= 6 && 6 <= vh->ord + vh->qty - 1)) {
        // simField->onset = 1;
        simField->qty = vh->qty;
        simField->suit = 0;
        simField->ord = 14;
        simField->seq = vh->seq;
		simField->lock = 0;
		if ((4 + vh->seq) <= vh->qty)
			simField->rev = !(simField->rev);
        // simField->pass = 0;
        simField->lest[simField->mypos] -= vh->qty;
        // simField->rank = [];
	}
	else {
        // simField->onset = 0;
        simField->qty = vh->qty;
        if (simField->suit == vh->suit)
			simField->lock = 1;
        simField->suit = vh->suit;
        simField->ord = vh->ord;
        simField->seq = vh->seq;
		if ((4 + vh->seq) <= vh->qty)
			simField->rev = !(simField->rev);
        // simField->pass = simField->pass;
        simField->lest[simField->mypos] -= vh->qty;
        // simField->rank = [];
	}
}


// 革命のとき カード配列の強さを反転させる
void reverseCards(int cards[5][14])
{
    int cp[5][14];
    memcpy(cp, cards, sizeof(*cards));
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 13; j++) {
            if (cp[i][j] == 1) {
                cards[i][j] = 0;
                cards[i][(12-j)%13] = 1;
            }
        }
    }
}


// 革命のとき 配列の強さを反転させる
void reverseArray(int array[53])
{
    int cp[53];
    memcpy(cp, array, sizeof(*array));
    
    int suit, ord;
    for (int i = 0; i < 52; i++) {
        if (cp[i] == 1) {
            suit = i/13;
            ord = i%13;
            array[i] = 0;
            array[suit*13 + (12-ord)%13] = 1;
        }
    }
}



// 提出されたカード（プロトコル） を カード配列 に変換する
void outCards2cards(int cards[5][14], int outCards[8][15])
{
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 14; j++) {
            if (outCards[i][j+1] != 0)
                cards[i][j] = 1;
        }
    }
}


// ビットカード を カード配列 に変換する
void bit2cards(int cards[5][14], int64 bitCards)
{
	int i, j;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 14; j++) {
			cards[i][j] = 0;
		}
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 13; j++) {
			if ( ((bitCards >> (i*13+j)) & 1) == 1) {
                cards[i][j] = 1;
            }
		}
	}
	if ( ((bitCards >> 52) & 1) == 1) {
        for (i = 0; i < 5; i++) {
            if (i == 4) {
                for (j = 0; j < 14; j++) {
                    cards[i][j] = 1;
                }
            }
            else {
                cards[i][13] = 1;
            }
        }
    }
}


// ビットカード を 配列 に変換する
void bit2array(int array[53], int64 bitCards)
{
    for (int i = 0; i < 53; i++) {
        array[i] = (bitCards >> i) & 1;
    }
}


// finfo を 状態 に変換する
void finfo2states(int state[24], const fieldInfo *finfo)
{
    state[0] = finfo->qty;
    state[1] = finfo->ord;
    state[2] = finfo->seq;
    state[3] = finfo->lock;
    state[4] = finfo->rev;
    
    for (int i = 0; i < 4; i++) {
        state[5+i] = (finfo->suit >> i) & 1;
    }
    
    int pass[5], rest[5], rank[5], mypos;
    mypos = finfo->mypos;
    for (int i = 0; i < 5; i++) {
        int pos = (finfo->seat[i] - finfo->mypos + 5) % 5;
        pass[pos] = (finfo->pass >> finfo->seat[i]) & 1;
        rest[pos] = finfo->lest[finfo->seat[i]];
        rank[pos] = 4 - finfo->rank[finfo->seat[i]];
    }
    for (int i = 0; i < 5; i++) {
        state[9+i] = pass[i];
        state[14+i] = rest[i];
        state[19+i] = rank[i];
    }
}


// finfo を 配列 に変換する
void finfo2array(int array[53], const fieldInfo *finfo)
{
    for (int i = 0; i < 53; i++)
        array[i] = 0;
    
    if (finfo->onset == 1)
        return;
    
    int ord = finfo->ord;
    if (finfo->seq == 0) {
        for (int i = 0; i < 4; i++) {
            if (((finfo->suit >> i) & 1) == 1)
                array[13*i + ord-1] = 1;
        }
        if (ord == 14 || ord == 0 || finfo->qty == 5)
            array[52] = 1; // joker
    }
    else if (finfo->seq == 1) {
        int suit = 0;
        for (int i = 0; i < 4; i++) {
            if (((finfo->suit >> i) & 1) == 1)
                suit = i;
        }
        for (int j = ord; j < finfo->qty; j++) {
            if (j == 14)
                array[52] = 1; // joker
            else
                array[13*suit + j-1] = 1;
        }
    }
}


// カードを出したときの場の変化を調べる
void finfoChange(FILE *fp, const fieldInfo *finfo, int64 submit, int fchange[5])
{
    for (int i = 0; i < 5; i++)
        fchange[i] = 0;
    
    fchange[0] = finfo->lock;
    fchange[1] = finfo->rev;
    
    int suit = 0, ord = 20, joker = 0, qty = 0, seq = 0;
    for (int i = 0; i < 52; i++) {
        if (((submit >> i) & 1) == 1) {
            suit |= (1 << (int)i/13);
            qty++;
            if (i%13 + 1 > ord)
                seq = 1;
            if ((i%13) + 1 < ord)
                ord = (i%13) + 1;
        }
    }
    if (((submit >> 52) & 1) == 1) { // joker
        joker = 1;
    }
    
    if (ord == 20 && joker == 0) // pass
        return;
        
    if (finfo->onset == 0 && finfo->lock == 0) {
        int samesuit = 0;
        for (int i = 0; i < 4; i++) {
            if (((finfo->suit >> i) & 1) * ((suit >> i) & 1) == 1)
                samesuit++;
        }
        if (finfo->qty == samesuit + joker || finfo->seq * samesuit == 1)
            fchange[2] = 1; // しばりしたか
    }
    if (finfo->rev == 0) {
        if (qty >= 4 + seq)
            fchange[3] = 1; // 革命したか
    }
    if (ord == 6 || (seq == 1 && ord <= 6 && 6 <= ord + qty - 1))
        fchange[4] = 1; // 8切りしたか
}



// カード配列を書き込む
void cardsWrite(FILE *fp, int cards[5][14])
{
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 14; j++) {
            fprintf(fp, "%d,", cards[i][j]);
        }
    }
    fprintf(fp, "\n");
}


// 長さ size の配列 array を書き込む
void arrayWrite(FILE *fp, int array[], int size) 
{
    int i;
    for (i = 0; i < size; i++) {
        fprintf(fp, "%d,", array[i]);
    }
}


// カードBit が表す手役をビジュアルよく書き込む
void visualWrite(FILE *fp, int64 cards)
{
    int i,j;
    for (j = 0; j < 13; j++) {
        for (i = 0; i < 13; i++) {
            if ((cards >> (i*13+j) & 1) == 1) {
                cardWrite(fp, i, j);
            }
        }
    }
    if ((cards >> 52) == 1) {
        fprintf(fp, "JO ");
    }
}


// suit, num のカードをビジュアルよく書き込む
void cardWrite(FILE *fp, int suit, int order)
{
    int mark[13] = {'3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', '1', '2'};
    if (suit == 0) {
        fprintf(fp, "S%c ", mark[order]);
    }
    else if (suit == 1) {
        fprintf(fp, "H%c ", mark[order]);
    }
    else if (suit == 2) {
        fprintf(fp, "D%c ", mark[order]);
    }
    else if (suit == 3) {
        fprintf(fp, "C%c ", mark[order]);
    }
}


// 場に出ているカード と finfo を書き込む
void baCardsWrite(FILE *fp, const fieldInfo *finfo)
{
    if (finfo->onset == 1) {
        fprintf(fp, "OO ");
    }
    int qty = 1, suit, ord = finfo->ord;
    for (suit = 0; suit < 4; suit++) {
        if (((finfo->suit >> suit) & 1) == 1) {
            cardWrite(fp, suit, ord-1);
            qty++;
            if (finfo->seq == 1) ord++;
        }
        if (finfo->qty < qty) break;
    }

    if (finfo->lock == 1 && finfo->rev == 1) fprintf(fp, " [LOCK REV]");
    else if(finfo->lock == 1) fprintf(fp, " [LOCK]    ");
    else if(finfo->rev == 1) fprintf(fp, " [REV]     ");
    else fprintf(fp, " []        ");
    int pass[5], rest[5], rank[5], mypos, i;
    mypos = finfo->mypos;
    for (i = 0; i < 5; i++) {
        int pos = (finfo->seat[i] - finfo->mypos + 5) % 5;
        pass[pos] = (finfo->pass >> finfo->seat[i]) & 1;
        rest[pos] = finfo->lest[finfo->seat[i]];
        rank[pos] = 4 - finfo->rank[finfo->seat[i]];
    }
    // fprintf(fp, " PASS: "); arrayWrite(fp, pass, 5);
    fprintf(fp, " REST: "); arrayWrite(fp, rest, 5);
    // fprintf(fp, " RANK: "); arrayWrite(fp, rank, 5);
}


// debug用　ビットカード を出力する。
void outputCards(int64 bitCards)
{
    FILE *fp;
    char *fn_debugCards = "checkData/dubugCards.txt";
    fp = fopen(fn_debugCards, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_debugCards); return;
    }
    int cards[5][14];
    bit2cards(cards, bitCards);
    cardsWrite(fp, cards);
    fclose(fp);
}

