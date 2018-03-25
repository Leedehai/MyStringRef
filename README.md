# MyStringRef

My play with C++17's `string_ref` proposal, with own extensions like `edit_distance` (Levenshtein distance). Taken from a larger project of mine.

Test (with GoogleTest, already supplied)
```
cd unit-tests
make
./string-ref-test
```

Linux required.

Note that the makefile's content is excerpted from Make's echoing while executing the makefile in the original larger project's `/unit-tests`. You should modify it if necessary.
