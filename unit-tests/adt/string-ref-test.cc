/**
 * File: string-ref-test.cc
 * ---------------------------
 * Test driver for class string_ref.
 */

#include "adt/string-ref.h"
#include <gtest/gtest.h>
using namespace adt;

TEST(StringRefTest, AccessorGroup1) {
    string_ref srEmpty;
    EXPECT_TRUE(srEmpty.empty());
    EXPECT_TRUE(srEmpty.equals(""));
    EXPECT_FALSE(srEmpty.equals("abc"));
    string_ref sr("abc");
    EXPECT_STREQ("abc", sr.ptr());
    EXPECT_EQ(3, sr.length());
    EXPECT_EQ(3, sr.size());
    EXPECT_EQ('a', *sr.begin());
    EXPECT_EQ('c', *(sr.end()-1));
    EXPECT_EQ('a', sr.front());
    EXPECT_EQ('c', sr.back());
    EXPECT_TRUE(sr.equals("abc"));
    EXPECT_FALSE(sr.equals("ab"));
    EXPECT_FALSE(sr.equals("abd"));
    EXPECT_FALSE(sr.equals(""));
    EXPECT_TRUE(sr.compare("abd") == -1);
    EXPECT_TRUE(sr.compare("abb") == 1);
    EXPECT_TRUE(sr.compare("abc") == 0);
    EXPECT_TRUE(sr.compare("abcd") == -1);
    EXPECT_TRUE(sr.compare("ab") == 1);
    EXPECT_EQ('b', sr[1]);
}

TEST(StringRefTest, AccessorGroup2) {
    std::string s("abc");
    string_ref sr(s);
    string_ref srEmpty(s, 0);
    EXPECT_TRUE(srEmpty.empty());
    EXPECT_TRUE(srEmpty.equals(""));
    EXPECT_FALSE(srEmpty.equals("abc"));
    EXPECT_TRUE(sr.starts_with("ab"));
    EXPECT_FALSE(sr.starts_with("ac"));
    EXPECT_TRUE(sr.starts_with(""));
    EXPECT_TRUE(sr.ends_with("bc"));
    EXPECT_FALSE(sr.ends_with("dc"));
    EXPECT_TRUE(sr.ends_with(""));
    EXPECT_TRUE(sr.contains('a'));
    EXPECT_TRUE(sr.contains("ab"));
    EXPECT_FALSE(sr.contains('d'));
    EXPECT_FALSE(sr.contains("bd"));
    EXPECT_EQ(1, sr.find('b'));
    EXPECT_EQ(1, sr.find("bc"));
    EXPECT_EQ(1, sr.rfind('b'));
    EXPECT_EQ(1, sr.rfind("bc"));
    EXPECT_EQ(1, sr.find_if([](char c){ return c > 'a'; }));
    EXPECT_EQ(1, sr.find_if_not([](char c){ return c <= 'a'; }));
    EXPECT_EQ(1, sr.rfind_if([](char c){ return c < 'c'; }));
    EXPECT_EQ(1, sr.rfind_if_not([](char c){ return c >= 'c'; }));
    EXPECT_EQ(1, sr.count_char('b'));
    EXPECT_EQ(1, sr.count_str("bc"));
    string_ref sr2("abcab");
    EXPECT_EQ(2, sr2.count_char('b'));
    EXPECT_EQ(2, sr2.count_str("ab"));
}

TEST(StringRefTest, EditDistance) {
    string_ref s0(""), s1("sea");
    EXPECT_EQ(0, s0.edit_distance(""));
    EXPECT_EQ(2, s1.edit_distance("eat"));
    EXPECT_EQ(2, s1.edit_distance("eat", true));
    EXPECT_EQ(2, edit_distance("sea", "eat"));
    EXPECT_EQ(2, edit_distance("sea", "eat", true));
    EXPECT_EQ(1, edit_distance("abcd", "abce"));
    EXPECT_EQ(0, edit_distance("abcd", "abcd"));
    EXPECT_EQ(0, edit_distance("abcd", "abcd", true));
    EXPECT_EQ(1, edit_distance("Abcd", "abcd"));
    EXPECT_EQ(1, edit_distance("Abcd", "abcd", true));
    EXPECT_EQ(0, edit_distance("Abcd", "abcd", false));
    EXPECT_EQ(1, edit_distance("aaa", "a0aa"));
    EXPECT_EQ(1, edit_distance("aaa", "a,aa"));
    EXPECT_EQ(1, edit_distance("aaa", "abaa"));
    EXPECT_EQ(2, edit_distance("aaccb", "aab"));
    EXPECT_EQ(1, edit_distance("baaa", "aaa"));
    EXPECT_EQ(8, edit_distance("same", "different"));
}

TEST(StringRefTest, SubString) {
    string_ref sr("abcdefgh");
    // [start, start + num) INTERSECT [0, len)
    EXPECT_STREQ("cdef", sr.substr(2,4).to_string().c_str());
    EXPECT_STREQ("cdefgh", sr.substr(2,100).to_string().c_str());
    EXPECT_STREQ("cdefgh", sr.substr(2).to_string().c_str());
    EXPECT_STREQ("cdef", sr.substr(2, /*num*/4).to_string().c_str());
    // [start, end) INTERSECT [0, len)
    EXPECT_STREQ("cdef", sr.slice(2, 6).to_string().c_str());
    EXPECT_STREQ("", sr.slice(2, 2).to_string().c_str());
    EXPECT_STREQ("cdef", sr.slice(6,2).to_string().c_str());
}

TEST(StringRefTest, TakeAndDrop) {
    string_ref sr("abcdefgh");
    EXPECT_STREQ("a", sr.take_front().to_string().c_str());
    EXPECT_STREQ("ab", sr.take_front(2).to_string().c_str());
    EXPECT_STREQ("abcdefgh", sr.take_front(100).to_string().c_str());
    EXPECT_STREQ("", sr.take_front_while([](char c){ return c < 'a'; }).to_string().c_str());
    EXPECT_STREQ("ab", sr.take_front_while([](char c){ return c < 'c'; }).to_string().c_str());
    EXPECT_STREQ("h", sr.take_back().to_string().c_str());
    EXPECT_STREQ("gh", sr.take_back(2).to_string().c_str());
    EXPECT_STREQ("abcdefgh", sr.take_back(100).to_string().c_str());
    EXPECT_STREQ("bcdefgh", sr.drop_front().to_string().c_str());
    EXPECT_STREQ("abcdefgh", sr.drop_front(0).to_string().c_str());
    EXPECT_STREQ("cdefgh", sr.drop_front(2).to_string().c_str());
    EXPECT_STREQ("abcdefgh", sr.drop_back(0).to_string().c_str());
    EXPECT_STREQ("abcdef", sr.drop_back(2).to_string().c_str());
}

TEST(StringRefTest, Split) {
    string_ref sr("abcdefabgh");
    // delimiter is found
    auto srPair1 = sr.split('c');
    EXPECT_STREQ("ab", srPair1.first.to_string().c_str());
    EXPECT_STREQ("defabgh", srPair1.second.to_string().c_str());
    auto srPair2 = sr.split('a');
    EXPECT_STREQ("", srPair2.first.to_string().c_str());
    EXPECT_STREQ("bcdefabgh", srPair2.second.to_string().c_str());
    auto srPair3 = sr.split('h');
    EXPECT_STREQ("abcdefabg", srPair3.first.to_string().c_str());
    EXPECT_STREQ("", srPair3.second.to_string().c_str());
    auto srPair4 = sr.split("cd");
    EXPECT_STREQ("ab", srPair4.first.to_string().c_str());
    EXPECT_STREQ("efabgh", srPair4.second.to_string().c_str());
    auto srPair5 = sr.split("ab");
    EXPECT_STREQ("", srPair5.first.to_string().c_str());
    EXPECT_STREQ("cdefabgh", srPair5.second.to_string().c_str());
    auto srPair6 = sr.split("gh");
    EXPECT_STREQ("abcdefab", srPair6.first.to_string().c_str());
    EXPECT_STREQ("", srPair6.second.to_string().c_str());
    auto srPair7 = sr.rsplit('a');
    EXPECT_STREQ("abcdef", srPair7.first.to_string().c_str());
    EXPECT_STREQ("bgh", srPair7.second.to_string().c_str());
    auto srPair8 = sr.rsplit('h');
    EXPECT_STREQ("abcdefabg", srPair8.first.to_string().c_str());
    EXPECT_STREQ("", srPair8.second.to_string().c_str());
    auto srPair9 = sr.rsplit("gh");
    EXPECT_STREQ("abcdefab", srPair9.first.to_string().c_str());
    EXPECT_STREQ("", srPair9.second.to_string().c_str());
    auto srPair10 = sr.rsplit("ab");
    EXPECT_STREQ("abcdef", srPair10.first.to_string().c_str());
    EXPECT_STREQ("gh", srPair10.second.to_string().c_str());
    auto srPair11 = sr.rsplit("gh");
    EXPECT_STREQ("abcdefab", srPair11.first.to_string().c_str());
    EXPECT_STREQ("", srPair11.second.to_string().c_str());
    // delimiter is not found
    auto srPairz1 = sr.split('z');
    EXPECT_STREQ("abcdefabgh", srPairz1.first.to_string().c_str());
    EXPECT_STREQ("", srPairz1.second.to_string().c_str());
    auto srPairz2 = sr.split("z");
    EXPECT_STREQ("abcdefabgh", srPairz2.first.to_string().c_str());
    EXPECT_STREQ("", srPairz2.second.to_string().c_str());
    auto srPairz3 = sr.rsplit('z');
    EXPECT_STREQ("abcdefabgh", srPairz3.first.to_string().c_str());
    EXPECT_STREQ("", srPairz3.second.to_string().c_str());
    auto srPairz4 = sr.rsplit("z");
    EXPECT_STREQ("abcdefabgh", srPairz4.first.to_string().c_str());
    EXPECT_STREQ("", srPairz4.second.to_string().c_str());
}

TEST(StringRefTest, OperatorOverloading) {
    EXPECT_TRUE(string_ref("abc") == string_ref("abc"));
    EXPECT_TRUE(string_ref("abc") != string_ref("abd"));
    EXPECT_TRUE(string_ref("abc") < string_ref("abd"));
    EXPECT_TRUE(string_ref("abc") < string_ref("abdc"));
    EXPECT_TRUE(string_ref("abc") <= string_ref("abdc"));
    EXPECT_TRUE(string_ref("abc") > string_ref("ab"));
    EXPECT_TRUE(string_ref("abc") > string_ref("abad"));
    EXPECT_TRUE(string_ref("abc") >= string_ref("abad"));
    std::string s("abc");
    EXPECT_STREQ("abcqw", (s+"qw").c_str());
    EXPECT_STREQ("abcde", (s+="de").c_str());
    string_ref sr(s,2);
    EXPECT_STREQ("abcdeab", (s+=sr).c_str());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}