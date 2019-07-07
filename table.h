#pragma once
#include <cstdint>
#include <string>

namespace sqlite {
	struct Row;
	class Pager;
	class Cursor;
	class Node;

	class Table {
	private:
		uint32_t num_rows;
		Pager* pager;
		uint32_t root_page_num;

		void handle_split(Cursor & cursor, Row & row, void * current_page, uint32_t new_page_num);

		void handle_root_split(uint32_t right_page_num);
	public:
		explicit Table(std::string& filename);

		~Table();

		void serialize_row(Row& source, void * destination) const;

		Row deserialize_row(Cursor& cursor) const;

		void flush() const;

		uint32_t nrows() const;

		void add_row(Cursor& cursor, Row& row);

		uint32_t get_root_page_num() const;

		Pager* get_pager() const;
	};
}
