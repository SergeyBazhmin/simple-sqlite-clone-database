#include <cstdint>
#include "node.h"
#include "cursor.h"
#include "utils.h"
#include "table.h"
#include "pager.h"
#include "constants.h"
#include "enumerates.h"

namespace sqlite {
	Node * Node::get_page_node(void * page)
	{
		auto type = Node::load_type(page);
		if (type == NODE_LEAF)
			return new Leaf(page);
		return new InternalNode(page);
	}

	NodeType Node::load_type(void * node)
	{
		uint8_t value = *reinterpret_cast<uint8_t *>(increment_void_ptr(node, node_header::NODE_TYPE_OFFSET));
		return static_cast<NodeType>(value);
	}
	void * Node::get_page() const
	{
		return node;
	}

	bool Node::is_root_node() const
	{
		auto casted_ptr = reinterpret_cast<uint8_t*>(increment_void_ptr(node, node_header::IS_ROOT_OFFSET));
		return static_cast<bool>(*casted_ptr);
	}

	void Node::set_root_node(bool state)
	{
		auto casted_ptr = reinterpret_cast<uint8_t*>(increment_void_ptr(node, node_header::IS_ROOT_OFFSET));
		*casted_ptr = static_cast<uint8_t>(state);
	}

	NodeType Node::node_type() const
	{
		uint8_t value = *reinterpret_cast<uint8_t *>(increment_void_ptr(node, node_header::NODE_TYPE_OFFSET));
		return static_cast<NodeType>(value);
	}

	void Node::set_node_type(NodeType type)
	{
		auto ptr = reinterpret_cast<uint8_t*>(increment_void_ptr(node, node_header::NODE_TYPE_OFFSET));
		*ptr = type;
	}

	uint32_t Node::max_key() const
	{
		return key(n_cells() - 1);
	}

	void Leaf::init()
	{
		set_node_type(NODE_LEAF);
		set_n_cells(0);
		set_next_leaf(0);
	}

	uint32_t Leaf::next_leaf() const
	{
		auto casted_ptr = reinterpret_cast<uint32_t*>(increment_void_ptr(node, leaf_node::header::NEXT_LEAF_OFFSET));
		return *casted_ptr;
	}

	void Leaf::set_next_leaf(uint32_t next_leaf)
	{
		auto casted_ptr = reinterpret_cast<uint32_t*>(increment_void_ptr(node, leaf_node::header::NEXT_LEAF_OFFSET));
		*casted_ptr = next_leaf;
	}

	uint32_t Leaf::n_cells() const
	{
		auto casted_ptr = increment_void_ptr(node, leaf_node::header::NUM_CELLS_OFFSET);
		return *reinterpret_cast<uint32_t *>(casted_ptr);
	}

	void Leaf::set_n_cells(uint32_t _n_cells)
	{
		auto casted_ptr = reinterpret_cast<uint32_t *>(increment_void_ptr(node, leaf_node::header::NUM_CELLS_OFFSET));
		*casted_ptr = _n_cells;
	}

	void * Leaf::cell(uint32_t cell_num) const
	{
		using namespace leaf_node;
		return increment_void_ptr(node, header::HEADER_SIZE + body::CELL_SIZE * cell_num);
	}

	uint32_t Leaf::key(uint32_t cell_num) const
	{
		return *reinterpret_cast<uint32_t *>(cell(cell_num));
	}

	void Leaf::set_key(uint32_t cell_num, uint32_t _key)
	{
		auto casted_ptr = reinterpret_cast<uint32_t *>(cell(cell_num));
		*casted_ptr = _key;
	}

	void * Leaf::value(uint32_t cell_num) const
	{
		return increment_void_ptr(cell(cell_num), leaf_node::body::VALUE_OFFSET);
	}

	Cursor Leaf::find_cell(Table * table, uint32_t page_num, uint32_t _key) const
	{
		uint32_t l = 0;
		uint32_t r = n_cells();
		while (l != r) {
			uint32_t mid = (r + l) >> 1;
			uint32_t k = key(mid);
			/*if (k == _key) {
				return Cursor(table, page_num, mid);
			}*/
			if (_key <= k) {
				r = mid;
			}
			else {
				l = mid + 1;
			}
		}
		return Cursor(table, page_num, l);
	}

	void InternalNode::init()
	{
		set_node_type(NODE_INTERNAL);
		set_n_cells(0);
	}

	uint32_t InternalNode::right_child() const
	{
		auto casted_ptr = reinterpret_cast<uint32_t*>(increment_void_ptr(node, internal_node::header::RIGHT_CHILD_OFFSET));
		return *casted_ptr;
	}

	void InternalNode::set_right_child(uint32_t child)
	{
		auto casted_ptr = increment_void_ptr(node, internal_node::header::RIGHT_CHILD_OFFSET);
		*casted_ptr = child;
	}

	uint32_t InternalNode::n_cells() const
	{
		auto casted_ptr = increment_void_ptr(node, internal_node::header::NUM_KEYS_OFFSET);
		return *reinterpret_cast<uint32_t *>(casted_ptr);
	}

	void InternalNode::set_n_cells(uint32_t _n_cells)
	{
		auto casted_ptr = increment_void_ptr(node, internal_node::header::NUM_KEYS_OFFSET);
		*casted_ptr = _n_cells;
	}

	void * InternalNode::cell(uint32_t cell_num) const
	{
		using namespace internal_node;
		auto casted_ptr = increment_void_ptr(node, header::HEADER_SIZE + body::CELL_SIZE * cell_num);
		return casted_ptr;
	}
	uint32_t InternalNode::key(uint32_t cell_num) const
	{
		using namespace internal_node;
		auto casted_ptr = increment_void_ptr(node, header::HEADER_SIZE + cell_num*body::CELL_SIZE + body::KEY_OFFSET);
		return *reinterpret_cast<uint32_t*>(casted_ptr);
	}
	void InternalNode::set_key(uint32_t cell_num, uint32_t _key)
	{
		using namespace internal_node;
		auto casted_ptr = increment_void_ptr(node, header::HEADER_SIZE + cell_num * body::CELL_SIZE + body::KEY_OFFSET);
		*casted_ptr = _key;
	}
	uint32_t InternalNode::value(uint32_t cell_num) const
	{
		using namespace internal_node;
		if (cell_num == n_cells())
			return right_child();
		auto casted_ptr = increment_void_ptr(node, header::HEADER_SIZE + cell_num * body::CELL_SIZE + body::CHILD_OFFSET);
		return *reinterpret_cast<uint32_t*>(casted_ptr);
	}
	void InternalNode::set_value(uint32_t cell_num, uint32_t val)
	{
		using namespace internal_node;
		auto casted_ptr = reinterpret_cast<uint32_t*>(increment_void_ptr(node, header::HEADER_SIZE + cell_num * body::CELL_SIZE + body::CHILD_OFFSET));
		*casted_ptr = val;
	}
	Cursor InternalNode::find_cell(Table * table, uint32_t page_num, uint32_t _key) const
	{
		uint32_t l = 0;
		uint32_t r = n_cells();
		while (l != r) {
			uint32_t index = (l + r) >> 1;
			uint32_t key_at_index = key(index);
			if (key_at_index >= _key)
				r = index;
			else
				l = index + 1;
		}
		uint32_t child_no = value(l);
		auto child = table->get_pager()->get_page(child_no);
		auto type = Node::load_type(child);
		switch (type) {
		case NODE_LEAF:
			return Leaf(child).find_cell(table, child_no, _key);
		case NODE_INTERNAL:
			return InternalNode(child).find_cell(table, child_no, _key);
		}
	}
}