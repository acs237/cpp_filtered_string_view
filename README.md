# cpp_filtered_string_view

# Motivation behind this project
When creating a function to take a (constant) string as an argument, programmers often opt for one of the following ways:

```cpp
// C Convention
void takes_char_star(const char *s);

// Old Standard C++ convention
void takes_string(const std::string &s);
```

These work fine when a caller has the string in the format already provided, but what happens when a conversion is needed (either from const char * to `std::string` or `std::string` to `const char *`)?
Callers needing to convert a `std::string` to a `const char *` need to use the (efficient and inconvenient) `c_str()` function:

```cpp
void already_had_string(const std::string &s) {
  takes_char_star(s.c_str()); // explicit conversion
}
```


Callers needing to convert a `const char *` to a `std::string` don’t need to do anything additional (the good news) but will invoke the creation of a (convenient but inefficient) temporary string, copying the contents of that string (the bad news):

```cpp
void already_has_char_star(const char *s) {
  takes_string(s); // compiler will make a copy
}
```

The solution is a `string_view`. `string_view` has implicit conversion constructors from both `const char *` and `const std::string &`, and since `string_view` doesn’t copy, there is no `O(n)` memory penalty for making a hidden copy. In the case where a `const std::string &` is passed, the constructor runs in `O(1)` time. In the case where a `const char *` is passed, the constructor will calculate the length for you (or you can use the two-parameter `string_view` constructor which takes in the length of the string).

# About the project
A `filtered_string_view` is like a `string_view`, however it presents a filtered view of the underlying data. This means that readers of the `filtered_string_view` may only see part of the underlying data.
The filter is optionally provided by the caller as a predicate (a function which returns a boolean) which returns `true` if the data is to be kept. If not provided, the filter will default to the "true" predicate, i.e., a function which always returns true. In this case, no data would be filtered.

The iterator is also provided to traverse through the `filtered_string_view` which will be a bidirectional `const_iterator` (as a `string_view` provides a read only view into its underlying data).


# what concepts applied

# what technology (functionality of C++) applied

# what you gained from this project
