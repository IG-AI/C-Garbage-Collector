# This are the results from our performance tests

## Running list\_bench without filling the heap
| Type of test   | Runtime (s) |
| -------------- | ----------- |
| Using Malloc   | 73.20       |
| Using IOOPM GC | 91.88       |


## Running list\_bench with 1.2 as size modifier (filling heap)
Unable to run as the program crashes after inserting more after removing
the root ptrs. No time for fix


## Trying to insert 100 000 objects with gc\_perf\_test run h_gc() and search 10 000 times
| Type of test   | Runtime (s) |
| -------------- | ----------- |
| Using Malloc   | 8.82        |
| Using IOOPM GC | 88.35       |

## Compare with BDW GC
We were unable to get BDW GC to work so we couldn't compare it with our other
results.
