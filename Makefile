usage_test:
	clang++ tests/usage_test.cpp -S -o tests/usage_test.asm -g -std=c++14 -I. -O3
	clang++ tests/usage_test.cpp -o tests/usage_test -g -std=c++14 -I. -O3

test: usage_test
	./tests/usage_test

clean:
	test ! -e tests/usage_test || rm tests/usage_test
	test ! -e tests/usage_test.dSYM || rm -rf tests/usage_test.dSYM
