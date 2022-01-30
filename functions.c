#include "headers.h"

// Functie care adauga caracterul null la sfaristul unui string in loc de \n
void add_null(char *str)
{
	if (str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = 0;
}

// Functie care returneaza lungiea unui numar
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

// Functie care memoreaza in sirul str numarul email-ului memorat in nr
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

// Functie care returneaza numarul de fisiere dintr-un director
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

// Functie care memoreaza keyword-urile date
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

// Functie care memoreaza keyword-urile adaugate de mine
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

// Functie care cauta in mail-ul "mail" fiecare dintre keyword-uri
void find(email *mail, keywords *keyword)
{
	FILE *fp = fopen(mail->mail_nr, "r");

	mail->recon_words = 0;

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
					mail->recon_words++;
				}
			}
		}
	}

	free(str);
	fclose(fp);
}

// Functie care parcurge fiecare mail si cauta aparitiile keyword-urilor
void search_words(emails *mails, keywords *keyword)
{
	for (int i = 0; i < mails->mail_nr; i++)
		find(mails->mail + i, keyword);
}

// Functie care afiseaza datele obtinute pentru primul task
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

// Functie care calculeaza de iatia standard pentru fiecare mail
void stdev(emails *mails, keywords *keyword)
{
	int n = mails->mail_nr;
	keyword->avg_stdev = 0;

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

		keyword->avg_stdev += stdev;
	}

	keyword->avg_stdev /= n;
}

// Functie care numara fiecare litera din sirul "p"
void is_char(email *mail, char *p)
{
	for (int j = 0; j < strlen(p); j++) {
		if ((p[j] >= 'a' && p[j] <= 'z') || (p[j] >= 'A' && p[j] <= 'Z'))
			mail->char_size++;
	}
}

// Functie care calculeaza marimea medie a mail-urilor
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

// Functie care calculeaza keywords score-ul pentru fiecare mail
void keywords_score(emails *mails, keywords *keyword)
{
	float avg_size = avg_sizef(mails) / mails->mail_nr;
	float avg_stdev = keyword->avg_stdev;

	mails->avg_size = avg_size;

	for (int i = 0; i < mails->mail_nr; i++) {
		mails->mail[i].key_score = 0;

		for (int j = 0; j < keyword->ext_word_nr; j++) {
			float count = mails->mail[i].word_count[j];
			float stdev = keyword->words[j].stdev;
			int size = mails->mail[i].size;

			float sol = count * (avg_size) / (size);

			mails->mail[i].key_score += sol;
		}
	}
}

// Functie care verifica daca jumatate din caracterele mail-ului sunt cu
// CAPS sau nu
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

// Functie care verifica daca mail-ul depaseste de cel putin doua ori marimea
// medie a mail-urilor
void is_longf(emails *mails)
{
	float avg_size = mails->avg_size;
	for (int i = 0; i < mails->mail_nr; i++) {
		int size = mails->mail[i].size;

		mails->mail[i].is_long = size / 2.0 >= avg_size ? 1 : 0;
	}
}

// Functie care salveaza adresele de mail care sunt cunoscute ca trimit spam
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

// Functie care atribuie scorul adreselor cunoscute ca spameaza mail-urilor
// trimise de acestea
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

// Functie care verifica daca un mail este spam sau nu
void is_spamf(emails *mails)
{
	float key_score;
	float avg_size = mails->avg_size;
	int has_caps;
	int spam_score;

	for (int i = 0; i < mails->mail_nr; i++) {
		float score = 0;
		key_score = mails->mail[i].key_score;
		has_caps = mails->mail[i].has_caps;
		spam_score = mails->mail[i].spam_score;

		score = 7 * key_score + 30 * has_caps + spam_score;

		mails->mail[i].final_score = score;

		mails->mail[i].is_spam = score > 35 ? 1 : 0;
	}
}

// Functie care afiseaza cerinta task-ului doi
void print_task2(emails *mails)
{
	FILE *fp = fopen("prediction.out", "w");

	for (int i = 0; i < mails->mail_nr; i++)
		fprintf(fp, "%d\n", mails->mail[i].is_spam);

	fclose(fp);
}

// Functie care elibereaza memoria alocata
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

// Functie care afiseaza datele memorate in structurile create de mine,
// date precum: deviatia standard a keyword-urilor, cate aparitii are fiecare,
// lungie fiecarui mail, numarul de aparitii ai fiecarui keyword in fiecare
// dintre mail-uri, etc.
void logs(emails *mails, keywords *keyword)
{
	FILE *fp = fopen("logs.txt", "w");

	for (int i = 0; i < keyword->ext_word_nr; i++) {
		if (i == 0)
			fprintf(fp, "-----Keywords----------------\n");
		else if (i == keyword->word_nr)
			fprintf(fp, "-----Additional Keywords-----\n");
		fprintf(fp, "%s:\n", keyword->words[i].word);
		fprintf(fp, "\tstdev = %.6f\n", keyword->words[i].stdev);
		fprintf(fp, "\tavg_stdev = %.6f\n", keyword->avg_stdev);
		fprintf(fp, "\tcount = %d\n", keyword->words[i].count);
	}

	for (int i = 0; i < mails->mail_nr; i++) {
		if (i == 0)
			fprintf(fp, "-------------Mails-----------\n");

		fprintf(fp, "-----------------Mail %d---------------\n", i);
		fprintf(fp, "%s:\n", mails->mail[i].mail_nr);
		fprintf(fp, "\tsize = %d\n", mails->mail[i].size);
		fprintf(fp, "\tchar_size = %d\n", mails->mail[i].char_size);
		fprintf(fp, "\thas_caps = %d\n", mails->mail[i].has_caps);
		fprintf(fp, "\tspam_score = %d\n", mails->mail[i].spam_score);
		fprintf(fp, "\tkey_score = %.6f\n", mails->mail[i].key_score);
		fprintf(fp, "\tfinal_score = %.6f\n", mails->mail[i].final_score);
		fprintf(fp, "\tis_spam = %d\n", mails->mail[i].is_spam);

		fprintf(fp, "-----------Word counter---------------\n");
		for (int j = 0; j < keyword->ext_word_nr; j++) {
			fprintf(fp, "\t%12s :", keyword->words[j].word);
			fprintf(fp, " %d", mails->mail[i].word_count[j]);
			fprintf(fp, "\t%.6f\n", keyword->words[j].stdev);
		}
		fprintf(fp, "-----------Finished Word counter--------------\n");
	}

	fclose(fp);
}
