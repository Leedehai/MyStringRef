/**
 * File: string-ref.h
 * ---------------------------
 * Exports class string_ref, a thin wrapper around a const char *.
 * This wrapper provides a non-mutable access to a memory chunk and some basic
 * non-mutable operations (i.e. operations that do not alter the characters
 * stored in that memory piece). Credit: C++17 N3921 and LLVM.
 */

#ifndef STRING_REF_H
#define STRING_REF_H

#include <cstring>
#include <string>
#include <cassert>
#include <algorithm>  /* std::min(), std::max() */
#include <functional> /* std::hash<>, std::function<> */
#include <iostream>   /* std::ostream */

namespace adt {
    /* Stack-allocated string, useful for short strings */
    class string_ref;

    /* Converts to lower case. ctype.h's tolower() only works with a single 
     * character. Note that the C-string must be mutable (NOT const char *) and
     * null-terminated. */
    inline void cstr_tolower(char *pstr) {
        for (char *p = pstr; *p; ++p) { 
            *p = (*p >= 'A' && *p <= 'Z') ? (*p | 0x60) : *p;
        }
    }
}

class adt::string_ref {
public:
    /* largest size_t value, itentionally underflow this unsigned integer */
    static const size_t npos = (size_t)-1;
    static const size_t capacity = npos;
    /* (random access) iterator: a vanilla pointer */
    typedef const char *iterator;
    /* size_type: STL has it, so string_ref mimics it */
    typedef size_t size_type;

    /**
     * Functor for hashing, needed in templates like std::unordered_map<>.
     * Usage: size_t hashval = adt::string_ref::Hash{}(s);
     *        std::unordered_map<adt::string_ref, Value, adt::string_ref::Hash> m;
     */
    struct Hash {
        size_t operator()(const string_ref s) const {
            return std::hash<std::string>{}(std::string(s.ptr(), s.size()));
        }
    };

    /**
     * Constructors.
     * Usage: string_ref(); string_ref("abc"); string_ref(char_ptr);
     *        string_ref(std_string); string_ref(another_string_ref);
     *        string_ref(char_ptr).withNullAsEmpty();
     * ---------------------------
     * Constructs a string_ref instance from a C-string (i.e. a pointer),
     * from a std::string instance (c_str() returns a pointer to 
     * a null-terminated string).
     * When the pointer is unkown (may be NULL), make the string_ref to be
     * empty (on memory, it's just a [\0]) with withNullAsEmpty() when the
     * pointer is NULL.
     */
    /* Default, returns an empty reference, points to NULL */
    string_ref() : ps(nullptr), len(0) {}
    /* Accepts a const char *, e.g. a character or C-string literal */
    string_ref(const char *str) : ps(str), len(str ? std::strlen(str) : 0) {}
    string_ref(const char *str, size_t len) : ps(str), len(len) {}
    /* Accepts a std::string instance, allowing std::string to be passed to
     * an argument that is of type string_ref. NOTE this string_ref's lifetime
     * should be no longer than that std::string, or s.c_str() is invalidated */
    string_ref(const std::string &s) : ps(s.c_str()), len(s.size()) {}
    string_ref(const std::string &s, size_t n)
    : ps(s.c_str()), len(n < s.size() ? n : s.size()) {}
    /* Static method. Accepts a pointer (but unsure if it is not NULL).
     * Usage: string_ref sr = adt::string_ref::withNullAsEmpty(ptr); */
    static string_ref withNullAsEmpty(const char *ptr) {
        return string_ref(ptr ? ptr : "");
    }
    /* copy constructor/assignment: shallow copy, like pointer */
    string_ref(const string_ref &s) = default;
    string_ref &operator=(const string_ref &) = default;
    /* move constructor/assignment (invalidates the original) */
    string_ref(string_ref &&) = default;
    string_ref &operator=(string_ref &&) = default;
    /* throw an error if a NULL pointer is passed */
    string_ref(std::nullptr_t nullPointer) = delete;
    string_ref(std::nullptr_t nullPointer, size_t anyNumber) = delete;
    /**
     * Some basic utility methods.
     * Usage: the same as std::string's methods.
     * ---------------------------
     * The interface is just ike std::string.
     */
    bool empty() const { return len == 0; }
    /* Get the pointer. NOTE ps[len] is NOT guaranteed to be '\0' */
    const char *ptr() const { return ps; }   /* std::string s(sr.ptr()); */
    /* Convert to a std::string object. NOTE it makes a deep copy. */
    std::string to_string() const { return std::string(ps, len); }
    /* two equivalent methods, for readability in different contexts */
    size_t length() const { return len; }   /* trailing '\0' not counted */
    size_t size() const { return len; }     /* trailing '\0' not counted */
    /* iterators: iterators are equivalent to constant iterators */
    iterator begin() const { return ps; }
    iterator cbegin() const { return ps; }
    iterator end() const { return ps + len; }
    iterator cend() const { return ps + len; }
    char front() const {
        assert(!empty() && "front() was called on an empty instance.");
        return ps[0];
    }
    char back() const {
        assert(!empty() && "back() was called on an empty instance.");
        return ps[len-1];
    }
    bool equals(string_ref rhs) const {
        return len == rhs.len && memCompare(ps, rhs.ps, len) == 0;
    }
    
    /**
     * Method: compare()
     * Usage: lhs.compare(rhs)
     * ---------------------------
     * Returns 0 if match; -1 if lhs is lexicographically lower; 1 otherwise.
     * NOTE that ps[len] is NOT guatanteed to be '\0'.
     */
    int compare(string_ref rhs) const {
        int comp = memCompare(ps, rhs.ps, std::min(len, rhs.len));
        if (comp != 0) { return comp < 0 ? -1 : 1; }
        if (len == rhs.len) { return 0; }
        return len < rhs.len ? -1 : 1;
    }

    /**
     * Method: operator[](), overloading operator
     * Usage: adt::string_ref sr("abc"); char c = sr[2]; --> c is 'c'
     * ---------------------------
     * Returns the character on the given position.
     */
    char operator[](size_t pos) const {
        assert(pos < len && "Out of range: invalid index on the string.");
        return ps[pos];
    }

    /**
     * Methods: see the prototypes
     * ---------------------------
     * Miscellaneous non-mutable methods.
     * NOTE that std::function<> can take a lambda, a function pointer, a 
     * function object class, or a function object.
     */
    /* Checks if it starts with a prefix, returns bool */
    bool starts_with(string_ref prefix) const;

    /* Checks if it ends with a suffix, returns bool */
    bool ends_with(string_ref suffix) const;

    /* Checks if it contains a character, returns bool */
    bool contains(char c) const {
        return find(c) != npos;
    }
    
    /* Checks if it contains a substring that matches a pattern, returns bool */
    bool contains(string_ref pattern) const {
        return find_str(pattern) != npos;
    }

    /* Returns edit distance (Levenshtein distance) */
    size_t edit_distance(const string_ref rhs, bool case_sensitive = true) const;

    /* Searches for a character, returns index if found, else npos. */
    size_t find(char c, size_t start = 0) const { 
        return find_char(c, start);
    }
    size_t find_char(char c, size_t start = 0) const;

    /* Searches for a character reversely, returns index if found, else npos */
    size_t rfind(char c, size_t rstart = npos) const {
        return rfind_char(c, rstart);
    }
    size_t rfind_char(char c, size_t rstart = npos) const;

    /* Searches for a substring (needle) that matches a pattern, returns the 
     * first index of that substring if found in *this (haystack) else npos.
     * Return 0 if pattern is empty, i.e. *(pattern.ps) == '\0' */
    size_t find(string_ref pattern) const {
        return find_str(pattern);
    }
    size_t find_str(string_ref pattern) const;

    /* Searches for a substring (needle) that matches a pattern reversely, returns the 
     * first index of that substring if found in *this (haystack), else npos */
    size_t rfind(string_ref pattern) const {
        return rfind_str(pattern);
    }
    size_t rfind_str(string_ref pattern) const;

    /* Searches with a predicate (can be a lambda), returns the first index 
     * that is found to be true, else npos */
    size_t find_if(std::function<bool(char)> pred, size_t start = 0) const;

    /* Searches with a predicate (can be a lambda), returns the first index
     * that is found to be false, else npos */
    size_t find_if_not(std::function<bool(char)> pred, size_t start = 0) const {
        return find_if([pred](char c){ return !pred(c); }, start);
    }

    /* Searches reversely with a predicate (can be a lambda), returns the first 
     * index that is found to be true, else npos */
    size_t rfind_if(std::function<bool(char)> pred, size_t rstart = npos) const;

    /* Searches reversely with a predicate (can be a lambda), returns the first
     * index that is found to be false, else npos */
    size_t rfind_if_not(std::function<bool(char)> pred, size_t rstart = npos) const {
        return rfind_if([pred](char c){ return !pred(c); }, rstart);
    }

    /* Counts occurrence of a character, returns size_t */
    size_t count_char(char c) const;

    /* Counts occurrence of a pattern (might overlaps), returns size_t */
    size_t count_str(string_ref pattern) const;

    /* Shallow-copies a string_ref, but only keeps the element between the
     * range [start, start + num) INTERSECT [0, len). */
    string_ref substr(size_t start, size_t num = capacity) const {
        start = std::min(start, len);
        return string_ref(ps + start, std::min(num, len - start));
    }

    /* Shallow-copies a string_ref, but only keeps the element between the
     * range [start, end) INTERSECT [0, len). */
    string_ref slice(size_t start, size_t end) const {
        if (start > end) { std::swap(start, end); }
        start = std::min(start, len);
        end = std::min(std::max(start, end), len);
        return string_ref(ps + start, end - start);
    }
    
    /* Shallow-copies a string_ref, but only keeps the first AT MOST n elements */
    string_ref take_front(size_t n = 1) const {
        return string_ref(ps, std::min(n, len));
    }

    /* Shallow-copies a string_ref, but only keeps the first consecutive elements
     * that are all found to be true */
    string_ref take_front_while(std::function<bool(char)> pred) const {
        return substr(0, find_if_not(pred));
    }

    /* Shallow-copies a string_ref, but only keeps the last AT MOST n elements */
    string_ref take_back(size_t n = 1) const {
        return (n <= len) ? drop_front(len - n) : *this;
    }

    /* Shallow-copies a string_ref, but drops the first EXACTLY n elements */
    string_ref drop_front(size_t n = 1) const {
        assert(n <= len && "Dropping more characters than exist.");
        return substr(n);
    }

    /* Shallow-copies a string_ref, but drops the last EXACTLY n elements */
    string_ref drop_back(size_t n = 1) const {
        assert(n <= len && "Dropping more characters than exist.");
        return substr(0, len - n);
    }

    /* Shallow-copies two string_ref s1 and s2, such that s1 + sep + s2 is the
     * original string_ref. If sep is not found, s1 is original and s2 is empty.
     * The position of splitting is at the FIRST occurrence of sep. */
    std::pair<string_ref, string_ref> split(char sep) const;
    std::pair<string_ref, string_ref> split(string_ref sep) const;
    /* Shallow-copies two string_ref s1 and s2, such that s1 + sep + s2 is the
     * original string_ref. If sep is not found, s1 is original and s2 is empty.
     * The position of splitting is at the LAST occurrence of sep. */
    std::pair<string_ref, string_ref> rsplit(char sep) const;
    std::pair<string_ref, string_ref> rsplit(string_ref sep) const;

private:
    const char *ps;   /* 1-word size */
    size_t len;       /* 1-word size, trailing '\0' NOT counted */
    static int memCompare(const char *lhs, const char *rhs, size_t len) {
        if (len == 0) { return 0; } /* here memcmp(lhs,rhs,len) undefined. */
        return std::memcmp(lhs, rhs, len);
    }
};

/* Operater overloading. No need to give the namespace qualifier when using
 * them, thanks to argument-dependent lookup (ADL) */
namespace adt {
    inline bool operator==(string_ref lhs, string_ref rhs) {
        return lhs.equals(rhs);
    }
    inline bool operator!=(string_ref lhs, string_ref rhs) {
        return !lhs.equals(rhs);
    }
    inline bool operator<(string_ref lhs, string_ref rhs) {
        return lhs.compare(rhs) < 0;
    }
    inline bool operator<=(string_ref lhs, string_ref rhs) {
        return lhs.compare(rhs) <= 0;
    }
    inline bool operator>(string_ref lhs, string_ref rhs) {
        return lhs.compare(rhs) > 0;
    }
    inline bool operator>=(string_ref lhs, string_ref rhs) {
        return lhs.compare(rhs) >= 0;
    }
    inline std::string operator+(std::string stdstr, string_ref sr) {
        std::string res = stdstr;
        return res.append(sr.ptr(), sr.size());
    }
    inline std::string operator+=(std::string stdstr, string_ref sr) {
        return stdstr.append(sr.ptr(), sr.size());
    }
    inline std::ostream &operator<<(std::ostream &os, string_ref sr) {
        os << std::string(sr.ptr(), sr.size());
        return os;
    }
    inline size_t edit_distance(const string_ref lhs, const string_ref rhs,
                                bool case_sensitive = true) {
        return lhs.edit_distance(rhs, case_sensitive);
    }
}

#endif