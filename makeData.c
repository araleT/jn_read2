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
    // char fn_restCards[30], fn_nextStates[30], fn_oppCards[30], fn_vhaScores[30];
    // sprintf(fn_restCards,  "moveData/P%d_restCards.txt",  player_num);
    // sprintf(fn_nextStates, "moveData/P%d_nextStates.txt", player_num);
    // sprintf(fn_oppCards,   "moveData/P%d_oppCards.txt",   player_num);
    // sprintf(fn_vhaScores,  "moveData/P%d_vhaScores.txt",  player_num);
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
        cardsWrite(fp, restCards);
    }
    fclose(fp);


    // nextStates
    fp = fopen(fn_nextStates, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_nextStates); return;
    }

    if (initial_write == 0) {
        fprintf(fp, "\n");
    }
    fieldInfo simField;
    for (int i = 0; i < vha.size; i++) {
        simField = *finfo;
        simuVH(&vha.hands[i], &simField);
        finfoWrite(fp, &simField);
        fprintf(fp, "\n");
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
 * 1. 場に出ていた手役
 * 2. 提出された手役
 * 3. 場の状態、変化
 * 4. 見えていない残りのカードで作られる手役構成
*/
void makeGuessInputData(const fieldInfo *finfo, int64 submitCards, int64 oppCards, int64 myCards)
{
    FILE *fp;
    char fn_baHand[40], fn_submitHand[40], fn_states[40], fn_oppHands[40];
    sprintf(fn_baHand,     "guessInputData/P%d_baHand.txt",     player_num);
    sprintf(fn_submitHand, "guessInputData/P%d_submitHand.txt", player_num);
    sprintf(fn_states,     "guessInputData/P%d_states.txt",     player_num);
    sprintf(fn_oppHands,   "guessInputData/P%d_oppHands.txt",   player_num);

    if (initial_write == 1) {
        outputFileReset(fn_baHand);
        outputFileReset(fn_submitHand);
        outputFileReset(fn_states);
        outputFileReset(fn_oppHands);
    }

    // baHands
    fp = fopen(fn_baHand, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_baHand); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }

    int ba_hand[51];
    finfo2hands(ba_hand, finfo);
    arrayWrite(fp, ba_hand, 51);
    fprintf(fp, "\n");
    fclose(fp);

    // submitHand
    fp = fopen(fn_submitHand, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_submitHand); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }

    int sub[5][14], submitHand[51];
    bit2cards(sub, submitCards);
    cards2hand1(submitHand, sub);
    arrayWrite(fp, submitHand, 51);
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
    
    // 場の状態
    finfoWrite(fp, finfo);
    // 提出カードによって場の状態の変化
    int ocinfo[4];
    outCardsInfo(ocinfo, sub);
    finfoChangedWrite(fp, finfo, ocinfo);
    fprintf(fp, "\n");
    fclose(fp);

    // oppHands
    fp = fopen(fn_oppHands, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_oppHands); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }

    int opp[5][14], oppHands[51];
    bit2cards(opp, oppCards | myCards);
    cards2hands(oppHands, opp);
    arrayWrite(fp, oppHands, 51);
    fprintf(fp, "\n");
    fclose(fp);
}


/**
 * 推測モデルの学習の正解ラベルに用いる
 * 以下の１つのデータ GuessLabelData/ のファイルに書き込む
 * 1. 推測する対象の残りの手札 で作られる 手役構成
*/
void makeGuessLabelData(int64 myCards, int64 outCards)
{
    FILE *fp;
    char fn_restHands[40];
    sprintf(fn_restHands,  "guessLabelData/P%d_restHands.txt",  player_num);

    if (initial_write == 1) {
        outputFileReset(fn_restHands);
    }

    // restHands
    fp = fopen(fn_restHands, "a");
    if (fp == NULL) {
        printf("%sファイルが開けません\n", fn_restHands); return;
    }
    if (game_begin == 1 && initial_write != 1) {
        fprintf(fp, "\n");
    }

    int restCards[5][14], restHands[51];
    bit2cards(restCards, myCards ^ outCards);
    cards2hands(restHands, restCards);
    arrayWrite(fp, restHands, 51);
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
    fprintf(fp, "");
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
        simField->ord = 15;
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


// 提出するカードの情報（枚数、階段か、スート、オーダー）を取得する
void outCardsInfo(int info[4], int cards[5][14])
{
    int qty = 0, seq = 0, suit = 0, ord = 0;
    for (int j = 0; j < 13; j++) {
        for (int i = 0; i < 5; i++) {
            if (cards[i][j] == 1) {
                qty++;
                suit += (1 << i);
                ord = j + 1;
                for (int k = j + 1; k < 14; k++) {
                    if (cards[i][k] == 1) {
                        seq = 1; qty++;
                    }
                }
            }
        }
        if (qty != 0) break;
    }
    info[0] = qty, info[1] = seq, info[2] = suit, info[3] = ord;
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


// カード配列 を 手役構成(重複あり) に変換する
void cards2hands(int hands[51], int cards[5][14])
{
    int SOLO = 0, MUL2 = 14, MUL3 = 27, SEQ = 40;

    for (int i = 0; i < 51; i++) {
        hands[i] = 0;
    }

    for (int j = 0; j < 13; j++) {
        int num = 0;
        for (int i = 0; i < 4; i++) {
            num += cards[i][j];
            if (j < 11 && (cards[i][j] * cards[i][j+1] * cards[i][j+2] == 1))
                hands[SEQ + j] = 1;
        }
        if (num >= 1) hands[SOLO + j] = 1;
        if (num >= 2) hands[MUL2 + j] = 1;
        if (num >= 3) hands[MUL3 + j] = 1;
    }
    if (cards[4][13] == 1) hands[SOLO + 13] = 1; // joker
}


// カード配列 を 手役構成(1つ) に変換する
void cards2hand1(int hands[51], int cards[5][14])
{
    int i, j;
    int SOLO = 0, MUL2 = 14, MUL3 = 27, SEQ = 40;

    for (i = 0; i < 51; i++) {
        hands[i] = 0;
    }

    for (j = 0; j < 14; j++) {
        int num = 0, seq = 0;
        for (i = 0; i < 5; i++) {
            num += cards[i][j];
            if (j < 12 && cards[i][j] == 1) {
                seq = cards[i][j] + cards[i][j+1] + cards[i][j+2];
                if (seq >= 3) break;
            }
        }
        if      (num == 0) continue;
        if      (seq >= 3) hands[SEQ  + j] = 1;
        else if (num == 1) hands[SOLO + j] = 1;
        else if (num == 2) hands[MUL2 + j] = 1;
        else if (num >= 3) hands[MUL3 + j] = 1;
        break;
    }
}


// vha を 手役構成 に変換する
void vha2hands(int hands[51], bitValidHand *vha)
{
    int SOLO = 0, MUL2 = 14, MUL3 = 27, SEQ = 40;

    for (int i = 0; i < 51; i++) {
        hands[i] = 0;
    }

    if (vha->qty == 1) {
        hands[SOLO + vha->ord - 1] = 1;
    }
    else if (vha->qty == 2) {
        hands[MUL2 + vha->ord - 1] = 1;
    }
    else if (vha->qty >= 3) {
        if (vha->seq == 1)
            hands[SEQ + vha->ord - 1] = 1;
        else
            hands[MUL3 + vha->ord - 1] = 1;
    }

    if (((vha->hands >> 52) & 1) == 1) {
        hands[SOLO + 13] = 1; // joker
    }
}


// finfo を 手役構成に変換する
void finfo2hands(int hands[51], const fieldInfo *finfo)
{
    int SOLO = 0, MUL2 = 14, MUL3 = 27, SEQ = 40;

    for (int i = 0; i < 51; i++) {
        hands[i] = 0;
    }

    if (finfo->onset == 1) return;
    if (finfo->qty == 1) {
        hands[SOLO + finfo->ord - 1] = 1;
    }
    else if (finfo->qty == 2) {
        hands[MUL2 + finfo->ord - 1] = 1;
    }
    else if (finfo->qty >= 3) {
        if (finfo->seq == 1)
            hands[SEQ + finfo->ord - 1] = 1;
        else
            hands[MUL3 + finfo->ord - 1] = 1;
    }
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


// finfo を書き込む
void finfoWrite(FILE *fp, const fieldInfo *finfo)
{
    fprintf(fp, "%d,%d,", finfo->qty, finfo->ord);
    fprintf(fp, "%d,%d,%d,", finfo->seq, finfo->lock, finfo->rev);
    int suit[4];
    for (int i = 0; i < 4; i++) {
        suit[i] = (finfo->suit >> i) & 1;
    }
    arrayWrite(fp, suit, 4);
    int pass[5], rest[5], rank[5], mypos;
    mypos = finfo->mypos;
    for (int i = 0; i < 5; i++) {
        int pos = (finfo->seat[i] - finfo->mypos + 5) % 5;
        pass[pos] = (finfo->pass >> finfo->seat[i]) & 1;
        rest[pos] = finfo->lest[finfo->seat[i]];
        rank[pos] = 4 - finfo->rank[finfo->seat[i]];
    }
    arrayWrite(fp, pass, 5);
    arrayWrite(fp, rest, 5);
    arrayWrite(fp, rank, 5);
}


// カードを出したときの場の変化 changedfinfo を書き込む
void finfoChangedWrite(FILE *fp, const fieldInfo *finfo, int ocinfo[4])
{
    int changed[3] = {0, 0, 0};
    int qty = ocinfo[0], seq = ocinfo[1], suit = ocinfo[2], ord = ocinfo[3];
    if (finfo->onset == 0 && finfo->lock == 0) {
        int samesuit = 0;
        for (int i = 0; i < 4; i++) {
            if (((finfo->suit >> i) & 1) * ((suit >> i) & 1) == 1)
                samesuit++;
        }
        if (finfo->qty == samesuit || finfo->seq * samesuit == 1)
            changed[0] = 1;
    }
    if (finfo->rev == 0) {
        if (qty >= 4 + seq)
            changed[1] = 1;
    }
    if (ord == 6 || (seq == 1 && ord <= 6 && 6 <= ord + qty - 1))
        changed[2] = 1;

    arrayWrite(fp, changed, 3);
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

