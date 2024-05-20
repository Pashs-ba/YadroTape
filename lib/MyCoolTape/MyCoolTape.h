#pragma once
#include<fstream>
#include "../config/Config.h"
#include "OneStepDirection.h"

class MyCoolTape {
 public:
  MyCoolTape(std::fstream tape_file,
             int32_t current_pos,
             Config config) : tape_file_{std::move(tape_file)},
                              current_pos_(current_pos),
                              config_(config) {}
  uint32_t read_current() noexcept;
  void write_current(uint32_t num) noexcept;
  void one_step(OneStepDirection direction) noexcept;
  void long_step(OneStepDirection direction, int32_t count) noexcept;

 private:
  std::fstream tape_file_;
  int32_t current_pos_;
  Config config_;
};