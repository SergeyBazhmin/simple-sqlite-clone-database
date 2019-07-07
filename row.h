#pragma once
#include <cstdint>
#include "definitions.h"

namespace sqlite {
	struct Row {
		uint32_t id;
		char username[COLUMN_USERNAME_SIZE + 1];
		char email[COLUMN_EMAIL_SIZE + 1];
	};
}