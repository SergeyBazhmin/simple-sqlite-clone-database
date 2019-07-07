#pragma once
#include <cstdint>
#include "row.h"
#include "definitions.h"
#include "utils.h"


namespace sqlite {
	namespace table_constants {
		const uint32_t ID_SIZE = size_of_attribute(Row, id);
		const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
		const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
		const uint32_t ID_OFFSET = 0;
		const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
		const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
		const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
		const uint32_t PAGE_SIZE = 2048;
		const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
		const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;
	}
	namespace node_header {
		const uint32_t NODE_TYPE_SIZE = sizeof uint8_t;
		const uint32_t NODE_TYPE_OFFSET = 0;
		const uint32_t IS_ROOT_SIZE = sizeof uint8_t;
		const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
		const uint32_t PARENT_POINTER_SIZE = sizeof uint32_t;
		const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
		const uint8_t HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;
	}
	namespace leaf_node {
		namespace header {
			const uint32_t NUM_CELLS_SIZE = sizeof uint32_t;
			const uint32_t NUM_CELLS_OFFSET = node_header::HEADER_SIZE;
			const uint32_t NEXT_LEAF_SIZE = sizeof uint32_t;
			const uint32_t NEXT_LEAF_OFFSET = NUM_CELLS_OFFSET + NUM_CELLS_SIZE;
			const uint32_t HEADER_SIZE = node_header::HEADER_SIZE + NUM_CELLS_SIZE + NEXT_LEAF_SIZE;
		}
		namespace body {
			const uint32_t KEY_SIZE = sizeof uint32_t;
			const uint32_t KEY_OFFSET = 0;
			const uint32_t VALUE_SIZE = table_constants::ROW_SIZE;
			const uint32_t VALUE_OFFSET = KEY_SIZE;
			const uint32_t CELL_SIZE = KEY_SIZE + VALUE_SIZE;
			const uint32_t SPACE_FOR_CELLS = table_constants::PAGE_SIZE - header::HEADER_SIZE;
			const uint32_t MAX_CELLS = SPACE_FOR_CELLS / CELL_SIZE;
		}
		namespace split {
			const uint32_t RIGHT_SPLIT_COUNT = (body::MAX_CELLS + 1) / 2;
			const uint32_t LEFT_SPLIT_COUNT = body::MAX_CELLS + 1 - RIGHT_SPLIT_COUNT;
		}
	}
	namespace internal_node {
		namespace header {
			const uint32_t NUM_KEYS_SIZE = sizeof uint32_t;
			const uint32_t NUM_KEYS_OFFSET = node_header::HEADER_SIZE;
			const uint32_t RIGHT_CHILD_SIZE = sizeof uint32_t;
			const uint32_t RIGHT_CHILD_OFFSET = NUM_KEYS_OFFSET + NUM_KEYS_SIZE;
			const uint32_t HEADER_SIZE = node_header::HEADER_SIZE + NUM_KEYS_SIZE + RIGHT_CHILD_SIZE;
		}
		namespace body {
			const uint32_t CHILD_SIZE = sizeof uint32_t;
			const uint32_t CHILD_OFFSET = 0;
			const uint32_t KEY_SIZE = sizeof uint32_t;
			const uint32_t KEY_OFFSET = CHILD_SIZE;
			const uint32_t CELL_SIZE = KEY_SIZE + CHILD_SIZE;
		}
	}
}