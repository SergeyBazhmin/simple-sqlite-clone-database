#pragma once
#include <vector>
#include <iostream>
#include <sstream>

#define size_of_attribute(Struct, Attribute) sizeof((static_cast<Struct*>(0))->Attribute)
#define increment_void_ptr(Ptr, offset) static_cast<char *>(Ptr) + offset
#define isNull(ptr) (ptr == nullptr)

namespace sqlite {
	std::vector<std::string> split(std::string& str);
}
