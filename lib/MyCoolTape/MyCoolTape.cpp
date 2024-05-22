//
// Created by pashs on 20.05.2024.
//

#include <thread>
#include "MyCoolTape.h"
uint32_t MyCoolTape::read_current() noexcept {
  std::this_thread::sleep_for(std::chrono::milliseconds(config_.read_wait_ms));
  uint32_t num;
  tape_file_.seekg(current_pos_ * sizeof(uint32_t));
  tape_file_.read(reinterpret_cast<char*>(&num), sizeof(uint32_t));
  return num;
}
void MyCoolTape::one_step(OneStepDirection direction) noexcept {
  std::this_thread::sleep_for(std::chrono::milliseconds(config_.one_step_ms));
  if (direction == UP) {
    current_pos_ -= 1;
  } else {
    current_pos_ += 1;
  }
}
void MyCoolTape::long_step(OneStepDirection direction, int32_t count) noexcept {
  if (count == 1) {
    one_step(direction);
    return;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(config_.long_step_ms * count));
  if (direction == UP) {
    current_pos_ -= count;
  } else {
    current_pos_ += count;
  }
}
void MyCoolTape::write_current(uint32_t num) noexcept {
  std::this_thread::sleep_for(std::chrono::milliseconds(config_.write_wait_ms));
  tape_file_.seekg(current_pos_ * sizeof(uint32_t));
  tape_file_.write(reinterpret_cast<char*>(&num), sizeof(uint32_t));
}
void MyCoolTape::rewind() noexcept {
  std::this_thread::sleep_for(std::chrono::milliseconds(config_.long_step_ms * current_pos_));
  tape_file_.seekg(std::ios::beg);
  current_pos_ = 0;

}
int32_t MyCoolTape::get_current_position() noexcept {
  return current_pos_;
}



