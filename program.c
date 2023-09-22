/* A simplified learned index implementation:
 *
 * Skeleton code written by Jianzhong Qi, April 2023
 * Edited by: Du-Simon Nguyen, 1352062
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define STAGE_NUM_ONE   1 /* stage numbers */
#define STAGE_NUM_TWO   2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR  4
#define STAGE_HEADER    "Stage %d\n==========\n" /* stage header format string */

#define DATASET_SIZE     100 /* number of input integers */
#define DATA_OUTPUT_SIZE 10  /* output size for stage 1 */

#define BS_NOT_FOUND (-1) /* used by binary search */
#define BS_FOUND     0

#define NOT_IN_DOMAIN 0
#define IN_DOMAIN     1

typedef int data_t; /* data type */

typedef struct {
    int p_a;
    int p_b;
    int ds_index;
} triple_t;

/****************************************************************/

/* function prototypes */
void print_stage_header(int stage_num);
int cmp(data_t *x1, data_t *x2);
void swap_data(data_t *x1, data_t *x2);
void partition(data_t dataset[], int n, data_t *pivot, int *first_eq,
               int *first_gt);
void quick_sort(data_t dataset[], int n);
int binary_search(data_t dataset[], int lo, int hi, data_t *key, int *locn);

void stage_one(data_t A[], int n);
void stage_two(data_t A[], int n);
int stage_three(int errm, data_t A[], triple_t segmemnts[], int n);
void stage_four(data_t A[], triple_t segments[], int n, int num_max,
                int max_err);

/* add your own function prototypes here */

void readintoarray(data_t A[], int n);
void printarray(data_t A[], int n);

void cmp_prederror(double fkey, int i, int *max_pe, int *position);
void stage_two_output(int A[], int *max_pe, int *position);
int pe(double fkey, int i, int stage_num);

int read_errm(int errm);
void para_a_b(int ds0, int index, int ds1, int *a, int *b);
double f_key(int *a, int *b, int key);
int iscovered(int *pred_error, int *errm);
void store_struct_array(int final_index, int a, int b, int segment_index,
                        triple_t segments[], int A[]);

int step_1(int val, int A[], int n);
int step_2(int val, int max_vals[], int num_max);
int binary_search_step2(int dataset[], int lo, int hi, data_t *key);
void step_3(int val, int map_num_index, int max_vals[], data_t dataset[],
            triple_t segments[], int max_err);
int binary_search_step3(int dataset[], int lo, int hi, data_t *key);
int low(int val, int fkey, int max_err);
int high(int val, int fkey, int max_err);

/****************************************************************/

/* main function controls all the action */
int
main(int argc, char *argv[]) {
    /* to hold all input data */
    data_t dataset[DATASET_SIZE];
    int max_err = 0;

    /* to hold the mapping functions */

    /* stage 1: read and sort the input */
    stage_one(dataset, DATASET_SIZE);

    /* stage 2: compute the first mapping function */
    stage_two(dataset, DATASET_SIZE);

    /* read in max_err for next stages */
    max_err = read_errm(max_err);

    /* holds mapping functions */
    triple_t segments[DATASET_SIZE];
    int num_max;

    /* stage 3: compute more mapping functions */
    num_max = stage_three(max_err, dataset, segments, DATASET_SIZE);

    /* stage 4: perform exact-match queries */
    stage_four(dataset, segments, DATASET_SIZE, num_max, max_err);

    /* all done; take some rest */
    /* algorithms are fun */
    return 0;
}

/****************************************************************/

/* add your code below */

/* stage 1: read and sort the input */
void
stage_one(data_t A[], int n) {
    /* add code for stage 1 */

    readintoarray(A, n);
    quick_sort(A, n);

    /* print stage header */
    print_stage_header(STAGE_NUM_ONE);
    printf("First 10 numbers:");
    printarray(A, DATA_OUTPUT_SIZE);
    printf("\n");
}

/* reads input and adds values into array */
void
readintoarray(data_t A[], int n) {

    int i = 0, n1;
    while (scanf("%d", &n1) == 1) {
        A[i] = n1;
        i++;
        if (i == n) {
            break;
        }
    }
}

/* prints the values of an array */
void
printarray(data_t A[], int n) {

    for (int i = 0; i < n; i++) {
        printf(" %d", A[i]);
    }
    printf("\n");
}

/* stage 2: compute the first mapping function */
void
stage_two(data_t A[], int n) {

    int a, b;
    int index = 0;
    int ds0 = A[index];
    int ds1 = A[index + 1];
    double fkey;
    int max_pe = 0;
    int position;

    /* calculate a and b */
    para_a_b(ds0, index, ds1, &a, &b);

    for (int i = 0; i < n; i++) {
        /* find fkey of each value */
        fkey = f_key(&a, &b, A[i]);

        /* compare to current max fkey and update if larger*/
        cmp_prederror(fkey, i, &max_pe, &position);
    }

    /* print stage header */
    print_stage_header(STAGE_NUM_TWO);
    stage_two_output(A, &max_pe, &position);
    printf("\n");
}

/* calculate parameters a and b */
void
para_a_b(int ds0, int index, int ds1, int *a, int *b) {

    if (ds0 == ds1) {
        /* special case where ds[0] = ds[1] */
        /* set b = 0 and a = subscript of ds0 */
        *a = index;
        *b = 0;
    } else if (index == DATASET_SIZE - 1) {
        /* 1 element left */
        *a = DATASET_SIZE - 1;
        *b = 0;
    } else {
        /* the 2 points are (ds0, index), (ds1, index + 1) */
        /* where ds0 = A[index]; ds1 = A[index + 1] */
        /* plugging the 2 points into linear equation (1) gives */
        *b = ds1 - ds0;
        *a = index * ds1 - (index + 1) * ds0;
    }
}

/* find f_key given a and b parameters */
double
f_key(int *a, int *b, int key) {

    double fkey;

    if (*b == 0) {
        /* special case */
        return *a;
    } else {
        /* find f(key) at given position*/
        fkey = (double) (key + *a) / (double) *b;
    }

    return fkey;
}

/* compares the current prediction error to max prediction error*/
void
cmp_prederror(double fkey, int i, int *max_pe, int *position) {

    /* find current prediction error*/
    int pred_error = pe(fkey, i, STAGE_NUM_TWO);

    if (pred_error > *max_pe) {
        /* if current pred error > max, then update max and position*/
        *max_pe = pred_error;
        *position = i;
    }
}

/* calculate prediction error of f for a given element*/
int
pe(double fkey, int i, int stage_num) {

    int pred_err = ((double) ceil(fkey)) - i;

    if (stage_num == STAGE_NUM_TWO) {
        /* used in stage 2 */
        return abs(pred_err);
    } else {
        /* used in stage 4 */
        return pred_err;
    }
}

/* Formats the output of stage 2 */
void
stage_two_output(int A[], int *max_pe, int *position) {

    printf("Maximum prediction error: %d\n", *max_pe);
    printf("For key: %d\n", A[*position]);
    printf("At position: %d\n", *position);
}

/* reads in the max prediction error value */
int
read_errm(int errm) {
    scanf("%d", &errm);

    return errm;
}

/* stage 3: compute more mapping functions */
int
stage_three(int errm, data_t A[], triple_t segments[], int n) {

    double fkey;
    int a, b, final_index, pred_error, index = 0, segment_index = 0;

    for (int i = index + 2; i < n; i++) {
        data_t ds0 = A[index];
        data_t ds1 = A[index + 1];

        /* calculate a and b, fkey and prediction error for current position */
        para_a_b(ds0, index, ds1, &a, &b);
        fkey = f_key(&a, &b, A[i]);
        pred_error = pe(fkey, i, STAGE_NUM_TWO);

        /* if element not covered, store triple(a, b, A[i - 1]) into segments */
        if (!iscovered(&pred_error, &errm)) {

            final_index = i - 1;
            store_struct_array(final_index, a, b, segment_index, segments, A);
            segment_index++;
            index = i;
        }
    }

    /* update the last segment */
    store_struct_array(DATASET_SIZE - 1, a, b, segment_index, segments, A);

    /* print stage header */
    print_stage_header(STAGE_NUM_THREE);
    printf("Target maximum prediction error: %d\n", errm);
    for (int i = 0; i <= segment_index; i++) {
        printf("Function %2d: a = %4d, b = %3d, max element = %3d\n", i,
               segments[i].p_a, segments[i].p_b, A[segments[i].ds_index]);
    }

    printf("\n");
    return segment_index;
}

/* checks whether a value is covered by f */
int
iscovered(int *pred_error, int *errm) {

    if (*pred_error > *errm) {
        /* is not covered */
        return 0;
    }

    return 1;
}

/* stores function num information (a, b, A[i - 1]) into structure array */
void
store_struct_array(int final_index, int a, int b, int segment_index,
                   triple_t segments[], int A[]) {

    triple_t info;

    info.p_a = a;
    info.p_b = b;
    info.ds_index = final_index;

    segments[segment_index] = info;
}

/* stage 4: perform exact-match queries */
void
stage_four(data_t A[], triple_t segments[], int n, int num_max, int max_err) {

    int val, map_num_index;
    int max_vals[DATASET_SIZE];

    /* create array containing only mapping function max values from segments */
    for (int i = 0; i <= num_max; i++) {
        max_vals[i] = A[segments[i].ds_index];
    }

    /* print stage header */
    print_stage_header(STAGE_NUM_FOUR);

    /* scan in each query and search for it */
    while (scanf("%d", &val) == 1) {

        printf("Searching for %d:\n", val);
        int valid_key = step_1(val, A, n);

        /* only continues to step 2 and 3 if query in domain */
        if (valid_key) {
            map_num_index = step_2(val, max_vals, num_max);
            step_3(val, map_num_index, max_vals, A, segments, max_err);
        }
    }

    printf("\n");
}

/* checks whether element is within array */
int
step_1(int val, int A[], int n) {

    if ((val < A[0]) || (val > A[n - 1])) {
        printf("Step 1: not found!\n");
        return NOT_IN_DOMAIN;

    } else {
        printf("Step 1: search key in data domain.\n");
        return IN_DOMAIN;
    }
}

/* binary search over array of mapping functions */
int
step_2(int val, int max_vals[], int num_max) {

    printf("Step 2:");

    int map_num_index;

    map_num_index = binary_search_step2(max_vals, 0, num_max + 1, &val);
    printf("\n");

    return map_num_index;
}

/* implementation of binary search to locate mapping function between
 * dataset[lo] & dataset[hi-1], adapted from binary_search at bottom of file */
int
binary_search_step2(int dataset[], int lo, int hi, data_t *key) {

    int mid, outcome;

    mid = (lo + hi) / 2;

    if (lo >= hi) {
        return mid;
    }

    if ((outcome = cmp(key, dataset + mid)) < 0) {
        printf(" %d", dataset[mid]);
        return binary_search_step2(dataset, lo, mid, key);
    } else if (outcome > 0) {
        printf(" %d", dataset[mid]);
        return binary_search_step2(dataset, mid + 1, hi, key);
    } else {
        printf(" %d", dataset[mid]);
        return mid;
    }
}

/* binary search over corresponding mapping function to locate key */
void
step_3(int val, int map_num_index, int max_vals[], data_t dataset[],
       triple_t segments[], int max_err) {

    printf("Step 3:");

    int a = segments[map_num_index].p_a;
    int b = segments[map_num_index].p_b;
    int fkey = ceil(f_key(&a, &b, val));
    int lo = low(val, fkey, max_err);
    int hi = high(val, fkey, max_err);

    binary_search_step3(dataset, lo, hi + 1, &val);
    printf("\n");
}

/* implementation of binary search to locate key between dataset[lo] &
 * dataset[hi-1], adapted from binary_search at bottom of file  */
int
binary_search_step3(int dataset[], int lo, int hi, data_t *key) {

    int mid, outcome;

    if (lo >= hi) {
        printf(" not found!");
        return BS_NOT_FOUND;
    }

    mid = (lo + hi) / 2;

    if ((outcome = cmp(key, dataset + mid)) < 0) {
        printf(" %d", dataset[mid]);
        return binary_search_step3(dataset, lo, mid, key);
    } else if (outcome > 0) {
        printf(" %d", dataset[mid]);
        return binary_search_step3(dataset, mid + 1, hi, key);
    } else {
        printf(" %d @ dataset[%d]!", dataset[mid], mid);
        return mid;
    }
}

/* finds the lower boundary used in binary search step 3 */
int
low(int val, int fkey, int max_err) {

    int bound = pe(fkey, max_err, STAGE_NUM_FOUR);

    if (bound > 0) {
        return bound;
    } else {
        return 0;
    }
}

/* finds the upper boundary used in binary search step 3 */
int
high(int val, int fkey, int max_err) {

    int bound = pe(fkey, -1 * max_err, STAGE_NUM_FOUR);

    if (bound < DATASET_SIZE - 1) {
        return bound;
    } else {
        return DATASET_SIZE - 1;
    }
}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void
print_stage_header(int stage_num) {
    printf(STAGE_HEADER, stage_num);
}

/* data swap function used by quick sort, adpated from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/sortscaffold.c
*/
void
swap_data(data_t *x1, data_t *x2) {
    data_t t;
    t = *x1;
    *x1 = *x2;
    *x2 = t;
}

/* partition function used by quick sort, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/quicksort.c
*/
void
partition(data_t dataset[], int n, data_t *pivot, int *first_eq,
          int *first_gt) {
    int next = 0, fe = 0, fg = n, outcome;

    while (next < fg) {
        if ((outcome = cmp(dataset + next, pivot)) < 0) {
            swap_data(dataset + fe, dataset + next);
            fe += 1;
            next += 1;
        } else if (outcome > 0) {
            fg -= 1;
            swap_data(dataset + next, dataset + fg);
        } else {
            next += 1;
        }
    }

    *first_eq = fe;
    *first_gt = fg;
    return;
}

/* quick sort function, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/quicksort.c
*/
void
quick_sort(data_t dataset[], int n) {
    data_t pivot;
    int first_eq, first_gt;
    if (n <= 1) {
        return;
    }
    /* array section is non-trivial */
    pivot = dataset[n / 2];   // take the middle element as the pivot
    partition(dataset, n, &pivot, &first_eq, &first_gt);
    quick_sort(dataset, first_eq);
    quick_sort(dataset + first_gt, n - first_gt);
}

/* comparison function used by binary search and quick sort, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c
*/
int
cmp(data_t *x1, data_t *x2) {
    return (*x1 - *x2);
}

/* binary search between dataset[lo] and dataset[hi-1], adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c
*/
int
binary_search(data_t dataset[], int lo, int hi, data_t *key, int *locn) {
    int mid, outcome;
    /* if key is in dataset, it is between dataset[lo] and dataset[hi-1] */
    if (lo >= hi) {
        return BS_NOT_FOUND;
    }
    mid = (lo + hi) / 2;

    if ((outcome = cmp(key, dataset + mid)) < 0) {
        printf("mid = %d ", dataset[mid]);
        return binary_search(dataset, lo, mid, key, locn);
    } else if (outcome > 0) {
        printf("mid = %d ", dataset[mid]);
        return binary_search(dataset, mid + 1, hi, key, locn);
    } else {
        *locn = mid;
        return BS_FOUND;
    }
}