#pragma once
#include <cstdint>
#include "enumerates.h"

namespace sqlite {
	enum NodeType;
	class Cursor;
	class Table;

	class Node {
	protected:
		void * node;
	public:
		Node() = default;
		Node(void * _node) : node(_node) {}

		static Node * get_page_node(void * page);

		static NodeType load_type(void * node);

		void * get_page() const;

		bool is_root_node() const;

		void set_root_node(bool state);

		NodeType node_type() const;

		void set_node_type(NodeType type);

		virtual void init() = 0;

		virtual uint32_t n_cells() const = 0;

		virtual void set_n_cells(uint32_t _n_cells) = 0;

		virtual void * cell(uint32_t cell_num) const = 0;

		virtual uint32_t key(uint32_t cell_num) const = 0;

		virtual void set_key(uint32_t cell_num, uint32_t _key) = 0;

		virtual uint32_t max_key() const;

		virtual Cursor find_cell(Table * table, uint32_t page_num, uint32_t key) const = 0;
	};

	class Leaf : public Node {
	public:
		Leaf(void * node) : Node(node) {}

		virtual void init() override;

		uint32_t next_leaf() const;

		void set_next_leaf(uint32_t next_leaf);

		virtual uint32_t n_cells() const override;

		virtual void set_n_cells(uint32_t _n_cells) override;

		virtual void * cell(uint32_t cell_num) const override;

		virtual uint32_t key(uint32_t cell_num) const override;

		virtual void set_key(uint32_t cell_num, uint32_t _key) override;

		void * value(uint32_t cell_num) const;

		virtual Cursor find_cell(Table * table, uint32_t page_num, uint32_t key) const override;
	};

	class InternalNode : public Node {
	public:
		InternalNode(void * node) : Node(node) {}

		virtual void init() override;

		virtual uint32_t n_cells() const override;

		virtual void set_n_cells(uint32_t _n_cells) override;

		virtual void * cell(uint32_t cell_num) const override;

		virtual uint32_t key(uint32_t cell_num) const override;

		virtual void set_key(uint32_t cell_num, uint32_t _key) override;

		uint32_t right_child() const;

		void set_right_child(uint32_t child);

		uint32_t value(uint32_t cell_num) const;

		void set_value(uint32_t cell_num, uint32_t val);

		virtual Cursor find_cell(Table * table, uint32_t page_num, uint32_t key) const override;
	};
}