#pragma once
#include "../MyCoolTape/MyCoolTape.h"

namespace Sorter{
void sort(MyCoolTape& from_tape, MyCoolTape& to_tape,const Config& config) noexcept;
void merge(uint32_t block_size,
           MyCoolTape& tmp1,
           MyCoolTape& tmp2,
           MyCoolTape& to,
           size_t block_1_size,
           size_t block_2_size) noexcept;
}