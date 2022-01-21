#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define EMAILS "data/emails"
#define KEYWORDS "data/keywords"
#define SPAMMERS "data/spammers"
#define WORDLEN 50
#define MAILLEN 100

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
	int size;
	int has_caps;
	int spam_score;
	int is_spam;
	float key_score;
	char mail_nr[20];
} email;

typedef struct {
	char address[MAILLEN];
	int score;
} spammer;

typedef struct {
	spammer *spammers;
	int spam_nr;
} spammers;

int email_nr(void);

void read_keywords(keywords *keyword);

void int_to_char(char *str, int nr);

void search_words(email *emails, keywords *keyword);

void free_all(email *emails, keywords *keyword, spammers *spams);

void print_task1(email *emails, keywords *keyword);

void stdev(email *emails, keywords *keyword);

float avg_sizef(email *emails);

void keywords_score(email *emails, keywords *keyword);

void has_caps(email *emails);

void save_spammers(spammers *spams);

void check_spammers(email *emails, spammers *spams);

void is_spamf(email *emails);

void print_task2(email *emails);
