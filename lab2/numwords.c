#include "numwords.h"


const char *words_en =
"zero\0one\0two\0three\0four\0five\0six\0seven\0eight\0nine\0"
"\0\0twenty\0thirty\0forty\0fifty\0sixty\0seventy\0eighty\0ninety\0"
"ten\0eleven\0twelve\0thirteen\0fourteen\0fifteen\0sixteen\0seventeen\0eighteen\0nineteen\0"
"\0thousand\0million\0billion\0trillion\0quadrillion\0quintillion\0sextillion\0septillion\0octillion\0"
"nonillion\0decillion\0hundred\0?\0";
const int pos_en[5][10] = {
    {  0,   5,   9,  13,  19,  24,  29,  33,  39,  45},
    { 50,  51,  52,  59,  66,  72,  78,  84,  92,  99},
    {106, 110, 117, 124, 133, 142, 150, 158, 168, 177},
    {186, 187, 196, 204, 212, 221, 233, 245, 256, 267},
    {277, 287, 305, 305, 305, 305, 305, 305, 305, 305}
};
const char* word_en(int a, int b) {
    return &words_en[pos_en[a][b]];
}


const char *words_ru =
"ноль\0один\0два\0три\0четыре\0пять\0шесть\0семь\0восемь\0девять\0"
"\0\0двадцать\0тридцать\0сорок\0пятьдесят\0шестьдесят\0семьдесят\0восемьдесят\0девяносто\0"
"\0сто\0двести\0триста\0четыреста\0пятьсот\0шестьсот\0семьсот\0восемьсот\0девятьсот\0"
"десять\0одиннадцать\0двенадцать\0тринадцать\0четырнадцать\0пятнадцать\0шестнадцать\0"
                                            "семнадцать\0восемнадцать\0девятнадцать\0"
"\0тысяч\0миллион\0миллиард\0триллион\0квадриллион\0квинтиллион\0секстиллион\0септиллион\0октиллион\0"
"нониллион\0дециллион\0одна\0две\0?\0 ов\0 \0 а\0 и\0";
const int pos_ru[7][10] = {
    {  0,   9,  18,  25,  32,  45,  54,  65,  74,  87},
    {100, 101, 102, 119, 136, 147, 166, 187, 206, 229},
    {248, 249, 256, 269, 282, 301, 316, 333, 348, 367},
    {386, 399, 422, 443, 464, 489, 510, 533, 554, 579},
    {604, 605, 616, 631, 648, 665, 688, 711, 734, 755},
    {774, 793, 828, 828, 828, 828, 828, 828, 828, 828},
    {828, 812, 821, 828, 830, 836, 838, 842, 828, 828}
};

const char* word_ru(int a, int b) {
    return &words_ru[pos_ru[a][b]];
}


static const char* buffadd(const char *part) {
    static char buffer[2048];
    static int offset = 0;

    if (!part) {
        offset = 0;
        buffer[0] = '\0';
        return buffer;
    }
    else if (!*part) return buffer;

    if (part[0] == ' ') ++part;
    else if (offset && buffer[offset-1] != ' ') buffer[offset++] = ' ';

    for (int i = 0; (buffer[offset] = part[i]); ++i, ++offset);

    return buffer;
}


const char* value2en(const char* value) {
    if (!value || !*value) return 0;

    int len = 0; while (value[len]) ++len;
    int exp = (len-1)/3;
    int rank = len-exp*3-1;
    int zeros = 2-rank;

    if (len > 36) return 0;
    buffadd(0);

    for (int i = 0; value[i]; ++i) {

        if (value[i] < '0' || '9' < value[i]) return 0;
        int num = value[i] - '0';

        if (rank == 0) {
            if (i && value[i-1] == '1') buffadd(word_en(2, num));
            else if (num) buffadd(word_en(0, num));
            else ++zeros;
        }
        else if (num) {
            buffadd(word_en(rank%2, num));
            if (rank == 2) buffadd("hundred");
        }
        else ++zeros;

        if (!rank--) {
            if (zeros-3) buffadd(word_en(3, exp));
            zeros = 0;
            rank = 2;
            --exp;
        }
    }

    return buffadd("\0")[0] ? buffadd("\0") : word_en(0, 0);
}


const char* value2ru(const char* value) {
    if (!value || !*value) return 0;

    int len = 0; while (value[len]) ++len;
    int exp = (len-1)/3;
    int rank = len-exp*3-1;
    int zeros = 2-rank;

    if (len > 36) return 0;
    buffadd(0);

    for (int i = 0; value[i]; ++i) {

        if (value[i] < '0' || '9' < value[i]) return 0;
        int num = value[i] - '0';

        if (rank == 0) {
            if (i && value[i-1] == '1') buffadd(word_ru(3, num));
            else if (!num) ++zeros;
            else buffadd(word_ru((exp == 1 && num < 3) ? 6 : 0, num));
        }
        else if (num) buffadd(word_ru(rank, num));
        else ++zeros;

        if (!rank--) {
            if (zeros != 3) {
                buffadd(word_ru(4, exp));
                rank = (((i && value[i-1] == '1') || num >= 5 || !num) ? 0 : 1 + (num!=1)) + (exp==1);
                if (exp) buffadd(word_ru(6, 4+rank));
            }
            zeros = 0;
            rank = 2;
            --exp;
        }
    }

    return buffadd("\0")[0] ? buffadd("\0") : word_ru(0, 0);
}
