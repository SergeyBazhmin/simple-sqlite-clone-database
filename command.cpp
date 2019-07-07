#include <memory>
#include "enumerates.h"
#include "command.h"
#include "constants.h"
#include "table.h"
#include "pager.h"
#include "cursor.h"
#include "node.h"
#include "statement.h"
#include "utils.h"

namespace sqlite {
	ExecuteResult CommandExecutor::execute_insert(Statement & statement, Table * table)
	{
		auto pager = table->get_pager();
		auto page = pager->get_page(table->get_root_page_num());
		//if stack variables
		auto node = std::unique_ptr<Node>(Node::get_page_node(page));
		uint32_t n_cells = node->n_cells();

		uint32_t key_to_insert = statement.row_to_insert.id;
		auto cursor = Cursor::find_room(table, key_to_insert);

		uint32_t key_at_index = node->key(cursor.get_cell_num());
		if (key_at_index == key_to_insert)
			return EXECUTE_DUPLICATE_KEY;

		table->add_row(cursor, statement.row_to_insert);
		return EXECUTE_SUCCESS;
	}

	ExecuteResult CommandExecutor::execute_select(Statement & statement, Table * table)
	{
		auto cursor = Cursor::table_start(table);
		auto page_num = cursor.get_page_num();
		auto pager = table->get_pager();
		auto page = pager->get_page(page_num);
		auto node = Leaf(page);

		//if stack variables
		while(!cursor.is_end()) {
			auto row = table->deserialize_row(cursor);
			std::cout << row.id << ' ' << row.username << ' ' << row.email << '\n';
			cursor.advance();
		}
		return EXECUTE_SUCCESS;
	}

	PrepareResult CommandExecutor::prepare_statement(std::string & buffer, Statement & statement)
	{
		auto tokens = split(buffer);
		auto command = tokens.front();
		if (command == "insert") {
			if (tokens.size() != 4) return PREPARE_SYNTAX_ERROR;
			statement.row_to_insert.id = std::stoi(tokens[1]);
			strcpy_s(statement.row_to_insert.username, tokens[2].c_str());
			strcpy_s(statement.row_to_insert.email, tokens[3].c_str());
			statement.type = STATEMENT_INSERT;
			return PREPARE_SUCCESS;
		}
		if (command == "select") {
			statement.type = STATEMENT_SELECT;
			return PREPARE_SUCCESS;
		}

		return PREPARE_UNRECOGNIZED_STATEMENT;
	}

	MetaCommandResult CommandExecutor::execute_meta_command(std::string & command, Table & table)
	{
		if (command == ".exit") {
			table.flush();
			exit(EXIT_SUCCESS);
		}
		else {
			return META_COMMAND_UNRECOGNIZED_COMMAND;
		}
	}

	ExecuteResult CommandExecutor::execute_statement(Statement & statement, Table * table)
	{
		switch (statement.type) {
		case STATEMENT_INSERT:
			return execute_insert(statement, table);
		case STATEMENT_SELECT:
			return execute_select(statement, table);
		}
	}
}