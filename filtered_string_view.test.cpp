#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>

TEST_CASE("fsv default constructor") {
	auto const fsv1 = fsv::filtered_string_view{};
	auto const size = fsv1.size();
	CHECK(size == 0);
}

TEST_CASE("fsv implicit string constructor") {
	auto const s = std::string{"cat"};
	SECTION("when calling constructor explicitly") {
		auto const fsv1 = fsv::filtered_string_view{s};
		auto const size = fsv1.size();
		CHECK(size == 3);
		CHECK(fsv1.data() == s);
	}
	SECTION("implicit constructor call") {
		fsv::filtered_string_view const fsv1 = s;
		auto const size = fsv1.size();
		CHECK(size == 3);
		CHECK(fsv1.data() == s);
	}
}

TEST_CASE("fsv string constructor with predicate") {
	auto const s = std::string{"cat"};
	auto const pred = [](const char& c) { return c == 'a'; };
	auto const fsv1 = fsv::filtered_string_view{s, pred};
	auto const size = fsv1.size();
	CHECK(size == 1);
	CHECK(fsv1.data() == s);
}

TEST_CASE("fsv implicit null-terminated string constructor") {
	auto const s = "cat";
	SECTION("when calling constructor explicitly") {
		auto const fsv1 = fsv::filtered_string_view{s};
		auto const size = fsv1.size();
		CHECK(size == 3);
		CHECK(fsv1.data() == s);
	}
	SECTION("implicit constructor call") {
		fsv::filtered_string_view const fsv1 = s;
		auto const size = fsv1.size();
		CHECK(size == 3);
		CHECK(fsv1.data() == s);
	}
}

TEST_CASE("fsv null-terminated string with predicate constructor") {
	auto pred = [](const char& c) { return c == 'a'; };
	auto const s = "cat";
	auto const fsv1 = fsv::filtered_string_view{s, pred};
	auto const size = fsv1.size();
	CHECK(size == 1);
	CHECK(fsv1.data() == s);
}

TEST_CASE("copy constructor") {
	auto const fsv1 = fsv::filtered_string_view{"bulldog"};
	auto const copy = fsv1;
	CHECK(copy.data() == fsv1.data());
	CHECK(copy.size() == fsv1.size());
}

TEST_CASE("copy assignment") {
	auto const pred = [](const char& c) { return c == '4' or c == '2'; };
	auto const fsv1 = fsv::filtered_string_view{"42 bro", pred};
	auto fsv2 = fsv::filtered_string_view{};
	fsv2 = fsv1;
	CHECK(fsv1.size() == fsv2.size());
	CHECK(fsv1 == fsv2);
}

TEST_CASE("move constructor") {
	auto fsv1 = fsv::filtered_string_view{"bulldog"};
	auto const data = "bulldog";
	auto const move = std::move(fsv1);

	// check for moved-from object
	CHECK(fsv1.data() == nullptr);
	CHECK(fsv1.size() == 0);

	// check for moved-to object
	CHECK(move.data() == data);
	CHECK(move.size() == 7);
}

TEST_CASE("move assignment") {
	auto const pred = [](const char& c) { return c == '8' or c == '9'; };
	auto fsv1 = fsv::filtered_string_view{"'89 baby", pred};
	auto fsv2 = fsv::filtered_string_view{};

	fsv2 = std::move(fsv1);

	// check for moved-from object
	CHECK(fsv1.size() == 0);
	CHECK(fsv1.data() == nullptr);

	// check for moved-to object
	auto const str = "'89 baby";
	CHECK(fsv2.size() == 2);
	CHECK(fsv2.data() == str);
}

TEST_CASE("subscript") {
	auto const pred = [](const char& c) { return c == '9' or c == '0' or c == ' '; };
	auto const fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};

	// with negative index
	CHECK_THROWS(fsv1[-1]);

	// with index larger than actual size
	CHECK_THROWS(fsv1[5]);

	// with valid index
	CHECK(fsv1[0] == ' ');
	CHECK(fsv1[1] == '9');
	CHECK(fsv1[2] == '0');
	CHECK(fsv1[3] == ' ');
}

TEST_CASE("string type conversion") {
	SECTION("default constructor") {
		auto const fsv = fsv::filtered_string_view{};
		CHECK(static_cast<std::string>(fsv) == "");
	}
	SECTION("a constructor with empty string") {
		auto const fsv = fsv::filtered_string_view{""};
		CHECK(static_cast<std::string>(fsv) == "");
	}
	SECTION("a constructor with a string without predicate") {
		auto const fsv = fsv::filtered_string_view{"mama"};
		CHECK(static_cast<std::string>(fsv) == "mama");
	}
	SECTION("a constructor with a string with a predicate") {
		auto const predicate = [](const char& c) { return c == 'b'; };
		auto const fsv = fsv::filtered_string_view{"bob", predicate};
		CHECK(static_cast<std::string>(fsv) == "bb");
	}
}

TEST_CASE("at") {
	SECTION("non empty string") {
		auto const vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
		auto const is_vowel = [&vowels](const char& c) { return vowels.contains(c); };
		auto const sv = fsv::filtered_string_view{"Malamute", is_vowel};

		// with negative index
		CHECK_THROWS_AS(sv.at(-1), std::domain_error);
		CHECK_THROWS_WITH(sv.at(-1), "filtered_string_view::at(-1): invalid index");

		// with index larger than actual size
		CHECK_THROWS_AS(sv.at(4), std::domain_error);
		CHECK_THROWS_WITH(sv.at(4), "filtered_string_view::at(4): invalid index");

		// with valid index
		CHECK(sv.at(0) == 'a');
		CHECK(sv.at(2) == 'u');
		CHECK(sv.at(3) == 'e');
	}
	SECTION("empty string") {
		auto const sv = fsv::filtered_string_view{""};
		CHECK_THROWS_AS(sv.at(0), std::domain_error);
		CHECK_THROWS_WITH(sv.at(0), "filtered_string_view::at(0): invalid index");
	}
	SECTION("default fsv") {
		auto const sv = fsv::filtered_string_view{};
		CHECK_THROWS_AS(sv.at(0), std::domain_error);
		CHECK_THROWS_WITH(sv.at(0), "filtered_string_view::at(0): invalid index");
	}
}

TEST_CASE("size") {
	SECTION("with default predicate") {
		auto const fsv = fsv::filtered_string_view{"Maltese"};
		CHECK(fsv.size() == 7);
	}
	SECTION("with custom predicate") {
		auto const fsv = fsv::filtered_string_view{"Cat!", [](const char& c) { return c == '!'; }};
		CHECK(fsv.size() == 1);
	}
	SECTION("with default fsv") {
		auto const fsv = fsv::filtered_string_view{};
		CHECK(fsv.size() == 0);
	}
}

TEST_CASE("empty") {
	SECTION("when the filtered string is not empty") {
		auto const sv = fsv::filtered_string_view{"Australian Shephard"};
		CHECK(sv.empty() == false);
	}
	SECTION("when the given string itself is empty") {
		auto const default_fsv = fsv::filtered_string_view{};
		CHECK(default_fsv.empty() == true);
		auto const empty_sv = fsv::filtered_string_view{""};
		CHECK(empty_sv.empty() == true);
	}
	SECTION("when the filtered string is empty") {
		auto sv = fsv::filtered_string_view{"Border Collie", [](const char&) { return false; }};
		CHECK(sv.empty() == true);
	}
}

TEST_CASE("data") {
	SECTION("with a string") {
		auto const s = "Sum 42";
		auto const fsv = fsv::filtered_string_view{s, [](const char&) { return false; }};
		CHECK(fsv.data() == s);
	}
	SECTION("default constructor") {
		auto const null = nullptr;
		auto const fsv = fsv::filtered_string_view{};
		CHECK(fsv.data() == null);
	}
}

TEST_CASE("predicate") {
	auto const print_and_return_true = [](const char&) {
		std::cout << "hi!";
		return true;
	};

	auto buffer = std::stringstream{};
	auto oldCoutBuffer = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

	// after constructing a filtered_string_view object, the predicate should not be called yet
	auto const s = fsv::filtered_string_view{"doggo", print_and_return_true};
	CHECK(buffer.str() == "");

	auto const& predicate = s.predicate();
	// calling the predicate got from member function will only invoke std::cout
	predicate(char{});

	std::cout.rdbuf(oldCoutBuffer);
	// therefore, there will be only one hi! output
	CHECK(buffer.str() == "hi!");
}

TEST_CASE("equality comparison") {
	SECTION("one with default constructor and another with empty string") {
		auto const fsv1 = fsv::filtered_string_view{};
		auto const fsv2 = fsv::filtered_string_view{""};
		CHECK(fsv1 == fsv2);
	}
	SECTION("one with empty string, one with predicate which filters out all chars") {
		auto const fsv1 = fsv::filtered_string_view{""};
		auto const pred = [](const char&) { return false; };
		auto const fsv2 = fsv::filtered_string_view{"zzz", pred};
		CHECK(fsv1 == fsv2);
	}
	SECTION("without predicate, different underlying data") {
		auto const fsv1 = fsv::filtered_string_view{"aaa"};
		auto const fsv2 = fsv::filtered_string_view{"zzz"};
		CHECK(fsv1 != fsv2);
	}
	SECTION("without predicate, same underlying data") {
		auto const fsv1 = fsv::filtered_string_view{"aaa"};
		auto const fsv2 = fsv::filtered_string_view{"aaa"};
		CHECK(fsv1 == fsv2);
	}
	SECTION("with the same predicate, same underlying data, same filtered string") {
		auto const pred1 = [](const char& c) { return c == 'b' or c == 'o'; };
		auto const fsv1 = fsv::filtered_string_view{"bacdoddb", pred1};
		auto const fsv2 = fsv::filtered_string_view{"bacdoddb", pred1};
		CHECK(fsv1 == fsv2);
	}
	SECTION("with different predicate, different underlying data, but same filtered string") {
		auto const pred1 = [](const char& c) { return c == 'b' or c == 'o'; };
		auto const pred2 = [](const char& c) { return c != 'a' and c != 'd'; };
		auto const fsv1 = fsv::filtered_string_view{"bacdoddb", pred1};
		auto const fsv2 = fsv::filtered_string_view{"baddddoddb", pred2};
		CHECK(fsv1 == fsv2);
	}
	SECTION("with different predicate, same underlying data, different filtered string") {
		auto const pred1 = [](const char& c) { return c == 'b' or c == 'o'; };
		auto const pred2 = [](const char& c) { return c != 'a'; };
		auto const fsv1 = fsv::filtered_string_view{"bacdoddb", pred1};
		auto const fsv2 = fsv::filtered_string_view{"bacdoddb", pred2};
		CHECK(fsv1 != fsv2);
	}
}

TEST_CASE("relational comparison") {
	SECTION("one with default constructor and another with empty string") {
		auto const fsv1 = fsv::filtered_string_view{};
		auto const fsv2 = fsv::filtered_string_view{""};
		CHECK(fsv1 <= fsv2);
		CHECK(fsv1 >= fsv2);
	}
	SECTION("one with empty string, one with predicate which filters out all chars") {
		auto const fsv1 = fsv::filtered_string_view{""};
		auto const pred = [](const char&) { return false; };
		auto const fsv2 = fsv::filtered_string_view{"zzz", pred};
		CHECK(fsv1 <= fsv2);
		CHECK(fsv1 >= fsv2);
	}
	SECTION("without predicate, different underlying data") {
		auto const fsv1 = fsv::filtered_string_view{"aaa"};
		auto const fsv2 = fsv::filtered_string_view{"zzz"};
		CHECK(fsv1 <= fsv2);
		CHECK(not(fsv1 >= fsv2));
	}
	SECTION("without predicate, same underlying data") {
		auto const fsv1 = fsv::filtered_string_view{"aaa"};
		auto const fsv2 = fsv::filtered_string_view{"aaa"};
		CHECK(fsv1 >= fsv2);
		CHECK(fsv2 >= fsv1);
	}
	SECTION("with different predicate, same underlying data, different filtered string") {
		auto const pred1 = [](const char& c) { return c == 'b' or c == 'o'; };
		auto const pred2 = [](const char& c) { return c != 'a'; };
		auto const fsv1 = fsv::filtered_string_view{"bacdoddb", pred1};
		auto const fsv2 = fsv::filtered_string_view{"bacdoddb", pred2};
		CHECK(fsv1 >= fsv2);
		CHECK(fsv1 > fsv2);
		CHECK(fsv2 < fsv1);
	}
}

TEST_CASE("output stream") {
	SECTION("when there is string output") {
		auto const fsv =
		    fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'c' or c == '+'; }};

		auto buffer = std::stringstream{};
		auto oldCoutBuffer = std::cout.rdbuf();
		std::cout.rdbuf(buffer.rdbuf());

		std::cout << fsv;

		std::cout.rdbuf(oldCoutBuffer);
		CHECK(buffer.str() == "c++");
	}
	SECTION("when there is no string output") {
		auto const fsv = fsv::filtered_string_view{};

		auto buffer = std::stringstream{};
		auto oldCoutBuffer = std::cout.rdbuf();
		std::cout.rdbuf(buffer.rdbuf());

		std::cout << fsv;

		std::cout.rdbuf(oldCoutBuffer);
		CHECK(buffer.str() == "");
	}
}

TEST_CASE("compose") {
	SECTION("with a list of predicates") {
		auto const best_languages = fsv::filtered_string_view{"c / c++"};
		auto const vf = std::vector<fsv::filter>{[](const char& c) { return c == 'c' or c == '+' or c == '/'; },
		                                         [](const char& c) { return c > ' '; },
		                                         [](const char&) { return true; }};

		auto const sv = fsv::compose(best_languages, vf);
		CHECK(static_cast<std::string>(sv) == "c/c++");
	}

	SECTION("with empty predicates list") {
		auto const fsv = fsv::filtered_string_view{"blah"};
		auto const vf = std::vector<fsv::filter>{};
		auto const new_fsv = fsv::compose(fsv, vf);
		CHECK(fsv == new_fsv);
	}

	SECTION("with empty underlying string") {
		SECTION("real empty underlying string") {
			auto const fsv = fsv::filtered_string_view{""};
			auto const vf = std::vector<fsv::filter>{[](const char& c) { return c == 'c' or c == '+' or c == '/'; },
			                                         [](const char& c) { return c > ' '; },
			                                         [](const char&) { return true; }};
			auto const new_fsv = fsv::compose(fsv, vf);
			CHECK(fsv == new_fsv);
		}
		SECTION("default fsv") {
			auto const fsv = fsv::filtered_string_view{};
			auto const vf = std::vector<fsv::filter>{[](const char& c) { return c == 'c' or c == '+' or c == '/'; },
			                                         [](const char& c) { return c > ' '; },
			                                         [](const char&) { return true; }};
			auto const new_fsv = fsv::compose(fsv, vf);
			CHECK(fsv == new_fsv);
		}
	}

	SECTION("with a list of predicates, constructor with predicate") {
		auto const pred = [](const char& c) { return c != 'y' and c != 'u'; };
		auto const fsv = fsv::filtered_string_view{"you think so?", pred};
		auto const vf = std::vector<fsv::filter>{[](const char& c) { return c == 'y' or c == 'o' or c == 'u'; },
		                                         [](const char& c) { return c >= 'u'; },
		                                         [](const char&) { return true; }};

		auto const sv = fsv::compose(fsv, vf);
		// predicate from the constructor is not applied
		// filters only the underlying string of fsv
		CHECK(static_cast<std::string>(sv) == "yu");
	}

	SECTION("shortcircuiting list of predicates") {
		auto const fsv = fsv::filtered_string_view{"blah"};
		auto const vf = std::vector<fsv::filter>{[](const char&) { return true; },
		                                         [](const char&) {
			                                         std::cout << "t";
			                                         return false;
		                                         },
		                                         [](const char&) {
			                                         std::cout << "should not print!";
			                                         return true;
		                                         }};
		auto buff = std::stringstream{};
		auto oldCoutBuffer = std::cout.rdbuf(buff.rdbuf());
		auto const new_fsv = fsv::compose(fsv, vf);
		// at this stage, nothing should be printed since filters are not called yet
		CHECK(buff.str().empty());
		// here, filters are applied to each character
		CHECK(fsv != new_fsv);
		std::cout.rdbuf(oldCoutBuffer);
		// this is the result of calling second predicate to "blah"
		// since the first one will always return true
		// also makes sure that the third predicate is not called
		auto pattern = std::string{"t+"};
		auto re = std::regex{pattern};
		auto match = std::smatch{};
		auto str = std::string{buff.str()};
		CHECK(std::regex_match(str, match, re) == true);
	}

	SECTION("getting and calling predicate got from compose()") {
		auto const best_languages = fsv::filtered_string_view{"c / c++"};
		auto const vf =
		    std::vector<fsv::filter>{[](const char& c) { return c == 'c' or c == '+' or c == '/'; },
		                             [](const char& c) { return c > ' '; },
		                             [](const char& c) { return c == 'c' or c == '+' or c == '/' or c == 'd'; }};

		auto const sv = fsv::compose(best_languages, vf);
		auto const pred = sv.predicate();
		CHECK(pred('c') == true);
		CHECK(pred('/') == true);
		CHECK(pred('+') == true);
		CHECK(pred(' ') == false);
		CHECK(pred('d') == false);
	}
}

TEST_CASE("split") {
	SECTION("when the tok is found in the middle only") {
		auto const interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
		auto const sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef",
		                                          [&interest](const char& c) { return interest.contains(c); }};
		auto const tok = fsv::filtered_string_view{" / "};
		auto const v = fsv::split(sv, tok);

		CHECK(static_cast<std::string>(v[0]) == "DEADBEEF");
		CHECK(static_cast<std::string>(v[1]) == "deadbeef");
	}
	SECTION("when the tok is found both at the start and end") {
		SECTION("when there is other character in between") {
			auto const fsv = fsv::filtered_string_view{"xax"};
			auto const tok = fsv::filtered_string_view{"x"};
			auto const v = fsv::split(fsv, tok);
			auto const expected = std::vector<fsv::filtered_string_view>{"", "a", ""};

			CHECK(v == expected);
		}
		SECTION("when there is no other character in between") {
			auto const fsv = fsv::filtered_string_view{"xx"};
			auto const tok = fsv::filtered_string_view{"x"};
			auto const v = fsv::split(fsv, tok);
			auto const expected = std::vector<fsv::filtered_string_view>{"", "", ""};

			CHECK(v == expected);
		}
	}
	SECTION("when tok does not appear on fsv") {
		auto const fsv = fsv::filtered_string_view{"blahblahblah"};
		auto const tok = fsv::filtered_string_view{" "};
		auto const v = fsv::split(fsv, tok);
		CHECK(v.size() == 1);
		CHECK(static_cast<std::string>(v[0]) == fsv);
	}
	SECTION("when fsv is empty") {
		SECTION("empty filtered string") {
			auto const fsv = fsv::filtered_string_view{"blahblahblah", [](const char&) { return false; }};
			auto const tok = fsv::filtered_string_view{"a"};
			auto const v = fsv::split(fsv, tok);
			CHECK(v.size() == 1);
			CHECK(static_cast<std::string>(v[0]) == fsv);
		}
		SECTION("with empty underlying string") {
			auto const fsv = fsv::filtered_string_view{""};
			auto const tok = fsv::filtered_string_view{"a"};
			auto const v = fsv::split(fsv, tok);
			CHECK(v.size() == 1);
			CHECK(static_cast<std::string>(v[0]) == fsv);
		}
		SECTION("with default fsv") {
			auto const fsv = fsv::filtered_string_view{};
			auto const tok = fsv::filtered_string_view{"a"};
			auto const v = fsv::split(fsv, tok);
			CHECK(v.size() == 1);
			CHECK(static_cast<std::string>(v[0]) == fsv);
		}
	}
	SECTION("when both fsv and tok have predicate, resulting in different strings from underlying ones") {
		auto const fsv_predicate = [](const char& c) { return c == 'b' or c == 'l' or c == 'h'; };
		auto const fsv = fsv::filtered_string_view{"blahblah", fsv_predicate};

		auto const tok_predicate = [](const char& c) { return c == 'l'; };
		auto const tok = fsv::filtered_string_view{"my light", tok_predicate};
		auto const v = fsv::split(fsv, tok);

		auto const expected = std::vector<fsv::filtered_string_view>{"b", "hb", "h"};
		CHECK(v == expected);
	}
}

TEST_CASE("substr") {
	SECTION("with default predicate") {
		auto const sv = fsv::filtered_string_view{"Siberian Husky"};
		auto const sub_sv = fsv::substr(sv, 9);
		CHECK(static_cast<std::string>(sub_sv) == "Husky");
	}
	SECTION("with a predicate which filters out some characters from the underlying string") {
		auto const fsv =
		    fsv::filtered_string_view{"c / c++", [](const char& c) { return c == 'c' or c == '+' or c == '/'; }};
		SECTION("giving neither of start pos and count") {
			auto const sub_sv = fsv::substr(fsv);
			CHECK(static_cast<std::string>(sub_sv) == "c/c++");
		}
		SECTION("giving only start pos") {
			auto const sub_sv = fsv::substr(fsv, 2);
			CHECK(static_cast<std::string>(sub_sv) == "c++");
		}
		SECTION("giving both start pos and count") {
			auto const sub_sv = fsv::substr(fsv, 0, 3);
			CHECK(static_cast<std::string>(sub_sv) == "c/c");
		}
	}
	SECTION("with empty string") {
		auto const sv = fsv::filtered_string_view{""};
		auto const sub_sv = fsv::substr(sv, 0);
		CHECK(static_cast<std::string>(sub_sv) == "");
	}
	SECTION("with empty string when filtered") {
		auto const sv = fsv::filtered_string_view{"bcdfghjklmn", [](const char& c) { return c == 'a'; }};
		auto const sub_sv = fsv::substr(sv, 0);
		CHECK(static_cast<std::string>(sub_sv) == "");
	}
	SECTION("with default constructor") {
		auto const sv = fsv::filtered_string_view{};
		auto const sub_sv = fsv::substr(sv, 0);
		CHECK(static_cast<std::string>(sub_sv) == "");
	}
	SECTION("gives empty substring") {
		auto const sv = fsv::filtered_string_view{"hello"};
		auto const sub_sv = fsv::substr(sv, static_cast<int>(sv.size()));
		CHECK(static_cast<std::string>(sub_sv) == "");
	}
}

TEST_CASE("iter on default fsv") {
	auto const fsv_const = fsv::filtered_string_view{};
	auto fsv = fsv::filtered_string_view{};
	CHECK(fsv_const.begin() == fsv_const.end());
	CHECK(fsv.begin() == fsv.end());
	CHECK(fsv_const.crbegin() == fsv_const.crend());
	CHECK(fsv.rbegin() == fsv.rend());
}

TEST_CASE("iter on empty string fsv") {
	auto const fsv_const = fsv::filtered_string_view{""};
	auto fsv = fsv::filtered_string_view{""};
	CHECK(fsv_const.begin() == fsv_const.end());
	CHECK(fsv.begin() == fsv.end());
	CHECK(fsv_const.crbegin() == fsv_const.crend());
	CHECK(fsv.rbegin() == fsv.rend());
}

TEST_CASE("iter combined with ranges") {
	SECTION("const_iterator: begin() const and cbegin() are the same") {
		auto const fsv_const = fsv::filtered_string_view{"c / cd+"};
		SECTION("going forward") {
			auto const_iter = fsv_const.begin();
			CHECK(*const_iter == 'c');
			CHECK(std::next(const_iter) != fsv_const.begin());
			CHECK(*std::next(const_iter) == ' ');
			CHECK(*std::next(const_iter, 5) == 'd');
			CHECK(std::next(const_iter, 7) == fsv_const.end());
		}
		SECTION("going backward using .cend()") {
			auto const_iter = fsv_const.cend();
			CHECK(*std::prev(const_iter) == '+');
			CHECK(*std::prev(const_iter, 5) == '/');
			CHECK(std::prev(const_iter, 7) == fsv_const.cbegin());
		}
		SECTION("going backward using .rbegin() const and .rend() const") {
			auto const vw = fsv::filtered_string_view{"dollyball", [](const char& c) { return c != 'l'; }};
			auto v = std::vector<char>{vw.rbegin(), vw.rend()};
			CHECK(v.size() == 5);
			CHECK(v[0] == 'a');
			CHECK(v[1] == 'b');
			CHECK(v[2] == 'y');
			CHECK(v[3] == 'o');
			CHECK(v[4] == 'd');
		}
		SECTION("going backward using .crbegin() const and .crend() const") {
			auto const vw = fsv::filtered_string_view{"dollyball", [](const char& c) { return c != 'l'; }};
			auto v = std::vector<char>{vw.crbegin(), vw.crend()};
			CHECK(v.size() == 5);
			CHECK(v[0] == 'a');
			CHECK(v[1] == 'b');
			CHECK(v[2] == 'y');
			CHECK(v[3] == 'o');
			CHECK(v[4] == 'd');
		}
	}
	SECTION("non const_iterator") {
		auto fsv = fsv::filtered_string_view{"tummy", [](const char& c) { return c != 'm'; }};
		SECTION("going forward") {
			auto iter = fsv.begin();
			CHECK(*std::next(iter) == 'u');
			CHECK(*std::next(iter, 2) == 'y');
			CHECK(std::next(iter, 3) == fsv.end());
		}
		SECTION("going backward") {
			auto iter = fsv.end();
			CHECK(*std::prev(iter) == 'y');
			CHECK(*std::prev(iter, 3) == 't');
			CHECK(std::prev(iter, 4) == fsv.begin());
		}
		SECTION("going backward using .rbegin() and .rend()") {
			auto const v = std::vector<char>{fsv.rbegin(), fsv.rend()};
			CHECK(v.size() == 3);
			CHECK(v[0] == 'y');
			CHECK(v[1] == 'u');
			CHECK(v[2] == 't');
		}
	}
}
TEST_CASE("basic iter operations") {
	auto const fsv =
	    fsv::filtered_string_view{"country road", [](const char& c) { return c != 'o' and c != 'u' and c != 'a'; }};
	SECTION("prefix ++ and --") {
		auto iter = fsv.begin();
		++iter;
		CHECK(*iter == 'n');
		++iter;
		--iter;
		--iter;
		CHECK(*iter == 'c');
	}
	SECTION("postfix ++ and --") {
		auto iter = fsv.end();
		iter--;
		CHECK(*iter == 'd');
		iter--;
		iter--;
		CHECK(*iter == ' ');
		iter++;
		CHECK(*iter == 'r');
	}
	SECTION("two iterators on the same fsv, going to the same character") {
		auto iter1 = fsv.begin();
		auto iter2 = fsv.rbegin();
		iter1++;
		iter1++;
		iter1++;
		iter1++;
		iter2++;
		iter2++;
		iter2++;
		CHECK(*iter1 == *iter2);
	}
}