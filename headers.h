#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define EMAILS "data/emails"
#define KEYWORDS "data/keywords"
#define WORDLEN 30

typedef struct {
    char word[WORDLEN];
    float stdev;
    int count;

} word;

typedef struct {
    word *words;
    int word_nr;
} keywords;

typedef struct {
    int *word_count, n;
    char mail_nr[20];
} email;

int email_nr();

void read_keywords(keywords *);

void int_to_char(char *, int);

void search_words(email *, keywords *);