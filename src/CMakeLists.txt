cmake_minimum_required(VERSION 3.10.0)

add_library(Horizons STATIC Horizon.cpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang") 

# ---------------------------------------------------------------------------
# Статический анализ Clang-Tidy (noexcept, constexpr/const, nodiscard, copy/move)
# ---------------------------------------------------------------------------
find_program(CLANG_TIDY_EXE NAMES clang-tidy 
    HINTS "C:/msys64/ucrt64/bin" "C:/Program Files/LLVM/bin"
)

if (CLANG_TIDY_EXE)
    set_target_properties(Horizons PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=performance-*,-performance-avoid-endl,-performance-enum-size,bugprone-exception-escape,readability-const-return-type,cppcoreguidelines-missing-std-forward;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/.*"
    )
endif()

endif()

# ---------------------------------------------------------------------------
# Флаги компилятора GCC
# ---------------------------------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")

target_compile_options(Horizons PRIVATE
        # Игнорирование неиспользуемых сущностей
        -Wno-unused-but-set-parameter 
        -Wno-unused-but-set-variable
        -Wno-unused-parameter
        -Wno-unused-variable 
        -Wno-unused-function 

        # Подсказки атрибутов функций и девиртуализации
        -Wsuggest-attribute=pure  
        -Wsuggest-attribute=const
        -Wsuggest-attribute=noreturn
        -Wsuggest-attribute=cold
        -Wsuggest-attribute=malloc
        -Wsuggest-final-types
        -Wsuggest-final-methods
        -Wsuggest-override

        # Проверки constexpr
        -Winvalid-constexpr

        # Диагностика перемещений, кастов и логики
        -Wpessimizing-move
        -Wredundant-move 
        -Wuseless-cast
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wnull-dereference
        -Wstrict-aliasing=2

        # Инлайнинг и ограничения оптимизатора
        -Winline
        -Wdisabled-optimization

        # Векторизация 
        -Wvector-operation-performance)

# ---------------------------------------------------------------------------
# Флаги компилятора Clang
# ---------------------------------------------------------------------------
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang") 

target_compile_options(Horizons PRIVATE
        # Оптимизация перемещений и копирований
        -Wpessimizing-move
        -Wredundant-move
        -Wreturn-std-move
        -Wrange-loop-analysis
        -Wnull-dereference

        # Неиспользуемый внутренний код (dead code elimination hints)
        -Wunneeded-internal-declaration
        -Wunneeded-member-function

        # Диагностика оптимизатора (Remarks)
        -Rpass-missed=inline
        -Rpass-missed=loop-vectorize
        -Rpass-analysis=loop-vectorize
        -Rpass=loop-vectorize)

endif()

target_include_directories(Horizons PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Horizons PUBLIC Vulkan::Vulkan)

if (WIN32)
	add_subdirectory(windows)
elseif(LINUX)
# linux code implementation
else()
	message(FATAL_ERROR "Unsupported platform")
endif()