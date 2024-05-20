add_library(my_cool_tape MyCoolTape.h MyCoolTape.cpp
        OneStepDirection.h)
target_link_libraries(my_cool_tape PRIVATE config)