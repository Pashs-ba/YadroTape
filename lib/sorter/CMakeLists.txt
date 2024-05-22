add_library(sorter Sorter.h
        Sorter.cpp)
target_link_libraries(sorter PRIVATE my_cool_tape config)