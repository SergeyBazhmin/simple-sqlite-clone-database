#include <memory>
#include "table.h"
#include "pager.h"
#include "cursor.h"
#include "node.h"
#include "constants.h"
#include "row.h"
#include "utils.h"

namespace sqlite {
	void Table::serialize_row(Row & source, void * destination) const
	{
		using namespace table_constants;
		memcpy(increment_void_ptr(destination, ID_OFFSET), &source.id, ID_SIZE);
		memcpy(increment_void_ptr(destination, USERNAME_OFFSET), &source.username, USERNAME_SIZE);
		memcpy(increment_void_ptr(destination, EMAIL_OFFSET), &source.email, EMAIL_SIZE);
	}

	Row Table::deserialize_row(Cursor & cursor) const
	{
		using namespace table_constants;
		Row destination;
		auto source = cursor.value();
		memcpy(&destination.id, increment_void_ptr(source, ID_OFFSET), ID_SIZE);
		memcpy(&destination.username, increment_void_ptr(source, USERNAME_OFFSET), USERNAME_SIZE);
		memcpy(&destination.email, increment_void_ptr(source, EMAIL_OFFSET), EMAIL_SIZE);
		return destination;
	}

	void Table::handle_split(Cursor & cursor, Row & row, void * current_page, uint32_t new_page_num)
	{
		using namespace leaf_node::body;
		using namespace leaf_node::split;

		auto current_node = Leaf(current_page);
		auto new_page = pager->get_page(new_page_num);
		auto new_node = Leaf(new_page);
		new_node.init();
		for (int i = MAX_CELLS; i >= 0; --i) {
			void * destination = (i >= LEFT_SPLIT_COUNT) ? new_page : current_node.get_page();
			uint32_t index_within_node = i % LEFT_SPLIT_COUNT;
			//from ptr to stack
			destination = std::unique_ptr<Node>(Node::get_page_node(destination))->cell(index_within_node);
			if (i == cursor.get_cell_num())
				serialize_row(row, destination);
			else if (i > cursor.get_cell_num())
				memcpy(destination, current_node.cell(i - 1), CELL_SIZE);
			else
				memcpy(destination, current_node.cell(i), CELL_SIZE);
		}
		current_node.set_n_cells(LEFT_SPLIT_COUNT);
		new_node.set_n_cells(RIGHT_SPLIT_COUNT);

		if (current_node.is_root_node()) {
			handle_root_split(new_page_num);
		}
	}

	void Table::handle_root_split(uint32_t right_page_num)
	{
		void * root = pager->get_page(root_page_num); 
		void * right_child = pager->get_page(right_page_num);

		uint32_t left_child_page_num = pager->get_free_page_num();
		void * left_child_page = pager->get_page(left_child_page_num);
		memcpy(left_child_page, root, table_constants::PAGE_SIZE);

		
		auto left_child = Leaf(left_child_page);
		left_child.set_next_leaf(right_page_num);
		left_child.set_root_node(false);

		auto new_root_node = InternalNode(root);
		new_root_node.set_node_type(NODE_INTERNAL);
		new_root_node.set_root_node(true);
		new_root_node.set_n_cells(1);
		new_root_node.set_key(0, left_child.max_key());
		new_root_node.set_value(0, left_child_page_num);
		new_root_node.set_right_child(right_page_num);
	}

	Table::Table(std::string & filename)
	{
		pager = Pager::create_pager(filename);
		root_page_num = 0;
		if (pager->n_pages == 0) {
			auto root_node = Leaf(pager->get_page(root_page_num));
			root_node.init();
			root_node.set_root_node(true);
		}
	}

	Table::~Table()
	{
		delete pager;
	}

	void Table::flush() const
	{
		for (uint32_t i = 0; i < pager->n_pages; ++i) {
			if (isNull(pager->pages[i])) {
				break;
			}
			pager->flush(i);
		}
	}

	uint32_t Table::nrows() const
	{
		return num_rows;
	}

	void Table::add_row(Cursor & cursor, Row & row)
	{
		using namespace leaf_node::body;

		auto current_page = pager->get_page(cursor.get_page_num());
		auto current_node = std::unique_ptr<Node>(Node::get_page_node(current_page));
		++num_rows;
		uint32_t n_cells = current_node->n_cells();
		if (n_cells == MAX_CELLS) {
			uint32_t new_page_num = pager->get_free_page_num();
			handle_split(cursor, row, current_page, new_page_num);
		}
		else {
			for (uint32_t cell_idx = n_cells; cell_idx > cursor.get_cell_num(); --cell_idx) {
				memcpy(
					current_node->cell(cell_idx),
					current_node->cell(cell_idx - 1),
					CELL_SIZE
				);
			}
			current_node->set_n_cells(current_node->n_cells() + 1);
			serialize_row(row, current_node->cell(cursor.get_cell_num()));
		}
	}

	uint32_t Table::get_root_page_num() const
	{
		return root_page_num;
	}

	Pager * Table::get_pager() const
	{
		return pager;
	}
}