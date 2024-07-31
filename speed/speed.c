#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

double timer(void (*func)(void)) {
    clock_t start;
    clock_t end;
    double cpu_time_used;

    start = clock();
    func();
    end = clock();

    cpu_time_used = ((double)(end - start));
    return cpu_time_used;
}

int str_cmp_a(const char* str_a, const char* str_b) {
  const unsigned char *s1 = (const unsigned char *) str_a;
  const unsigned char *s2 = (const unsigned char *) str_b;
  unsigned char c1, c2;

  do
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
        return c1 - c2;
    }
  while (c1 == c2);

  return c1 - c2;
}

int str_cmp_b(const char* str_a, const char* str_b) {
    int i = 0;
    while (str_a[i] != '\0' && str_b[i] != '\0') {
        if (str_a[i] != str_b[i]) {
            return str_a[i] - str_b[i];
        }
        i++;
    }

    return str_a[i] - str_b[i];
}

#define STRING_AMOUNT 5000000
#define STRING_LENGTH 32
void generate_rand_strings(char (*out)[STRING_LENGTH]) {
    srand(time(NULL));
    for (int i = 0; i < STRING_AMOUNT; ++i) {
        for (int j = 0; j < STRING_LENGTH - 1; ++j) {
            char rand_char = rand() % ('~' - '!' + 1) + '!';
            out[i][j] = rand_char;
        }
        out[i][STRING_LENGTH - 1] = '\0';
    }
}

char strings[STRING_AMOUNT][STRING_LENGTH];

void string_compare_A(void) {
    for (int i = 0; i < STRING_AMOUNT - 1; ++i) {
        str_cmp_a(strings[i], strings[i + 1]);
    }
}

void string_compare_B(void) {
    for (int i = 0; i < STRING_AMOUNT - 1; ++i) {
        str_cmp_b(strings[i], strings[i + 1]);
    }
}

void string_compare_C(void) {
    for (int i = 0; i < STRING_AMOUNT - 1; ++i) {
        (void)strcmp(strings[i], strings[i + 1]);
    }
}

int main(void) {

    // Generate random "words"
    generate_rand_strings(strings);

    // Time pointer comparison

    // Time index comparison
    double time_b = timer(string_compare_B);
    double time_a = timer(string_compare_A);

    // Time built-in comparison
    double time_c = timer(string_compare_C);

    // Output
    printf("String comparison (pointer type) took: %f\nString comparison (index type) took: %f\nString comparison (built-in) took: %f\n", time_a, time_b, time_c);

    return 0;
}
