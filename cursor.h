#pragma once
#include <cstdint>

namespace sqlite {
	class Table;

	class Cursor {
	private:
		Table * table;
		uint32_t page_num;
		uint32_t cell_num;
		bool end_of_table;

	public:
		Cursor(Table* _table, uint32_t _page_num, uint32_t _cell_num);

		uint32_t get_page_num() const;

		uint32_t get_cell_num() const;

		void advance();

		void * value();

		bool is_end() const;

		Table * get_table_ptr() const;

		static Cursor table_start(Table* _table);

		static Cursor find_room(Table * _table, uint32_t key);
	};
}