
# testing bubble sort
g++ -O5 -I../lib -DBUBBLE_SORT -o bubble_sort main.cpp
bubble_sort ../chart_printer/bubble_sort.chart

# testing selection sort
g++ -O5 -I../lib -DSELECTION_SORT -o selection_sort main.cpp
selection_sort ../chart_printer/selection_sort.chart

# testing insertion sort
g++ -O5 -I../lib -DINSERTION_SORT -o insertion_sort main.cpp
insertion_sort ../chart_printer/insertion_sort.chart

# testing merge sort
g++ -O5 -I../lib -DMERGE_SORT -o merge_sort main.cpp
merge_sort ../chart_printer/merge_sort.chart
