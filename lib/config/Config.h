#pragma once
#include<cstdint>
#include<string>

struct Config {
  int read_wait_ms;
  int write_wait_ms;
  int one_step_ms;
  int long_step_ms;
  uint32_t N;
  uint32_t M;
  std::string tmp_folder;
};
