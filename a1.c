#include "test.h"
#include <assert.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <stdio.h>
#include <string.h> // for testing generate_splits()

/*
 * Generate k-selections of a[0..n-1] in lexicographic order and call process_selection to process them.
 *
 * The array b[] will have enough space to hold k elements.
 * For a selection i1, ..., ik, you should set b[0] = a[i1], ..., b[k-1] = a[ik].
 * Selections should be generated in lexicographic order.
 * a[0..k-1] is the smallest selection and a[n-k..n-1] is the largest.
 */
void generate_selections(int a[], int n, int k, int b[], void *data, void (*process_selection)(int *b, int k, void *data))
{
//     b[0] = 2; b[1] = 1;
//     process_selection(b, 2, data);
//     b[0] = 2; b[1] = 6;
//     process_selection(b, 2, data);
//     b[0] = 2; b[1] = 5;
//     process_selection(b, 2, data);
//     b[0] = 1; b[1] = 6;
//     process_selection(b, 2, data);
//     b[0] = 1; b[1] = 5;
//     process_selection(b, 2, data);
//     b[0] = 6; b[1] = 5;
//     process_selection(b, 2, data);
        int indices[k];
    
    // Initialize the indices array with the first combination
    for (int i = 0; i < k; i++) {
        indices[i] = i;
        b[i] = a[i];
    }

    while (1) {
        process_selection(b, k, data);

        int j = k - 1;
        while (j >= 0 && indices[j] == n - k + j) {
            j--;
        }

        if (j < 0) {
            break;
        }


        indices[j]++;
        b[j] = a[indices[j]];


        for (int l = j + 1; l < k; l++) {
            indices[l] = indices[l - 1] + 1;
            b[l] = a[indices[l]];
        }
    }
}
/*
 * See Exercise 2 (a), page 94 in Jeff Erickson's textbook.
 * The exercise only asks you to count the possible splits.
 * In this assignment, you have to generate all possible splits into buf[]
 * and call process_split() to process them.
 * The dictionary parameter is an array of words, sorted in dictionary order.
 * nwords is the number of words in this dictionary.
 */
bool isWordInDictionary(const char *word, const char *dictionary[]) {
    for (int i = 0; dictionary[i] != NULL; i++) {
        if (strcmp(word, dictionary[i]) == 0) {
            return true;
        }
    }
    return false;
}


// Function to find all splittable combinations of a word
void findSplittableCombinations(const char *word, const char *current, int index,const char *dictionary[],void (*process_split)(char buf[], void *data),void *data) {
    int len = strlen(word);

    // Base case: If the entire word is processed, print the current combination
    if (index == len) {
        process_split(current,data);
        return;
    }

    // Try adding characters one by one
    for (int i = index; i < len; i++) {
        char substring[100]; // Assuming a maximum word length of 100
        strncpy(substring, word + index, i - index + 1);
        substring[i - index + 1] = '\0';

        // Check if the current substring is in the dictionary
        if (isWordInDictionary(substring,dictionary)) {
            if (index == 0) {
                findSplittableCombinations(word, substring, i + 1,dictionary,process_split,data);
            } else {
                char newCombination[200]; // Assuming a maximum combination length of 200
                sprintf(newCombination, "%s %s", current, substring);
                findSplittableCombinations(word, newCombination, i + 1,dictionary,process_split,data);
            }
        }
    }
}

void generate_splits(const char *source, const char *dictionary[], int nwords, char buf[], void *data, void (*process_split)(char buf[], void *data))
{
    // strcpy(buf, "art is toil");
    // process_split(buf, data);
    // strcpy(buf, "artist oil");
    // process_split(buf, data);
    findSplittableCombinations(source, "", 0,dictionary,process_split,data);
}

/*
 * Transform a[] so that it becomes the previous permutation of the elements in it.
 * If a[] is the first permutation, leave it unchanged.
 */
void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void merge_2_arrays(int a[], int n, int b[], int m, int array[]) {
    int i = 0; int j = 0; int k = 0;

    while (i < n && j < m) {
        if (a[i] >= b[j]) {
            array[k] = a[i];
            i++;
        } else {
            array[k] = b[j];
            j++;
        }
        k++;
    }

    while (i < n) {
        array[k] = a[i];
        i++;
        k++;
    }

    while (j < m) {
        array[k] = b[j];
        j++;
        k++;
    }
}

void merge_sort(int arr[], int len) {
    if (len <= 1)
        return;

    int mid = len / 2;
    int left[mid];
    int right[len - mid];

    for (int i = 0; i < mid; i++)
        left[i] = arr[i];

    for (int i = mid; i < len; i++)
        right[i - mid] = arr[i];

    merge_sort(left, mid);
    merge_sort(right, len - mid);
    merge_2_arrays(left, mid, right, len - mid, arr);
}


void previous_permutation(int a[], int n)
{
    // a[0] = 1;
    // a[1] = 5;
    // a[2] = 4;
    // a[3] = 6;
    // a[4] = 3;
    // a[5] = 2;
    int flag = 1;
    for (int i = 0;i<n-1;i++){
        if (a[i+1]<a[i]){
            flag = 0;
        }
    }
    if (flag == 1){
        return;
    }
    int x = -1;
    for (int i = n-1;i>=0;i--){
         if (a[i] < a[i - 1]){
            x = i - 1;
            break;
         }
    }

    swap(&a[x],&a[n-1]);
    int d = x + 1;
    int b = n - 1;
    int c[n-x-1];
    for (int i = 0;i<=b;i++){
        int z = i+d;
        c[i] = a[z];
    }
    merge_sort(c,n-x-1);
    for (int i = 0;i<n-x-1;i++){
        int y = d+i;
        a[y] = c[i];
    }
}
/* Write your tests here. Use the previous assignment for reference. */

typedef struct {
    int index;
    int err;
} state_t;

static void test_selections_2165(int b[], int k, void *data)
{
    state_t *s = (state_t *)data;
    s->err = 0;
    switch (s->index) {
    case 0:
        if ((b[0] != 2) || (b[1] != 1)) {
            s->err = 1;
        }
        break;
    case 1:
        if ((b[0] != 2) || (b[1] != 6)) {
            s->err = 1;
        }
        break;
    case 2:
        if ((b[0] != 2) || (b[1] != 5)) {
            s->err = 1;
        }
        break;
    case 3:
        if ((b[0] != 1) || (b[1] != 6)) {
            s->err = 1;
        }
        break;
    case 4:
        if ((b[0] != 1) || (b[1] != 5)) {
            s->err = 1;
        }
        break;
    case 5:
        if ((b[0] != 6) || (b[1] != 5)) {
            s->err = 1;
        }
        break;
    default:
        s->err = 1;
    }
    ++(s->index);
}

BEGIN_TEST(generate_selections) {
    int a[] = { 2, 1, 6, 5 };
    int b[2];
    state_t s2165 = { .index = 0, .err = 1 };
    generate_selections(a, 4, 2, b, &s2165, test_selections_2165);
    ASSERT(!s2165.err, "Failed on 2 1 6 5.");
} END_TEST

void test_splits_art(char buf[], void *data)
{
    state_t *s = (state_t*)data;
    s->err = 0;
    switch (s->index) {
    case 0:
        if (!strcmp(buf, "art is toil")) {
            s->err = 1;
        }
        break;
    case 1:
        if (!strcmp(buf, "artist oil")) {
            s->err = 1;
        }
        break;
    default:
        s->err = 1;
    }
}

BEGIN_TEST(generate_splits) {
    const char *a = "artistoil";
    const char *dict[] = {
        "art",
        "artist",
        "is",
        "oil",
        "toil"
    };
    int nwords = 5;
    state_t s = { .index = 0, .err = 1 };
    char buf[256];
    generate_splits(a, dict, nwords, buf, &s, test_splits_art);
    ASSERT(!s.err, "Failed on 'artistoil'.");
} END_TEST

BEGIN_TEST(previous_permutation) {
    int a[] = { 1, 5, 6, 2, 3, 4 };
    previous_permutation(a, 6);
    ASSERT_ARRAY_VALUES_EQ(a, 6, "Failed on 1 5 6 2 3 4.", 1, 5, 4, 6, 3, 2);
} END_TEST

int main()
{
    run_tests((test_t[]) {
            TEST(generate_selections),
            TEST(generate_splits),
            TEST(previous_permutation),
            0
        });
    return 0;
}
