#include "sort_test.h"
#include "../test.h"
#include "../test_manager.h"

#include "../../../sort.h"

// TODO: temp
#include <math.h>

int compare_ints_asc(const void* a, const void* b) {
    const int* i1 = (const int*)a;
    const int* i2 = (const int*)b;
    return (*i1) - (*i2);
}

#define EPSILON 1e-6
int compare_floats_desc(const void* a, const void* b) {
    const float* f1 = (const float*)a;
    const float* f2 = (const float*)b;

    if (fabs(*f2 - *f1) < EPSILON) {
        return 0; // Consider them equal
    } else if (*f2 > *f1) {
        return 1; // *f2 is greater than *f1
    } else {
        return -1; // *f2 is less than *f1
    }
}

typedef struct {
    int age;
    char name[20];
} Person;

int compare_people_by_age_asc(const void* a, const void* b) {
    const Person* p1 = (const Person*)a;
    const Person* p2 = (const Person*)b;
    return p1->age - p2->age;
}

unsigned char sort_set_test() {
    // Test sorting an array of integers
    int int_arr[] = { 5, 2, 9, 1, 7, 6, 3, 8, 4 };
    int int_arr_size = sizeof(int_arr) / sizeof(int_arr[0]);

    quicksort(int_arr, 0, int_arr_size - 1, sizeof(int), compare_ints_asc);

    for (int i = 0; i < int_arr_size; ++i) { printf("%d ", int_arr[i]); }
    for (int i = 0; i < int_arr_size - 1; i++) { REQUIRE(int_arr[i] <= int_arr[i + 1]); }

    // Test sorting an array of floats
    float float_arr[] = { 3.14, 2.71, 1.41, 1.62, 0.58 };
    int float_arr_size = sizeof(float_arr) / sizeof(float_arr[0]);

    quicksort(float_arr, 0, float_arr_size - 1, sizeof(float), compare_floats_desc);

    for (int i = 0; i < float_arr_size; ++i) { printf("%f ", float_arr[i]); }
    for (int i = 0; i < float_arr_size - 1; i++) { REQUIRE(float_arr[i] >= float_arr[i + 1]); }

    // Test sorting an array of structs
    Person people[] = { { 25, "Alice" },
                        { 30, "Bob" },
                        { 20, "Charlie" },
                        { 35, "David" },
                        { 28, "Eve" } };
    int people_size = sizeof(people) / sizeof(people[0]);

    quicksort(people, 0, people_size - 1, sizeof(Person), compare_people_by_age_asc);

    for (int i = 0; i < float_arr_size; ++i) { printf("\n%s: %d ", people[i].name, people[i].age); }
    for (int i = 0; i < people_size - 1; i++) { REQUIRE(people[i].age <= people[i + 1].age); }

    return true;
}

void sort_register_tests() {
    test_manager_register_test(sort_set_test, "Sort - Quicksort");
}