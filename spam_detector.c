#include "headers.h"

int main() {
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

    for (int i = 0; i < keyword->word_nr; i++) {
        float ma = 0, stdev = 0;

        for (int j = 0; j < n; j++) {
            ma = ma + emails[j].word_count[i];
            stdev = stdev + (emails[j].word_count[i] - ma) * (emails[j].word_count[i] - ma);
        }

        ma = ma / n;

        stdev = sqrt(stdev / n);

        keyword->words[i].stdev = stdev;
    }

    FILE *fp = fopen("statistics.out", "w");

    for (int i = 0; i < keyword->word_nr; i++) {
        fprintf(fp, "%s %d %.6f\n", keyword->words[i].word, keyword->words[i].count, keyword->words[i].stdev);
    }

    fclose(fp);

    return 0;
}