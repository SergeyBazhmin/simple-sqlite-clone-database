#pragma once
#include <string>


namespace sqlite {
	enum ExecuteResult;
	enum PrepareResult;
	enum MetaCommandResult;
	struct Statement;
	class Table;

	class CommandExecutor {
	private:
		ExecuteResult execute_insert(Statement & statement, Table* table);

		ExecuteResult execute_select(Statement & statement, Table* table);
	public:
		CommandExecutor() = default;
		PrepareResult prepare_statement(std::string& buffer, Statement& statement);

		MetaCommandResult execute_meta_command(std::string& command, Table& table);

		ExecuteResult execute_statement(Statement& statement, Table* table);
	};
}
