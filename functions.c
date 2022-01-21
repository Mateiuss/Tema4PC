#include "headers.h"

void add_null(char *str)
{
	if (str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = 0;
}

int sizeof_int(int nr)
{
	if (nr == 0)
		return 1;

	int k = 0;
	while (nr) {
		k++;
		nr /= 10;
	}
	return k;
}

void int_to_char(char *str, int nr)
{
	char s[5];

	int n = sizeof_int(nr);

	for (int i = n - 1; i >= 0; i--) {
		s[i] = nr % 10 + '0';
		nr = nr / 10;
	}

	s[n] = 0;

	strcpy(str, "data/emails/");
	strcat(str, s);
}

int email_nr(void)
{
	DIR *dirp = opendir(EMAILS);
	struct dirent *entry;

	int file_count = 0;

	while ((entry = readdir(dirp)) != NULL) {
		if (entry->d_type == DT_REG)
			file_count++;
	}

	closedir(dirp);
	return file_count;
}

void read_keywords(keywords *keyword)
{
	FILE *fp = fopen(KEYWORDS, "r");

	fscanf(fp, "%d", &keyword->word_nr);
	fgetc(fp);
	int word_nr = keyword->word_nr;

	keyword->words = malloc(word_nr * sizeof(word));

	for (int i = 0; i < word_nr; i++) {
		fgets(keyword->words[i].word, WORDLEN, fp);
		add_null(keyword->words[i].word);

		keyword->words[i].count = 0;
	}

	fclose(fp);
}

void find(email *email, keywords *keyword)
{
	FILE *fp = fopen(email->mail_nr, "r");

	char *str = malloc(1000 * sizeof(char));

	for (int i = 0; i < keyword->word_nr; i++) {
		fseek(fp, 0, SEEK_SET);

		int in_body = 0;

		while (fgets(str, 1000, fp)) {
			char *p = str;

			if (in_body == 1 || strstr(str, "Body:")) {
				in_body = 1;

				while (strcasestr(p, keyword->words[i].word)) {
					if (p[0] != 0)
						p = strcasestr(p, keyword->words[i].word) + 1;
					keyword->words[i].count++;
					email->word_count[i]++;
				}
			}
		}
	}

	free(str);
	fclose(fp);
}

void search_words(email *emails, keywords *keyword)
{
	for (int i = 0; i < emails->n; i++)
		find(emails + i, keyword);
}

void print_task1(email *emails, keywords *keyword)
{
	FILE *fp = fopen("statistics.out", "w");

	for (int i = 0; i < keyword->word_nr; i++) {
		char *str = keyword->words[i].word;
		int count = keyword->words[i].count;
		float stdev = keyword->words[i].stdev;

		fprintf(fp, "%s %d %.6f\n", str, count, stdev);
	}

	fclose(fp);
}

void stdev(email *emails, keywords *keyword)
{
	int n = emails->n;

	for (int i = 0; i < keyword->word_nr; i++) {
		float ma = 0, stdev = 0;

		for (int j = 0; j < n; j++)
			ma = ma + emails[j].word_count[i];

		ma = ma / n;

		for (int j = 0; j < n; j++) {
			float count = emails[j].word_count[i];
			stdev = stdev + (count - ma) * (count - ma);
		}

		stdev = sqrt(stdev / n);

		keyword->words[i].stdev = stdev;
	}
}

float avg_sizef(email *emails)
{
	int len = 0;

	char *str = malloc(1000 * sizeof(char));

	for (int i = 0; i < emails->n; i++) {
		FILE *fp = fopen(emails[i].mail_nr, "r");

		emails[i].size = 0;

		int in_body = 0;

		while (fgets(str, 1000, fp)) {
			if (in_body == 1 || strstr(str, "Body:")) {
				add_null(str);

				if (in_body == 0) {
					char *p = str;
					p = p + strlen("Body:");
					len += strlen(p);
					emails[i].size += strlen(p);
					in_body = 1;
				} else {
					int str_len = strlen(str);
					len += str_len;
					emails[i].size += str_len;
				}
			}
		}

		fclose(fp);
	}

	free(str);

	return len;
}

void keywords_score(email *emails, keywords *keyword)
{
	float avg_size = avg_sizef(emails) / emails->n;

	for (int i = 0; i < emails->n; i++) {
		emails[i].key_score = 0;

		for (int j = 0; j < keyword->word_nr; j++) {
			float count = emails[i].word_count[j];
			int size = emails[i].size;
			emails[i].key_score += count * avg_size / size;
		}
	}
}

void has_caps(email *emails)
{
	char *str = malloc(1000 * sizeof(char));

	for (int i = 0; i < emails->n; i++) {
		emails[i].has_caps = 0;

		FILE *fp = fopen(emails[i].mail_nr, "r");

		int in_body = 0;
		int caps = 0;

		while (fgets(str, 1000, fp)) {
			if (in_body == 1 || strstr(str, "Body:")) {
				int i = in_body ? 0 : strlen("Body:");
				for (i ; i < strlen(str); i++) {
					if (str[i] >= 'A' && str[i] <= 'Z')
						caps++;
				}

				if (in_body == 0)
					in_body = 1;
			}
		}
		emails[i].has_caps = caps > (float)emails[i].size / 2 ? 1 : 0;

		fclose(fp);
	}

	free(str);
}

void save_spammers(spammers *spams)
{
	FILE *fp = fopen(SPAMMERS, "r");

	int n = 0;
	fscanf(fp, "%d\n", &n);

	spams->spam_nr = n;

	spams->spammers = malloc(n * sizeof(spammer));

	char *address;
	int *score;

	for (int i = 0; i < n; i++) {
		address = spams->spammers[i].address;
		score = &spams->spammers[i].score;
		fscanf(fp, "%s %d", address, score);
	}

	fclose(fp);
}

void check_spammers(email *emails, spammers *spams)
{
	char *str = malloc(MAILLEN * sizeof(char));

	for (int i = 0; i < emails->n; i++) {
		FILE *fp = fopen(emails[i].mail_nr, "r");

		emails[i].spam_score = 0;

		int in_from = 0;

		while (in_from == 0 && fgets(str, MAILLEN, fp)) {
			if (strstr(str, "From:")) {
				in_from = 1;

				for (int j = 0; j < spams->spam_nr; j++) {
					if (strstr(str, spams->spammers[j].address)) {
						emails[i].spam_score = spams->spammers[j].score;
						break;
					}
				}
			}
		}

		fclose(fp);
	}

	free(str);
}

void is_spamf(email *emails)
{
	float key_score;
	int has_caps;
	int spam_score;

	for (int i = 0; i < emails->n; i++) {
		float score = 0;
		key_score = emails[i].key_score;
		has_caps = emails[i].has_caps;
		spam_score = emails[i].spam_score;

		score = 10 * key_score + 30 * has_caps + spam_score;

		emails[i].is_spam = score > 35 ? 1 : 0;
	}
}

void print_task2(email *emails)
{
	FILE *fp = fopen("prediction.out", "w");

	for (int i = 0; i < emails->n; i++)
		fprintf(fp, "%d\n", emails[i].is_spam);

	fclose(fp);
}

void free_all(email *emails, keywords *keyword, spammers *spams)
{
	for (int i = 0; i < emails->n; i++)
		free(emails[i].word_count);
	free(emails);

	free(spams->spammers);
	free(spams);

	free(keyword->words);
	free(keyword);
}
