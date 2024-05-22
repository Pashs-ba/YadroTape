#include "Sorter.h"
#include<vector>
#include<iostream>
inline void simple_sort(MyCoolTape& from_tape, MyCoolTape& to_tape, const Config& config) noexcept {
  std::vector<uint32_t> data{};
  data.reserve(config.M / 4);
  for (int i = 0; i < config.N; ++i) {
    data.emplace_back(from_tape.read_current());
    from_tape.one_step(DOWN);
  }
  std::sort(data.begin(), data.end());
  for (auto& i : data) {
    to_tape.write_current(i);
    to_tape.one_step(DOWN);
  }
}

uint32_t get_suitable_block_size(uint32_t N, uint32_t M) noexcept {
  for (uint32_t i = N / 2; i > 1; i /= 2) {
    if (i <= M / 4) {
      return i == M / 4 ? i / 2 : i;
    }
  }
  return 1;
}

MyCoolTape make_tmp(const std::string& name, const Config& config) noexcept {
  std::ofstream create{config.tmp_folder + name};
  create.close();
  std::fstream file;
  file.open(config.tmp_folder + name, std::ios::in | std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Can't open file " << name << '\n';
    std::exit(1);
  }
  return MyCoolTape{std::move(file), 0, config};
}

std::pair<size_t, size_t> pre_sort(MyCoolTape& from_tape,
                                   MyCoolTape& tmp1,
                                   MyCoolTape& tmp2,
                                   const Config& config,
                                   uint32_t block_size) noexcept {
  std::vector<uint32_t> data{};
  data.reserve(block_size);
  size_t tmp1_size = 0;
  for (size_t i = 0; i < config.N / 2; ++i) {
    data.emplace_back(from_tape.read_current());
    from_tape.one_step(DOWN);
    if (data.size() == block_size) {
      tmp1_size += data.size();
      std::sort(data.begin(), data.end());
      for (auto& j : data) {
        tmp1.write_current(j);
        tmp1.one_step(DOWN);
      }
      data.clear();
    }
  }
  if (!data.empty()) {
    tmp1_size += data.size();
    std::sort(data.begin(), data.end());
    for (auto& j : data) {
      tmp1.write_current(j);
      tmp1.one_step(DOWN);
    }
  }
  data.clear();
  uint32_t tmp2_size = 0;
  for (size_t i = config.N / 2; i < config.N; ++i) {
    data.emplace_back(from_tape.read_current());
    from_tape.one_step(DOWN);
    if (data.size() == block_size) {
      tmp2_size += data.size();
      std::sort(data.begin(), data.end());
      for (auto& j : data) {
        tmp2.write_current(j);
        tmp2.one_step(DOWN);
      }
      data.clear();
    }
  }
  if (!data.empty()) {
    tmp2_size += data.size();
    std::sort(data.begin(), data.end());
    for (auto& j : data) {
      tmp2.write_current(j);
      tmp2.one_step(DOWN);
    }
  }
  return {tmp1_size, tmp2_size};
}

void Sorter::sort(MyCoolTape& from_tape, MyCoolTape& to_tape, const Config& config) noexcept {
  if (config.M / 4 >= config.N) {
    simple_sort(from_tape, to_tape, config);
    return;
  }
  uint32_t pre_sorted_block_size = get_suitable_block_size(config.N, config.M);
  auto tmp1 = make_tmp("tmp1.tape", config);
  auto tmp2 = make_tmp("tmp2.tape", config);
  auto tmp3 = make_tmp("tmp3.tape", config);
  auto tmp4 = make_tmp("tmp4.tape", config);
  auto [tmp1_size, tmp2_size] = pre_sort(
      from_tape,
      tmp1,
      tmp2,
      config,
      pre_sorted_block_size);
  tmp1.rewind();
  tmp2.rewind();
  size_t block_count =
      tmp1_size % 2 == 0 ? tmp1_size / (pre_sorted_block_size) : tmp1_size / (pre_sorted_block_size) + 1;
  size_t current_block_size = pre_sorted_block_size;
  while (current_block_size < config.N) {
    for (size_t i = 0; i < block_count / 2; ++i) {
      size_t block_1_size = current_block_size;
      size_t block_2_size = current_block_size;
      merge(current_block_size,
            tmp1,
            tmp2,
            tmp3,
            block_1_size,
            block_2_size);
    }
    bool is_empty = false;
    for (size_t i = block_count / 2; i < block_count; ++i) {
      size_t block_1_size = current_block_size;
      size_t block_2_size = current_block_size;
      if (i == block_count - 1) {
        block_1_size = tmp1_size - i * (current_block_size);
        block_2_size = tmp2_size - i * (current_block_size);
        if (block_1_size == 0 || block_2_size == 0) {
          is_empty = true;
        }
      }
      merge(current_block_size,
            tmp1,
            tmp2,
            tmp4,
            block_1_size,
            block_2_size);
    }

    tmp1_size = tmp3.get_current_position();
    tmp2_size = tmp4.get_current_position();
    if (is_empty) {
      tmp3.rewind();
      tmp4.rewind();
      merge(current_block_size,
            tmp3,
            tmp4,
            to_tape,
            tmp1_size,
            tmp2_size);
      return;
    }
    current_block_size *= 2;
    block_count = std::max(tmp1_size, tmp2_size) % 2 == 0 ? std::max(tmp1_size, tmp2_size) / (current_block_size) :
                  std::max(tmp1_size, tmp2_size) / (current_block_size) + 1;

    tmp3.rewind();
    tmp4.rewind();
    tmp1.rewind();
    tmp2.rewind();
    std::swap(tmp1, tmp3);
    std::swap(tmp2, tmp4);
  }
}

void Sorter::merge(uint32_t block_size,
                   MyCoolTape& tmp1,
                   MyCoolTape& tmp2,
                   MyCoolTape& to,
                   size_t block_1_size,
                   size_t block_2_size) noexcept {
  size_t iter1 = 0;
  size_t iter2 = 0;
  while (iter1 < block_1_size || iter2 < block_2_size) {
    if (iter1 == block_1_size) {
      to.write_current(tmp2.read_current());
      tmp2.one_step(DOWN);
      to.one_step(DOWN);
      ++iter2;
      continue;
    }
    if (iter2 == block_2_size) {
      to.write_current(tmp1.read_current());
      tmp1.one_step(DOWN);
      to.one_step(DOWN);
      ++iter1;
      continue;
    }
    auto current_1 = tmp1.read_current();
    auto current_2 = tmp2.read_current();
    if (current_1 < current_2) {
      to.write_current(current_1);
      tmp1.one_step(DOWN);
      to.one_step(DOWN);
      ++iter1;
    } else {
      to.write_current(current_2);
      tmp2.one_step(DOWN);
      to.one_step(DOWN);
      ++iter2;
    }
  }
}
