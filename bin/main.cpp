#include <iostream>
#include <fstream>
#include "lib/MyCoolTape/MyCoolTape.h"

int main() {
  std::fstream tape_file;
  tape_file.open("../../tape.txt", std::ios::in | std::ios::out | std::ios::binary);
  if (!tape_file.is_open()) {
    std::cerr << "Can't open tape file" << std::endl;
    return 1;
  }

  MyCoolTape tape{std::move(tape_file), 0, {100, 100, 100, 10}};
  tape.write_current(42);
  std::cout << tape.read_current() << std::endl;
  tape.one_step(DOWN);
  tape.write_current(1);
  tape.one_step(DOWN);
  tape.write_current(100);
  tape.one_step(DOWN);
  tape.write_current(101);
  tape.long_step(UP, 2);
  std::cout << tape.read_current() << std::endl;

  return 0;
}
