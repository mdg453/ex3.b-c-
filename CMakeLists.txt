cmake_minimum_required(VERSION 3.23)
project(ex3b_meitar453 C)

set(CMAKE_C_STANDARD 99)
add_compile_options("-Wpointer-arith" "-Werror")

include_directories(.)

add_executable(ex3b_meitar453
        linked_list.c
        linked_list.h
        markov_chain.h
        snakes_and_ladders.c
        tweets_generator.c)
