#include "headers.h"

int main(void)
{
	int n = email_nr();

	keywords *keyword = malloc(sizeof(keywords));
	read_keywords(keyword);

	email *emails = malloc(n * sizeof(email));
	emails->n = n;

	spammers *spams = malloc(sizeof(spammers));

	for (int i = 0; i < n; i++) {
		emails[i].word_count = calloc(keyword->word_nr, sizeof(int));
		int_to_char(emails[i].mail_nr, i);
	}

	search_words(emails, keyword);

	stdev(emails, keyword);

	print_task1(emails, keyword);

	keywords_score(emails, keyword);

	/*for(int i = 0; i < n; i++) {
		printf("----------------------------------Mail-ul %d----------------------------------\n", i);
		printf("Marimea mail-ului este: %d\n", emails[i].size);
		for (int j = 0; j < keyword->word_nr; j++) {
			printf("%s %d\n", keyword->words[j].word, emails[i].word_count[j]);
		}
	}*/

	/*for (int i = 0; i < n; i++) {
		printf("scorul emailului %d este: %.6f\n", i, emails[i].score);
	}*/

	has_caps(emails);

	/*for (int i = 0; i < n; i++) {
		printf("Mail-ul %d are caps: %d\n", i, emails[i].has_caps);
	}*/
	save_spammers(spams);

	/*for (int i = 0; i < spams->spam_nr; i++) {
		printf("Adresa este: %s si scorul ei este: %d\n", spams->spammers[i].address, spams->spammers[i].score);
	}*/

	check_spammers(emails, spams);

	/*for (int i = 0; i < emails->n; i++) {
		printf("Spam score-ul email-ului %d este: %d\n", i, emails[i].spam_score);
	}*/

	is_spamf(emails);

	print_task2(emails);

	free_all(emails, keyword, spams);
	return 0;
}
