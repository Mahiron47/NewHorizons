#include <catch2/catch_test_macros.hpp>

#include "memory/Hstack.hpp"

#include <string>
#include <utility>

TEST_CASE("Hstack::push, Hstack::last", "[Hstack]") {

    SECTION("Hstack::push(lvalue reference)") {
    	Hstack<std::size_t, int> stack;
        
        stack.push(10);
        stack.push(20);
        stack.push(30);

        REQUIRE(stack.last() == 30);
    }

    SECTION("Hstack::push(rvalue reference)") {
        Hstack<std::size_t, std::string> stack;
        std::string value = "alpha";
        
        stack.push(std::move(value));

        REQUIRE(stack.last() == "alpha");
    }

    SECTION("Hstack::last : empty stack throws std::out_of_range") {
        Hstack<std::size_t, std::string> stack;

		REQUIRE_THROWS_AS(stack.last(), std::out_of_range);
	}
}

TEST_CASE("Hstack::pop", "[Hstack]") {

    SECTION("Hstack::pop(lvalue reference)") {
        Hstack<std::size_t, int> stack;

        stack.push(100);
        stack.push(200);
        stack.push(204);
        stack.push(3250);
        stack.push(-241);

        REQUIRE(stack.pop() == -241);
        REQUIRE(stack.pop() == 3250);
        REQUIRE(stack.pop() == 204);
        REQUIRE(stack.pop() == 200);
        REQUIRE(stack.pop() == 100);
    }

    SECTION("Hstack::pop : empty stack throws std::out_of_range") {
        Hstack<std::size_t, int> stack;

        REQUIRE_THROWS_AS(stack.pop(), std::out_of_range);
    }

    SECTION("Hstack::pop(rvalue) : properly moves complex objects") {
        Hstack<std::size_t, std::string> stack;
        
        stack.push("hello");
        stack.push("world");
        
        std::string result1 = stack.pop();
        std::string result2 = stack.pop();
        
        REQUIRE(result1 == "world");
        REQUIRE(result2 == "hello");
    }

    SECTION("Hstack::pop, Hstack::last : returns mutable reference to top") {
        Hstack<std::size_t, int> stack;

		stack.push(1);
		stack.push(2);
		int& top = stack.last();
		top = 42;

		REQUIRE(stack.pop() == 42);
		REQUIRE(stack.pop() == 1);
	}
}

TEST_CASE("Hstack::operator[]", "[Hstack]") {
	Hstack<std::size_t, int> stack;

	stack.push(3);
	stack.push(6);
	stack.push(9);

	REQUIRE(stack[0] == 3);
	REQUIRE(stack[1] == 6);
	REQUIRE(stack[2] == 9);
	REQUIRE_THROWS_AS(stack[3], std::out_of_range);

	SECTION("Hstack::operator[] : handles mutable access with rvalue objects") {
		Hstack<std::size_t, std::string> rvalue_stack;
		
		rvalue_stack.push("initial_1");
		rvalue_stack.push("initial_2");
		rvalue_stack.push("initial_3");
		
		std::string& ref = rvalue_stack[1];
		REQUIRE(ref == "initial_2");
		ref = "modified";
		REQUIRE(rvalue_stack[1] == "modified");
	}
}

TEST_CASE("Hstack::clear", "[Hstack]") {
	Hstack<std::size_t, int> stack;

	stack.push(7);
	stack.push(8);
	stack.push(9);

	stack.clear();

    REQUIRE_THROWS_AS(stack.pop(), std::out_of_range);
    
    stack.push(77);

    REQUIRE(stack.last() == 77);
}

TEST_CASE("Hstack::del", "[Hstack]") {
	Hstack<std::size_t, int> stack;

	stack.push(11);
	stack.push(12);

    stack.del();

    REQUIRE_THROWS_AS(stack.pop(), std::out_of_range);

	stack.push(21);
	stack.push(22);

    REQUIRE(stack.pop() == 22);
	REQUIRE(stack.pop() == 21);
}

TEST_CASE("Hstack::push, Hstack::pop : handles large sequence", "[Hstack]") {
	Hstack<std::size_t, int> stack;
	constexpr int count = 1000;

	for (int i = 0; i < count; ++i) {
		stack.push(i);
	}

	for (int i = count - 1; i >= 0; --i) {
		REQUIRE(stack.pop() == i);
	}

	SECTION("Hstack::push(rvalue), Hstack::pop : handles large sequence of strings") {
		Hstack<std::size_t, std::string> rvalue_stack;
		constexpr int rvalue_count = 100;
		
		for (int i = 0; i < rvalue_count; ++i) {
			rvalue_stack.push("item_" + std::to_string(i));
		}
		
		for (int i = rvalue_count - 1; i >= 0; --i) {
			REQUIRE(rvalue_stack.pop() == "item_" + std::to_string(i));
		}
	}
}

TEST_CASE("Hstack::do_each", "[Hstack]") {
    
    SECTION("Hstack::do_each : modifies values") {
        Hstack<std::size_t, int> stack;

        stack.push(1);
        stack.push(2);
        stack.push(3);

        auto increment = [](int& value) -> void {
            value += 10;
        };

        stack.do_each(increment);

        REQUIRE(stack[0] == 11);
        REQUIRE(stack[1] == 12);
        REQUIRE(stack[2] == 13);
    }

    SECTION("Hstack::do_each : finds value") {
        Hstack<std::size_t, std::string> stack;

        stack.push("apple");
        stack.push("banana");
        stack.push("cherry");

        std::string* found_value = nullptr;
        auto find_banana = [](std::string& value) -> bool {
            return value == "banana";
        };

        stack.do_each(find_banana, found_value);

        REQUIRE(found_value != nullptr);
        REQUIRE(*found_value == "banana");
    }

    SECTION("Hstack::do_each : processes rvalue objects with transformation") {
        Hstack<std::size_t, std::string> stack;
        
        stack.push("a");
        stack.push("b");
        stack.push("c");
        
        auto append_suffix = [](std::string& value) -> void {
            value += "_processed";
        };
        
        stack.do_each(append_suffix);
        
        REQUIRE(stack[0] == "a_processed");
        REQUIRE(stack[1] == "b_processed");
        REQUIRE(stack[2] == "c_processed");
    }
}
