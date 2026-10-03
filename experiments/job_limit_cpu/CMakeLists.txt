cmake_minimum_required(VERSION 3.20)

project(job_limit_cpu LANGUAGES CXX)

add_executable(job_limit_cpu
main.cpp
)

target_compile_features(job_limit_cpu PRIVATE cxx_std_17)

if (MSVC)
target_compile_options(job_limit_cpu PRIVATE /W4)
else()
target_compile_options(job_limit_cpu PRIVATE -Wall -Wextra -Wpedantic)
endif()
