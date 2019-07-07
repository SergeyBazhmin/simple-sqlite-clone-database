#pragma once
#include "row.h"

namespace sqlite {
	enum StatementType;

	struct Statement {
		StatementType type;
		Row row_to_insert;
	};
}