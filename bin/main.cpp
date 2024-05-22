#include <iostream>
#include <fstream>
#include "lib/MyCoolTape/MyCoolTape.h"
#include "lib/sorter/Sorter.h"
#include<vector>
#include<nlohmann/json.hpp>

void generator(size_t N, Config& config) {
  std::fstream tape_file;
  tape_file.open(config.from_tape, std::ios::out | std::ios::binary);
  srand(time(nullptr));
  for (size_t i = 0; i < N; ++i) {
    uint32_t num = rand() % 10000;
    std::cout << num << '\n';
    tape_file.write(reinterpret_cast<char*>(&num), sizeof(uint32_t));
  }
}

Config parse_config(const std::string& path) {
  std::fstream file;
  file.open(path, std::ios::in);
  if (!file.is_open()) {
    std::cerr << "Can't open file " << path << '\n';
    std::exit(1);
  }
  nlohmann::json j;
  file >> j;
  return {
      j["read_wait_ms"],
      j["one_step_ms"],
      j["long_step_ms"],
      j["write_wait_ms"],
      j["N"],
      j["M"],
      j["tmp_folder"],
      j["from_tape"],
      j["to_tape"],
      j["generation_strategy"],
      j["output_strategy"]
  };
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <config file>\n";
    return 1;
  }
  Config config = parse_config(argv[1]);
  if (config.generation_strategy == "random") {
    generator(config.N, config);
  }
  std::fstream from_file;
  from_file.open(config.from_tape, std::ios::in | std::ios::binary);
  std::fstream to_file;
  to_file.open(config.to_tape, std::ios::out | std::ios::binary);
  MyCoolTape from_tape{
      std::move(from_file),
      0,
      config
  };
  MyCoolTape to_tape{
      std::move(to_file),
      0,
      config
  };
  Sorter::sort(from_tape, to_tape, config);
  if (config.output_strategy == "print") {
    std::fstream end;
    end.open("../../to.tape", std::ios::in | std::ios::binary);
    std::cout << "Sorted:\n";
    size_t i = 1;
    while (!end.eof()) {
      uint32_t num;
      end.read(reinterpret_cast<char*>(&num), sizeof(num));
      std::cout << i << ' ' << num << '\n';
      i++;
    }
  }
  return 0;
}
