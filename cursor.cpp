#include "table.h"
#include "pager.h"
#include "cursor.h"
#include "node.h"
#include "enumerates.h"
#include <memory>

namespace sqlite {
	Cursor::Cursor(Table * _table, uint32_t _page_num, uint32_t _cell_num)
	{
		table = _table;
		page_num = _page_num;
		cell_num = _cell_num;
		auto pager = _table->get_pager();
		//if stack variables
		auto node = std::unique_ptr<Node>(Node::get_page_node(pager->get_page(page_num)));
		uint32_t num_cells = node->n_cells();
		end_of_table = num_cells == 0;
	}

	uint32_t Cursor::get_page_num() const
	{
		return page_num;
	}

	uint32_t Cursor::get_cell_num() const
	{
		return cell_num;
	}

	void Cursor::advance()
	{
		auto pager = table->get_pager();
		auto node = Leaf(pager->get_page(page_num));
		++cell_num;
		if (cell_num == node.n_cells()) {
			uint32_t next_page = node.next_leaf();
			if (!next_page) {
				end_of_table = true;
			}
			else {
				page_num = next_page;
				cell_num = 0;
			}
		}
	}

	void * Cursor::value()
	{
		auto pager = table->get_pager();
		auto page = pager->get_page(page_num);
		auto node = std::unique_ptr<Node>(Node::get_page_node(page));
		//if stack variables
		return node->cell(cell_num);
	}

	bool Cursor::is_end() const
	{
		return end_of_table;
	}

	Table * Cursor::get_table_ptr() const
	{
		return table;
	}

	Cursor Cursor::table_start(Table * _table)
	{
		return find_room(_table, 0);
	}

	Cursor Cursor::find_room(Table * _table, uint32_t key)
	{
		auto root_page_num = _table->get_root_page_num();
		auto pager = _table->get_pager();
		auto page = pager->get_page(root_page_num);
		//if ... stack variables
		auto node = std::unique_ptr<Node>(Node::get_page_node(pager->get_page(root_page_num)));
		return node->find_cell(_table, root_page_num, key);
	}
}