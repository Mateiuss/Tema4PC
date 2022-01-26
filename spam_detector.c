#include "headers.h"

int main(void)
{
	FILE *fp = fopen("logs.txt", "w");

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

	for (int i = 0; i < keyword->ext_word_nr; i++) {
		if (i == 0)
			fprintf(fp, "-----Keywords----------------\n");
		else if (i == keyword->word_nr)
			fprintf(fp, "-----Additional Keywords-----\n");
		fprintf(fp, "%s:\n", keyword->words[i].word);
		fprintf(fp, "\tstdev = %.6f\n", keyword->words[i].stdev);
		fprintf(fp, "\tcount = %d\n", keyword->words[i].count);
	}

	for (int i = 0; i < mails->mail_nr; i++) {
		if (i == 0)
			fprintf(fp, "-------------Mails-----------\n");
		fprintf(fp, "%s:\n", mails->mail[i].mail_nr);
		fprintf(fp, "\tsize = %d\n", mails->mail[i].size);
		fprintf(fp, "\tchar_size = %d\n", mails->mail[i].char_size);
		fprintf(fp, "\thas_caps = %d\n", mails->mail[i].has_caps);
		fprintf(fp, "\tspam_score = %d\n", mails->mail[i].spam_score);
		fprintf(fp, "\tkey_score = %.6f\n", mails->mail[i].key_score);
		fprintf(fp, "\tfinal_score = %.6f\n", mails->mail[i].final_score);
		fprintf(fp, "\tis_spam = %d\n", mails->mail[i].is_spam);
	}

	free_all(mails, keyword, spams);

	fclose(fp);

	return 0;
}
