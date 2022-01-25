#include "headers.h"

int main(void)
{
	// Numarul de mail-uri
	int n = email_nr();

	// Aloc memorie pentru structura de keywords
	keywords *keyword = malloc(sizeof(keywords));

	// Citesc keyword-urile
	read_keywords(keyword);

	add_keywords(keyword);

	// Aloc memorie pentru structura de mail-uri
	emails *mails = malloc(n * sizeof(mails));
	mails->mail = malloc(n * sizeof(email));
	mails->mail_nr = n;

	// Aloc memorie pentru structura de mail-uri de spam
	spammers *spams = malloc(sizeof(spammers));

	// Aloc memorie pentru vectorul de aparitii al cuvintelor din keywords
	// pentru fiecare email si memorez locatia fiecarui mail in
	// emails[i].mail_nr
	for (int i = 0; i < n; i++) {
		mails->mail[i].word_count = calloc(keyword->ext_word_nr, sizeof(int));
		int_to_char(mails->mail[i].mail_nr, i);
	}

	// Functii a caror explicatie o voi da in fisierul functions.c
	search_words(mails, keyword);

	stdev(mails, keyword);

	print_task1(keyword);

	keywords_score(mails, keyword);

	has_caps(mails);

	save_spammers(spams);

	check_spammers(mails, spams);

	is_spamf(mails);

	print_task2(mails);

	free_all(mails, keyword, spams);

	return 0;
}
