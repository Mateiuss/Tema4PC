#include "headers.h"

void add_null(char *str) {
    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = 0;
    }
}

int sizeof_int(int nr) {
    if(nr == 0) {
        return 1;
    }

    int k = 0;
    while(nr) {
        k++;
        nr /= 10;
    }
    return k;
}

void int_to_char(char *str, int nr) {
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

int email_nr() {
    DIR *dirp = opendir(EMAILS);
    struct dirent * entry;

    int file_count = 0;

    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            file_count++;
        }
    }

    closedir(dirp);
    return file_count;
}

void read_keywords(keywords *keyword) {
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

void find(email *email, keywords *keyword) {
    FILE *fp = fopen(email->mail_nr, "r");

    char *str = malloc(200);

    for (int i = 0; i < keyword->word_nr; i++) {
        fseek(fp, 0, SEEK_SET);

        int in_body = 0;

        while (fgets(str, 200, fp)) {

            char *p = str;

            if (strstr(str, "Body:") > 0 || in_body == 1) {
                in_body = 1;

                while (strcasestr(p, keyword->words[i].word)) {
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

void search_words(email *emails, keywords *keyword) {
    for (int i = 0; i < emails->n; i++) {
        find(emails + i, keyword);
    }
}