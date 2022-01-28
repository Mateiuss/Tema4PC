#include "headers.h"

// Test branch

// Functie care adauga caracterul null la sfaristul unui string in loc de \n
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

	keyword->words = malloc(word_nr * sizeof(kword));

	for (int i = 0; i < word_nr; i++) {
		fgets(keyword->words[i].word, WORDLEN, fp);
		add_null(keyword->words[i].word);

		keyword->words[i].count = 0;
	}

	fclose(fp);
}

void add_keywords(keywords *keyword)
{
	keyword->ext_word_nr = keyword->word_nr;

	FILE *fp = fopen(ADD_KEYWORDS, "r");

	int add = 0;

	fscanf(fp, "%d", &add);
	fgetc(fp);

	keyword->ext_word_nr += add;

	int word_nr = keyword->ext_word_nr;

	keyword->words = realloc(keyword->words, word_nr * sizeof(kword));

	for (int i = keyword->word_nr; i < word_nr; i++) {
		fgets(keyword->words[i].word, WORDLEN, fp);
		add_null(keyword->words[i].word);

		keyword->words[i].count = 0;
	}

	fclose(fp);
}

void find(email *mail, keywords *keyword)
{
	FILE *fp = fopen(mail->mail_nr, "r");

	char *str = malloc(STRINGLEN * sizeof(char));

	for (int i = 0; i < keyword->ext_word_nr; i++) {
		fseek(fp, 0, SEEK_SET);

		int in_body = 0;

		while (fgets(str, STRINGLEN, fp)) {
			char *p = str;

			if (in_body == 1 || strstr(str, "Body:")) {
				in_body = 1;

				while (strcasestr(p, keyword->words[i].word)) {
					if (p[0] != 0)
						p = strcasestr(p, keyword->words[i].word) + 1;
					keyword->words[i].count++;
					mail->word_count[i]++;
				}
			}
		}
	}

	free(str);
	fclose(fp);
}

void search_words(emails *mails, keywords *keyword)
{
	for (int i = 0; i < mails->mail_nr; i++)
		find(mails->mail + i, keyword);
}

void print_task1(keywords *keyword)
{
	FILE *fp = fopen(STATISTICS, "w");

	for (int i = 0; i < keyword->word_nr; i++) {
		char *str = keyword->words[i].word;
		int count = keyword->words[i].count;
		float stdev = keyword->words[i].stdev;

		fprintf(fp, "%s %d %.6f\n", str, count, stdev);
	}

	fclose(fp);
}

void stdev(emails *mails, keywords *keyword)
{
	int n = mails->mail_nr;

	for (int i = 0; i < keyword->ext_word_nr; i++) {
		float ma = 0, stdev = 0;

		for (int j = 0; j < n; j++)
			ma = ma + mails->mail[j].word_count[i];

		ma = ma / n;

		for (int j = 0; j < n; j++) {
			float count = mails->mail[j].word_count[i];
			stdev = stdev + (count - ma) * (count - ma);
		}

		stdev = sqrt(stdev / n);

		keyword->words[i].stdev = stdev;
	}
}

void is_char(email *mail, char *p)
{
	for (int j = 0; j < strlen(p); j++) {
		if ((p[j] >= 'a' && p[j] <= 'z') || (p[j] >= 'A' && p[j] <= 'Z'))
			mail->char_size++;
	}
}

float avg_sizef(emails *mails)
{
	int len = 0;

	char *str = malloc(STRINGLEN * sizeof(char));

	for (int i = 0; i < mails->mail_nr; i++) {
		FILE *fp = fopen(mails->mail[i].mail_nr, "r");

		mails->mail[i].size = 0;
		mails->mail[i].char_size = 0;

		int in_body = 0;

		while (fgets(str, STRINGLEN, fp)) {
			if (in_body == 1 || strstr(str, "Body:")) {
				add_null(str);

				if (in_body == 0) {
					char *p = str;
					p = p + strlen("Body:");
					len += strlen(p);

					mails->mail[i].size += strlen(p);

					is_char(mails->mail + i, p);

					in_body = 1;
				} else {
					int str_len = strlen(str);
					len += str_len;
					mails->mail[i].size += str_len;

					is_char(mails->mail + i, str);
				}
			}
		}

		fclose(fp);
	}

	free(str);

	return len;
}

void keywords_score(emails *mails, keywords *keyword)
{
	float avg_size = avg_sizef(mails) / mails->mail_nr;
	mails->avg_size = avg_size;

	for (int i = 0; i < mails->mail_nr; i++) {
		mails->mail[i].key_score = 0;

		for (int j = 0; j < keyword->ext_word_nr; j++) {
			float count = mails->mail[i].word_count[j];
			int size = mails->mail[i].size;
			mails->mail[i].key_score += count * avg_size / size;
		}
	}
}

void has_caps(emails *mails)
{
	char *str = malloc(STRINGLEN * sizeof(char));

	for (int i = 0; i < mails->mail_nr; i++) {
		mails->mail[i].has_caps = 0;

		FILE *fp = fopen(mails->mail[i].mail_nr, "r");

		int in_body = 0;
		int caps = 0;

		while (fgets(str, STRINGLEN, fp)) {
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

		float size = mails->mail[i].char_size;
		mails->mail[i].has_caps = caps > size / 2 ? 1 : 0;

		fclose(fp);
	}

	free(str);
}

void is_longf(emails *mails)
{
	float avg_size = mails->avg_size;
	for (int i = 0; i < mails->mail_nr; i++) {
		int size = mails->mail[i].size;

		mails->mail[i].is_long = size / 1.8 >= avg_size ? 1 : 0;
	}
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

void check_spammers(emails *mails, spammers *spams)
{
	char *str = malloc(MAILLEN * sizeof(char));

	for (int i = 0; i < mails->mail_nr; i++) {
		FILE *fp = fopen(mails->mail[i].mail_nr, "r");

		mails->mail[i].spam_score = 0;

		int in_from = 0;

		while (in_from == 0 && fgets(str, MAILLEN, fp)) {
			if (strstr(str, "From:")) {
				in_from = 1;

				for (int j = 0; j < spams->spam_nr; j++) {
					if (strstr(str, spams->spammers[j].address)) {
						mails->mail[i].spam_score = spams->spammers[j].score;
						break;
					}
				}
			}
		}

		fclose(fp);
	}

	free(str);
}

void is_spamf(emails *mails)
{
	float key_score;
	float avg_size = mails->avg_size;
	int has_caps;
	int spam_score;
	int is_long;

	for (int i = 0; i < mails->mail_nr; i++) {
		float score = 0;
		key_score = mails->mail[i].key_score;
		has_caps = mails->mail[i].has_caps;
		spam_score = mails->mail[i].spam_score;
		is_long = mails->mail->is_long;

		score = 7 * key_score + 30 * has_caps + spam_score + 30 * is_long;

		mails->mail[i].final_score = score;

		mails->mail[i].is_spam = score > 35 ? 1 : 0;
	}
}

void print_task2(emails *mails)
{
	FILE *fp = fopen("prediction.out", "w");

	for (int i = 0; i < mails->mail_nr; i++)
		fprintf(fp, "%d\n", mails->mail[i].is_spam);

	fclose(fp);
}

void free_all(emails *mails, keywords *keyword, spammers *spams)
{
	for (int i = 0; i < mails->mail_nr; i++)
		free(mails->mail[i].word_count);
	free(mails->mail);
	free(mails);

	free(spams->spammers);
	free(spams);

	free(keyword->words);
	free(keyword);
}
