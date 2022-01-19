#include "headers.h"

int main(void)
{
	int n = email_nr();

	keywords *keyword = malloc(sizeof(keywords));
	read_keywords(keyword);

	email *emails = malloc(n * sizeof(email));
	emails->n = n;

	for (int i = 0; i < n; i++) {
		emails[i].word_count = calloc(keyword->word_nr, sizeof(int));
		int_to_char(emails[i].mail_nr, i);
	}

	search_words(emails, keyword);

	stdev(emails, keyword);

	print_task1(emails, keyword);

	free_all(emails, keyword);

	return 0;
}
