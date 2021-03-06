#include <iostream>

#include "nbt.h"

nbt::Byte *blocks;
nbt::Byte *skylight;
nbt::Byte *heightmap;
nbt::Byte *blocklight;
nbt::Byte *data;
nbt::Int data_length;
nbt::Int xPos;
nbt::Int zPos;

void begin_compound(nbt::String name) {
  std::cout << "TAG_Compound('" << name << "') BEGIN" << std::endl;
}

void end_compound() {
  std::cout << "TAG_Compound END" << std::endl;
}

void register_long(nbt::String name, nbt::Long l) {
  std::cout << "TAG_Long('" << name << "'): " << l << std::endl;
}

void register_short(nbt::String name, nbt::Short s) {
  std::cout << "TAG_Short('" << name << "'): " << (int)s << std::endl;
}

void register_string(nbt::String name, nbt::String s) {
  std::cout << "TAG_String('" << name << "'): " << s << std::endl;
}

void register_float(nbt::String name, nbt::Float f) {
  std::cout << "TAG_Float('" << name << "'): " << f << std::endl;
}

void register_double(nbt::String name, nbt::Double f) {
  std::cout << "TAG_Double('" << name << "'): " << f << std::endl;
}

void register_int(nbt::String name, nbt::Int f) {
  std::cout << "TAG_Int('" << name << "'): " << f << std::endl;
}

void register_byte(nbt::String name, nbt::Byte f) {
  //std::cout << "TAG_Byte('" << name << "'): " << (int)f << std::endl;
}

void register_byte_array(nbt::String name, nbt::Int length, nbt::Byte *a) {
  std::cout << "TAG_Byte_Array('" << name << "'): " << length << std::endl;
  for (int i = 0; i < length && i < 10; i++) {
    std::cout << "   " << (int)a[i] << std::endl;
  }
}

void begin_list(nbt::String name, nbt::Byte type, nbt::Int length) {
  std::cout << "TAG_List('" << name << "'): " << length << " items" << std::endl;
}

void end_list() {
  std::cout << "TAG_List END" << std::endl;
}


using namespace std;

int main(int argc, char *argv[]) {
  nbt::Parser parser;

  parser.begin_compound = begin_compound;
  parser.end_compound = end_compound;
  parser.register_long = register_long;
  parser.register_short = register_short;
  parser.register_string = register_string;
  parser.register_float = register_float;
  parser.register_double = register_double;
  parser.register_int = register_int;
  parser.register_byte = register_byte;
  parser.begin_list = begin_list;
  parser.end_list = end_list;
  parser.parse_file(argv[1]);

  cout << xPos << ":" << zPos << endl;
}
