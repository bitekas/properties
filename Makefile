usage_test:
	clang++ tests/usage_test.cpp -o tests/usage_test -g -std=c++14 -I.

test: usage_test
	./tests/usage_test

clean:
	test ! -e tests/usage_test || rm tests/usage_test
	test ! -e tests/usage_test.dSYM || rm -rf tests/usage_test.dSYM
