#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define STATISTICS "statistics.out"
#define EMAILS "data/emails"
#define KEYWORDS "data/keywords"
#define ADD_KEYWORDS "additional_keywords"
#define SPAMMERS "data/spammers"
#define WORDLEN 50
#define MAILLEN 100
#define STRINGLEN 1000

typedef struct {
	char word[WORDLEN];
	float stdev;
	int count;
} kword;

typedef struct {
	kword *words;
	int word_nr;
	int ext_word_nr;
} keywords;

typedef struct {
	int *word_count;
	int recon_words;
	int link_count;
	int size;
	int char_size;
	int has_caps;
	int is_long;
	int spam_score;
	int is_spam;
	float key_score;
	float final_score;
	char mail_nr[20];
} email;

typedef struct {
	email *mail;
	int mail_nr;
	float avg_size;
} emails;

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

void add_keywords(keywords *keyword);

void int_to_char(char *str, int nr);

void search_words(emails *mails, keywords *keyword);

void free_all(emails *mails, keywords *keyword, spammers *spams);

void print_task1(keywords *keyword);

void stdev(emails *mails, keywords *keyword);

float avg_sizef(emails *mails);

void keywords_score(emails *mails, keywords *keyword);

void has_caps(emails *mails);

void save_spammers(spammers *spams);

void check_spammers(emails *mails, spammers *spams);

void is_spamf(emails *mails);

void print_task2(emails *mails);

void is_longf(emails *mails);

void logs(emails *mails, keywords *keyword);
