#include <iostream>
#include <fstream>
#include "lib/MyCoolTape/MyCoolTape.h"
#include "lib/sorter/Sorter.h"
#include<vector>

void generator(size_t N) {
  std::fstream tape_file;
  tape_file.open("../../from.tape", std::ios::out | std::ios::binary);
  srand(time(nullptr));
  for (size_t i = 0; i < N; ++i) {
    uint32_t num = rand() % 10000;
    std::cout << num << '\n';
    tape_file.write(reinterpret_cast<char*>(&num), sizeof(uint32_t));
  }
}

int main() {
  generator(1002);
  Config config = {
      0,
      0,
      0,
      0,
      800,
      256 * 4,
      "../../tmp/"
  };
  std::fstream from_file;
  from_file.open("../../from.tape", std::ios::in | std::ios::binary);
  std::fstream to_file;
  to_file.open("../../to.tape", std::ios::out | std::ios::binary);
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
//  std::fstream end2;
//  i = 1;
//  end2.open("../../tmp/tmp4.tape", std::ios::in | std::ios::binary);
//  std::cout << "Sorted2:\n";
//  while (!end2.eof()) {
//    uint32_t num;
//    end2.read(reinterpret_cast<char*>(&num), sizeof(num));
//    std::cout << i << ' ' << num << '\n';
//    i++;
//  }
  return 0;
}
