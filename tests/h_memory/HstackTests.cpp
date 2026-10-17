#include <catch2/catch_test_macros.hpp>

#include "Horizon.hpp"

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

#define HSTACK_HSTACK_HSTACK_SIZE_HSTACK_OPERATOR_SQUARE_BRACKETS
TEST_CASE("Hstack::Hstack, Hstack::size, Hstack::operator[]", "[Hstack]") {
    SECTION("Hstack::Hstack(size) ; Hstack::size() : simple check") {
        Hstack<size_t, std::string> stack = Hstack<size_t, std::string>(47);

        REQUIRE(stack.size() == 47);
        REQUIRE_THROWS_AS(stack[47], std::out_of_range);
    }

    SECTION("Hstack::Hstack(Args) ; Hstack::operator[] : simple check") {
        Hstack<size_t, int> stack = {
            5, 4, 5, 2, 95, 23
        };

        REQUIRE(stack[0] == 5);
        REQUIRE(stack[1] == 4);
        REQUIRE(stack[2] == 5);
        REQUIRE(stack[3] == 2);
        REQUIRE(stack[4] == 95);
        REQUIRE(stack[5] == 23);
        REQUIRE_THROWS_AS(stack[6], std::out_of_range);
    }

}

#define HSTACK_LVALUE
TEST_CASE("Hstack::Hstack(lvalue), Hstack::operator=(lvalue)", "[Hstack]") {
    SECTION("Hstack::Hstack(lvalue) : simple check") {
        Hstack<size_t, int> stack1 = {
            1, 2, 3
        };

        Hstack<size_t, int> stack2 = stack1;

        REQUIRE(stack2[0] == 1);
        REQUIRE(stack2[1] == 2);
        REQUIRE(stack2[2] == 3);
        REQUIRE(stack1[0] == 1);
        REQUIRE(stack1[1] == 2);
        REQUIRE(stack1[2] == 3);
    }

    SECTION("Hstack::operator=(lvalue) : simple check") {
        Hstack<size_t, int> stack1 = {
            10, 20, 30
        };

        Hstack<size_t, int> stack2;
        stack2 = stack1;

        REQUIRE(stack2[0] == 10);
        REQUIRE(stack2[1] == 20);
        REQUIRE(stack2[2] == 30);
        REQUIRE(stack1[0] == 10);
        REQUIRE(stack1[1] == 20);
        REQUIRE(stack1[2] == 30);
    }
}

#define HSTACK_RVALUE
TEST_CASE("Hstack::Hstack(rvalue), Hstack::operator=(rvalue)", "[Hstack]") {
    SECTION("Hstack::Hstack(rvalue) : simple check") {
        Hstack<size_t, int> stack1 = {
            1, 2, 3
        };

        Hstack<size_t, int> stack2 = std::move(stack1);

        REQUIRE(stack2[0] == 1);
        REQUIRE(stack2[1] == 2);
        REQUIRE(stack2[2] == 3);
    }

    SECTION("Hstack::operator=(rvalue) : simple check") {
        Hstack<size_t, int> stack1 = {
            10, 20, 30
        };

        Hstack<size_t, int> stack2;
        stack2 = std::move(stack1);

        REQUIRE(stack2[0] == 10);
        REQUIRE(stack2[1] == 20);
        REQUIRE(stack2[2] == 30);
        REQUIRE_THROWS_AS(stack1[0], std::out_of_range);
    }
}

#define HSTACK_LAST
TEST_CASE("Hstack::last", "[Hstack]") {
    SECTION("Hstack::last ; Hstack::Hstack(Args) : simple check") {
        Hstack<size_t, int> stack = {
            10, 20, 30
        };

        REQUIRE(stack.last() == 30);
    }

    SECTION("Hstack::last : empty stack throws std::out_of_range") {
        Hstack<size_t, int> stack;

        REQUIRE_THROWS_AS(stack.last(), std::out_of_range);
    }
}

#define HSTACK_PUSH
TEST_CASE("Hstack::push", "[Hstack]") {
    SECTION("Hstack::push(rvalue) : simple check") {
    	Hstack<size_t, int> stack;
        
        stack.push(10);
        stack.push(20);
        stack.push(30);

        REQUIRE(stack[0] == 10);
        REQUIRE(stack[1] == 20);
        REQUIRE(stack[2] == 30);
    }

    SECTION("Hstack::push(rvalue) : hard check", "[Hstack]") {
		Hstack<size_t, std::string> rvalue_stack;
		constexpr int rvalue_count = 100;
		
		for (int i = 0; i < rvalue_count; i++) {
			rvalue_stack.push("item_" + std::to_string(i));
		}

        for (int i = rvalue_count - 1; i >= 0; i--) {
            REQUIRE(rvalue_stack[i] == "item_" + std::to_string(i));
        }
    }

    SECTION("Hstack::push(lvalue) : simple check") {
        Hstack<size_t, int> stack;
        int value1 = 42;
        int value2 = 47;
        int value3 = 99;

        stack.push(value1);
        stack.push(value2);
        stack.push(value3);

        REQUIRE(stack[0] == 42);
        REQUIRE(stack[1] == 47);
        REQUIRE(stack[2] == 99);
        REQUIRE(value1 == 42); // value1 should remain unchanged
        REQUIRE(value2 == 47); // value2 should remain unchanged
        REQUIRE(value3 == 99); // value3 should remain unchanged
    }

    SECTION("Hstack::push(rvalue) : complex check") {
        MoveChecker_copy_count = 0; MoveChecker_copy_assign_count = 0;  
        MoveChecker_move_count = 0; MoveChecker_move_assign_count = 0; // reset counters

        Hstack<size_t, MoveChecker> stack;
        MoveChecker value = MoveChecker(42);

        stack.push(std::move(value));

        REQUIRE(MoveChecker_move_count == 0);
        REQUIRE(MoveChecker_copy_count == 0);
        REQUIRE(MoveChecker_move_assign_count == 1);
        REQUIRE(MoveChecker_copy_assign_count == 0);
        REQUIRE(stack[0].value == 42);
        REQUIRE(value.value == 0); // value should be in a moved-from state
    }
}

#define HSTACK_POP
TEST_CASE("Hstack::pop", "[Hstack]") {
    SECTION("Hstack::pop : simple check") {
        Hstack<size_t, int> stack;

        stack.push(100);
        stack.push(200);
        stack.push(204);
        stack.push(3250);
        stack.push(-241);

        int& value1 = stack.pop();
        REQUIRE_THROWS_AS(stack[4], std::out_of_range); // Ensure the stack size has decreased after pop
        int& value2 = stack.pop();
        REQUIRE_THROWS_AS(stack[3], std::out_of_range); // Ensure the stack size has decreased after pop

        REQUIRE(value1 == -241);
        REQUIRE(value2 == 3250);
        REQUIRE(stack.pop() == 204);
        REQUIRE_THROWS_AS(stack[2], std::out_of_range); // Ensure the stack size has decreased after pop
        REQUIRE(stack.pop() == 200);
        REQUIRE_THROWS_AS(stack[1], std::out_of_range); // Ensure the stack size has decreased after pop
        REQUIRE(stack.pop() == 100);
        REQUIRE_THROWS_AS(stack[0], std::out_of_range); // Ensure the stack size has decreased after pop
    }

    SECTION("Hstack::pop : complex check") {
        // reset counters
        MoveChecker_copy_count = 0; MoveChecker_move_count = 0;
        MoveChecker_copy_assign_count = 0; MoveChecker_move_assign_count = 0;

        Hstack<size_t, MoveChecker> stack;
        MoveChecker value1 = MoveChecker(42);
        MoveChecker value2 = MoveChecker(47);
        
        stack.push(std::move(value1));
        stack.push(std::move(value2));
        
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

    SECTION("Hstack::pop ; Hstack::push(rvalue) : hard check", "[Hstack]") {
		Hstack<size_t, std::string> stack;
		constexpr int count = 100;
		
		for (int i = 0; i < count; i++) {
			stack.push("item_" + std::to_string(i));
		}
		
		for (int i = count - 1; i >= 0; i--) {
			REQUIRE(stack.pop() == "item_" + std::to_string(i));
		}
	}

    SECTION("Hstack::pop ; Hstack::last, Hstack::push : modify return check") {
        Hstack<size_t, int> stack;

		stack.push(1);
		stack.push(2);
		int& top = stack.last();
		top = 42;

		REQUIRE(stack.pop() == 42);
		REQUIRE(stack.pop() == 1);
	}

    SECTION("Hstack::pop : empty stack throws std::out_of_range") {
        Hstack<size_t, int> stack;

        REQUIRE_THROWS_AS(stack.pop(), std::out_of_range);
    }

}

#define HSTACK_CLEAR
TEST_CASE("Hstack::clear", "[Hstack]") {
    SECTION("Hstack::clear : simple check", "[Hstack]") {
        Hstack<size_t, int> stack;

        stack.push(7);
        stack.push(8);
        stack.push(9);

        stack.clear();

        REQUIRE_THROWS_AS(stack.pop(), std::out_of_range);

        stack.push(77);

        REQUIRE(stack.last() == 77);
    }
}

#define HSTACK_DEL
TEST_CASE("Hstack::del", "[Hstack]") {
	SECTION("Hstack::del : simple check", "[Hstack]") {
        Hstack<size_t, int> stack;

        stack.push(11);
        stack.push(12);

        stack.del();

        REQUIRE_THROWS_AS(stack.pop(), std::out_of_range);

        stack.push(21);
        stack.push(22);

        REQUIRE(stack.pop() == 22);
        REQUIRE(stack.pop() == 21);
    }
}

#define HSTACK_DO_EACH
TEST_CASE("Hstack::do_each", "[Hstack]") {
    SECTION("Hstack::do_each : modify values simple check") {
        Hstack<size_t, int> stack;

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

    SECTION("Hstack::do_each : find value") {
        Hstack<size_t, std::string> stack;

        stack.push("apple");
        stack.push("banana");
        stack.push("cherry");

        std::string* found_value = nullptr;
        auto find_banana = [](std::string& value) -> bool {
            return value == "banana";
        };

        stack.do_each(find_banana, &found_value);

        REQUIRE(found_value != nullptr);
        REQUIRE(*found_value == "banana");
    }

    SECTION("Hstack::do_each : modify values hard check") {
        Hstack<size_t, std::string> stack;
        
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
