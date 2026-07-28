#include <catch2/catch_test_macros.hpp>

#include "memory/Hstack.hpp"

#include <string>
#include <utility>

static int MoveChecker_copy_count = 0;
static int MoveChecker_move_count = 0;
static int MoveChecker_copy_assign_count = 0;
static int MoveChecker_move_assign_count = 0;

struct MoveChecker {
    int value;

    MoveChecker() : value(5) {};

    MoveChecker(int value) : value(value) {};

    MoveChecker(const MoveChecker& other) : value(other.value) { 
        ++MoveChecker_copy_count;
    }

    MoveChecker(MoveChecker&& other) noexcept : value(other.value) { 
        ++MoveChecker_move_count;
        other.value = 0;
    }

    MoveChecker& operator=(const MoveChecker& other) {
        ++MoveChecker_copy_assign_count;
        this->value = other.value;
        return *this;
    }

    MoveChecker& operator=(MoveChecker&& other) noexcept {
        ++MoveChecker_move_assign_count;
        this->value = other.value;
        other.value = 0;
        return *this;
    }
};

TEST_CASE("Hstack::Hstack", "[Hstack]") {

    SECTION("Hstack::Hstack(size)") {
        Hstack<size_t, std::string> stack = Hstack<size_t, std::string>(47);

        REQUIRE(stack.size() == 47);
    }

    SECTION("Hstack::Hstack(Args)") {
        Hstack<size_t, int> stack = {
            5, 4, 5, 2, 95, 23
        };

        REQUIRE(stack[0] == 5);
        REQUIRE(stack[1] == 4);
        REQUIRE(stack[2] == 5);
        REQUIRE(stack[3] == 2);
        REQUIRE(stack[4] == 95);
        REQUIRE(stack[5] == 23);
    }

}

TEST_CASE("Hstack::push, Hstack::last", "[Hstack]") {

    SECTION("Hstack::push(static value)") {
    	Hstack<std::size_t, int> stack;
        
        stack.push(10);
        stack.push(20);
        stack.push(30);

        REQUIRE(stack.last() == 30);
    }

    SECTION("Hstack::push(lvalue reference)") {
        Hstack<std::size_t, int> stack;
        int value = 47;

        stack.push(value);

        REQUIRE(stack.last() == 47);
        REQUIRE(value == 47);
    }

    SECTION("Hstack::push(rvalue reference)") {
        // reset counters
        MoveChecker_copy_count = 0; MoveChecker_move_count = 0;
        MoveChecker_copy_assign_count = 0; MoveChecker_move_assign_count = 0;

        Hstack<std::size_t, MoveChecker> stack;
        MoveChecker value = MoveChecker(42);

        stack.push(value);

        REQUIRE(MoveChecker_move_count == 0);
        REQUIRE(MoveChecker_copy_count == 0);
        REQUIRE(MoveChecker_move_assign_count == 1);
        REQUIRE(MoveChecker_copy_assign_count == 0);
        REQUIRE(stack.last().value == 42);
        REQUIRE(value.value == 0); // value should be in a moved-from state
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

        int& value1 = stack.pop();
        int& value2 = stack.pop();

        REQUIRE(value1 == -241);
        REQUIRE(value2 == 3250);
        REQUIRE(stack.pop() == 204);
        REQUIRE(stack.pop() == 200);
        REQUIRE(stack.pop() == 100);
    }

    SECTION("Hstack::pop : empty stack throws std::out_of_range") {
        Hstack<std::size_t, int> stack;

        REQUIRE_THROWS_AS(stack.pop(), std::out_of_range);
    }

    SECTION("Hstack::pop(rvalue) : properly moves complex objects") {
        // reset counters
        MoveChecker_copy_count = 0; MoveChecker_move_count = 0;
        MoveChecker_copy_assign_count = 0; MoveChecker_move_assign_count = 0;

        Hstack<std::size_t, MoveChecker> stack;
        MoveChecker value1 = MoveChecker(42);
        MoveChecker value2 = MoveChecker(47);
        
        stack.push(value1);
        stack.push(value2);
        
        MoveChecker& result2 = stack.pop();
        MoveChecker& result1 = stack.pop();
        
        REQUIRE(MoveChecker_copy_count == 0);
        REQUIRE(MoveChecker_move_count == 0);
        REQUIRE(MoveChecker_copy_assign_count == 0);
        REQUIRE(MoveChecker_move_assign_count == 3);
        REQUIRE(result1.value == 42);
        REQUIRE(result2.value == 47);
        REQUIRE(value1.value == 0);
        REQUIRE(value2.value == 0);
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
