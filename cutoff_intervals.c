#include "common.h"






void find_intervals_above_cutoff(float* data, size_t len, float cutoff, interval_t** intervals_out, size_t* nr_intervals_out) {
    assert(nr_intervals_out != NULL && intervals_out != NULL);

    size_t nr_intervals = 0, intervals_calculated = 0;
    bool is_in_interval = false;
    for(int i = 0; i < len; i++) {
        if(!is_in_interval && data[i] > cutoff) {
            is_in_interval = true;
            nr_intervals++;
        } else if (is_in_interval && data[i] < cutoff) {
            is_in_interval = false;
        }
    }

    nr_intervals_out[0] = nr_intervals;
    intervals_out[0] = calloc(sizeof(interval_t), nr_intervals);

    is_in_interval = false;
    for(int i = 0; i < len; i++) {
        if(!is_in_interval && data[i] > cutoff) {
            is_in_interval = true;
            intervals_out[0][intervals_calculated].lower_index = i;
        } else if (is_in_interval && data[i] < cutoff) {
            is_in_interval = false;
            intervals_out[0][intervals_calculated].upper_index = i;
            intervals_calculated++;
        }
    }
    if(is_in_interval) {
        is_in_interval = false;
        intervals_out[0][intervals_calculated].upper_index = len-1;
        intervals_calculated++;
    }

    assert(intervals_calculated == nr_intervals);
}


void print_intervals(interval_t* intervals, size_t nr_intervals) {
    for(int i = 0; i < nr_intervals; i++) {
        printf("[%li,%li]\n", intervals[i].lower_index, intervals[i].upper_index);
    }
}


bool interval_contains(interval_t environment, interval_t inner) {
    return ((environment.lower_index <= inner.lower_index)&&(inner.upper_index <= environment.upper_index));
}
size_t smax(size_t a, size_t b) { if(a < b) return b; else return a; }
size_t smin(size_t a, size_t b) { if(a > b) return b; else return a; }
size_t distance(interval_t a, interval_t b) {
    size_t min_upper = smin(a.upper_index, b.upper_index);
    size_t max_lower = smax(a.lower_index, b.lower_index);
    size_t distance = max_lower - min_upper;
    if(distance <= 0) return 0;
    else return distance;
}
interval_t encompassing(interval_t a, interval_t b) {
    interval_t ret;
    ret.upper_index = smax(a.upper_index, b.upper_index);
    ret.lower_index = smin(a.lower_index, b.lower_index);
    return ret;
}
void print_interval_distances(interval_t* intervals, size_t nr_intervals) {
    for(int i = 0; i < nr_intervals-1; i++) {
        interval_t merge = encompassing(intervals[i], intervals[i+1]);
        printf("[%li,%li] to [%li,%li] : %li; merge = [%li,%li]\n", intervals[i].lower_index, intervals[i].upper_index, intervals[i+1].lower_index, intervals[i+1].upper_index, distance(intervals[i], intervals[i+1]), merge.lower_index, merge.upper_index);
    }
}



/* destroys "intervals" param; also [0,0] intervals will be deleted */
void chunk_interval_list(interval_t* intervals, size_t nr_intervals, interval_t** intervals_out, size_t* nr_intervals_out, size_t max_distance) {
    /*
     * start with the list, and replace any pair of intervals a,b with distance(a,b) < N with the interval encompassing(a,b) as long as possible
     */
    int* pos_marker = calloc(nr_intervals, sizeof(int));
    for(int i = 0; i < nr_intervals; i++) {
        pos_marker[i] = i;
    }
    while(true) {
        bool sth_still_to_do = false;
        for(int i = 0 ; i < nr_intervals; i++) {
            for(int j = i+1; j < nr_intervals; j++) {
                int i_a = pos_marker[i];
                int i_b = pos_marker[j];
                if(i_a != i_b) {
                    interval_t a = intervals[i_a];
                    interval_t b = intervals[i_b];
                    if(distance(a,b) < max_distance) {
                        sth_still_to_do = true;
                        interval_t enc = encompassing(a,b);
                        pos_marker[j] = pos_marker[i];
                        intervals[i_a] = enc;
                        intervals[i_b].upper_index = intervals[i_b].lower_index = 0;
                    }
                }
            }
        }
        if(!sth_still_to_do) break;
    }

    nr_intervals_out[0] = 0;
    for(int i = 0; i < nr_intervals; i++) {
        if(intervals[i].upper_index != 0 || intervals[i].lower_index != 0)
            nr_intervals_out[0]++;
    }

    intervals_out[0] = calloc(sizeof(interval_t),nr_intervals_out[0]);
    int written = 0;
    for(int i = 0; i < nr_intervals; i++) {
        if(intervals[i].upper_index != 0 || intervals[i].lower_index != 0) {
            intervals_out[0][written] = intervals[i];
            written++;
        }
    }

    assert(nr_intervals_out[0] == written);
}



void merge_interval_lists(interval_t* old_intervals, size_t nr_old_intervals, interval_t* new_intervals, size_t nr_new_intervals, interval_t** intervals_out, size_t* nr_intervals_out) {
    size_t how_many_new_intervals = 0, nr_copied_over = 0;
    for(int i = 0; i < nr_new_intervals; i++) {
        bool is_new = true;
        for(int j = 0; j < nr_old_intervals; j++) {
            if(interval_contains(new_intervals[i], old_intervals[j])) {
                is_new = false;
                break;
            }
        }
        if(is_new) how_many_new_intervals++;
    }

    nr_intervals_out[0] = nr_old_intervals+how_many_new_intervals;
    intervals_out[0] = calloc(sizeof(interval_t),nr_intervals_out[0]);
    memcpy(intervals_out[0], old_intervals, sizeof(interval_t)*nr_old_intervals);

    for(int i = 0; i < nr_new_intervals; i++) {
        bool is_new = true;
        for(int j = 0; j < nr_old_intervals; j++) {
            if(interval_contains(new_intervals[i], old_intervals[j])) {
                is_new = false;
                break;
            }
        }
        if(is_new) {
            intervals_out[0][nr_old_intervals+nr_copied_over] = new_intervals[i];
            nr_copied_over++;
        }
    }

    assert(nr_copied_over == how_many_new_intervals);
}




void merge_interval_lists_close_step(interval_t* old_intervals, size_t nr_old_intervals, interval_t* new_intervals, size_t nr_new_intervals, interval_t** intervals_out, size_t* nr_intervals_out) {
    size_t how_many_new_intervals = 0, nr_copied_over = 0;
    for(int i = 0; i < nr_new_intervals; i++) {
        bool is_new = true;
        for(int j = 0; j < nr_old_intervals; j++) {
            if(interval_contains(new_intervals[i], old_intervals[j])) {
                is_new = false;
                break;
            }
        }
        if(is_new) how_many_new_intervals++;
    }



    nr_intervals_out[0] = nr_old_intervals+how_many_new_intervals;
    intervals_out[0] = calloc(sizeof(interval_t),nr_intervals_out[0]);
    memcpy(intervals_out[0], old_intervals, sizeof(interval_t)*nr_old_intervals);

    for(int i = 0; i < nr_new_intervals; i++) {
        bool is_new = true;
        for(int j = 0; j < nr_old_intervals; j++) {
            if(interval_contains(new_intervals[i], old_intervals[j])) {
                is_new = false;
                break;
            }
        }
        if(is_new) {
            intervals_out[0][nr_old_intervals+nr_copied_over] = new_intervals[i];
            nr_copied_over++;
        }
    }

    assert(nr_copied_over == how_many_new_intervals);
}



void sort_interval_lists(interval_t* intervals, size_t nr_intervals) {
    /* iterative quicksort, adapted from https://www.geeksforgeeks.org/iterative-quick-sort/ */
    int* stack = calloc(nr_intervals,sizeof(int));
    stack[0] = 0;
    stack[1] = nr_intervals-1;
    int top = 1;
    while (top >= 0) {
        int h = stack[top--];
        int l = stack[top--];

        /* partition: */
        size_t x = intervals[h].lower_index; /* pivot */
        int i = l-1;
        for(int j = l; j < h; j++) {
            if(intervals[j].lower_index <= x) {
                i++;
                interval_t tmp = intervals[i];
                intervals[i] = intervals[j];
                intervals[j] = tmp;
            }
        }
        interval_t tmp = intervals[i+1];
        intervals[i+1] = intervals[h];
        intervals[h] = tmp;

        if (i > l) {
            stack[++top] = l;
            stack[++top] = i;
        }
        if (i+2 < h) {
            stack[++top] = i+2;
            stack[++top] = h;
        }
    }
    free(stack);
}


void get_sorted_iteratively_merged_interval_list_by_cutoff_step(float* data, size_t len, float cutoff_step, interval_t** intervals_out, size_t* nr_intervals_out) {
    assert(cutoff_step < 1.0f && cutoff_step > 0.0f);

    interval_t* base_intervals, *iteration_intervals, *merge_intervals, *chunk_intervals;
    size_t nr_base, nr_it, nr_merge, nr_chunk;

    find_intervals_above_cutoff(data, len, 1.0f, &base_intervals, &nr_base);
    /*printf("Base: \n");
    print_intervals(base_intervals, nr_base);*/

    size_t nr_iterations = floor(1/cutoff_step)+1;
    for(int i = 1; i < nr_iterations; i++) {
        float cutoff = fmaxf(1.0f-cutoff_step*i, 0.0f);

        /*printf("Iteration %i: \n", i); */
        find_intervals_above_cutoff(data, len, cutoff, &iteration_intervals, &nr_it);
        /*print_intervals(iteration_intervals, nr_it);*/


        /*printf("tmp Chunk %i: \n", i);*/
        chunk_interval_list(iteration_intervals, nr_it, &chunk_intervals, &nr_chunk, 5);
        /*print_intervals(chunk_intervals, nr_chunk);*/

        /*printf("Merge %i: \n", i);*/
        merge_interval_lists(base_intervals, nr_base, chunk_intervals, nr_chunk, &merge_intervals, &nr_merge);
        /*print_intervals(merge_intervals, nr_merge);
        print_interval_distances(merge_intervals, nr_merge);*/

        /*
        printf("Sort %i: \n", i);
        sort_interval_lists(merge_intervals, nr_merge);
        print_intervals(merge_intervals, nr_merge);
        */

        free(chunk_intervals);
        free(base_intervals);
        free(iteration_intervals);

        base_intervals = merge_intervals;
        nr_base = nr_merge;
    }

    if(nr_intervals_out != NULL)
        nr_intervals_out[0] = nr_base;
    if(intervals_out != NULL)
        intervals_out[0] = base_intervals;
}










/*

- Midpoints instead of whole intervals
- interpolate between two steps to add only one of two intervals which merge very fast (are only different by small dB)
- have minimal Hz or log2(Hz) distance that is necessary to register as a different sound (maximal note resolution, say 1/8-Note?)

- on a kind of come-shape, where would be the "midpoint"?

 */

