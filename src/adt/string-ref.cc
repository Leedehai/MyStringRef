/**
 * File: string-ref.cc
 * ---------------------------
 * Implements class string_ref, a thin wrapper around a const char *.˜
 */

#include "adt/string-ref.h"
#include <iostream>
#include <vector>
#include <locale> /* std::tolower() */

bool adt::string_ref::starts_with(string_ref prefix) const {
    return prefix.len <= len
           && memCompare(ps, prefix.ps, prefix.len) == 0;
}

bool adt::string_ref::ends_with(string_ref suffix) const {
    return suffix.len <= len 
           && memCompare(end() - suffix.len, suffix.ps, suffix.len) == 0;
}

size_t adt::string_ref::edit_distance(const string_ref rhs,
                                      bool case_sensitive) const {
    const string_ref &s1 = *this;
    const string_ref &s2 = rhs;
    size_t len1 = s1.size(), len2 = s2.size();
    if (len1 == 0 || len2 == 0) { return len1 + len2; }
    std::vector<size_t> dp(len2 + 1, 0), dpPrev(len2 + 1, 0); /* dynamic programming */
    for (size_t j = 1; j <= len2; j++) { dp[j] = j; }
    for (size_t i = 1; i <= len1; i++) {
        swap(dp, dpPrev); // O(1)
        dp[0] = i;
        for (size_t j = 1; j <= len2; j++) {
            if (case_sensitive ? s1[i - 1] == s2[j - 1]
                : std::tolower(s1[i - 1]) == std::tolower(s2[j - 1])) {
                    dp[j] = dpPrev[j - 1];
            }
            else {
                dp[j] = std::min(dpPrev[j - 1] + 1,
                        std::min(dp[j - 1] + 1, dpPrev[j] + 1));
            }
        }
    }
    return dp[len2];
}

size_t adt::string_ref::find_char(char c, size_t start) const {
    if (start >= len) { return npos; }
    for (size_t i = start; i < len; ++i) {
        if (c == ps[i]) { return i; }
    }
    return npos;
}

size_t adt::string_ref::rfind_char(char c, size_t rstart) const {
    for (size_t i = std::min(rstart, len - 1); i >= 0; --i) {
        if (c == ps[i]) { return i; }
    }
    return npos;
}

size_t adt::string_ref::find_str(adt::string_ref pattern) const {
    if (pattern.size() == 0) { return 0; }
    /* const char *strstr(const char *haystack, const char *needle) */
    const char *found = std::strstr(ps, pattern.to_string().c_str());
    if (!found) { return npos; }
    return found - ps;
}

size_t adt::string_ref::rfind_str(adt::string_ref pattern) const {
    if (pattern.size() == 0) { return 0; }
    /* oh oh, I am lazy, so I turned to std::string. */
    size_t pos = to_string().rfind(pattern.to_string());
    if (pos == std::string::npos) { return npos; }
    return pos;
    /* Maybe I should implement using KMP algorithm, or brute-force?
     * Usage for rfind() is rare enough to not care about its efficiency. */
}

size_t adt::string_ref::find_if(std::function<bool(char)> pred, size_t start) const {
    if (start >= len) { return npos; }
    for (size_t i = start, e = len; i != e; ++i) {
        if (pred(ps[i])) { return i; }
    }
    return npos;
}

size_t adt::string_ref::rfind_if(std::function<bool(char)> pred, size_t rstart) const {
    for (size_t i = std::min(rstart, len - 1); i >= 0; --i) {
        if (pred(ps[i])) { return i; }
    }
    return npos;
}

size_t adt::string_ref::count_char(char c) const {
    size_t count = 0;
    for (size_t i = 0, e = len; i != e; ++i) { /* plays with cache locality */
        if (ps[i] == c) { ++count; }
    }
    return count;
}

size_t adt::string_ref::count_str(adt::string_ref pattern) const {
    size_t patternLen = pattern.len;
    if (patternLen > len) { return 0; }
    size_t count = 0;
    for (size_t i = 0, e = len - patternLen + 1; i != e; ++i) {
        if (substr(i, patternLen).equals(pattern)) { ++count; }
    }
    return count;
}

std::pair<adt::string_ref, adt::string_ref> adt::string_ref::split(char sep) const {
    size_t pos = find(sep);
    return pos == npos ? std::make_pair(*this, string_ref())
                       : std::make_pair(slice(0, pos), slice(pos + 1, len));
}

std::pair<adt::string_ref, adt::string_ref> adt::string_ref::split(string_ref sep) const {
    size_t pos = find(sep);
    return pos == npos ? std::make_pair(*this, string_ref())
                       : std::make_pair(slice(0, pos), slice(pos + sep.size(), len));
}

std::pair<adt::string_ref, adt::string_ref> adt::string_ref::rsplit(char sep) const {
    size_t pos = rfind(sep);
    return pos == npos ? std::make_pair(*this, string_ref())
                       : std::make_pair(slice(0, pos), slice(pos + 1, len));
}

std::pair<adt::string_ref, adt::string_ref> adt::string_ref::rsplit(string_ref sep) const {
    size_t pos = rfind(sep);
    return pos == npos ? std::make_pair(*this, string_ref())
                       : std::make_pair(slice(0, pos), slice(pos + sep.size(), len));
}
