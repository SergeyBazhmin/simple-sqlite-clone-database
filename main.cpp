#include <memory>
#include <iostream>
#include "command.h"
#include "table.h"
#include "enumerates.h"
#include "statement.h"

std::string buffer;

inline void print_prompt() {
	std::cout << "db > ";
}

inline void read_input() {
	std::getline(std::cin, buffer);
}

inline bool starts_with_dot() {
	return buffer[0] == '.';
}

using namespace sqlite;
int main(int argc, char * argv[]) {

	if (argc != 2) {
		return 1;
	}
	auto executor = CommandExecutor();
	std::string filename = std::string(argv[1]);
	auto table = Table(filename);
	while (true) {
		print_prompt();
		read_input();
		if (starts_with_dot()) {
			switch (executor.execute_meta_command(buffer, table)) {
				case META_COMMAND_SUCCESS:
					continue;
				case META_COMMAND_UNRECOGNIZED_COMMAND:
					std::cout << "unrecognized command: " << buffer << '\n';
					continue;
			}
		}

		Statement statement;
		switch (executor.prepare_statement(buffer, statement)) {
			case PREPARE_SUCCESS:
				break;
			case PREPARE_SYNTAX_ERROR:
				std::cout << "syntax error\n";
				break;
			case PREPARE_UNRECOGNIZED_STATEMENT:
				std::cout << "unrecognized keyword: " << buffer << '\n';
				continue;
		}
		switch (executor.execute_statement(statement, &table)) {
			case EXECUTE_SUCCESS:
				std::cout << "executed\n";
				break;
			case EXECUTE_TABLE_FULL:
				std::cout << "error: table is full\n";
				break;
			case EXECUTE_DUPLICATE_KEY:
				std::cout << "error: duplicate key\n";
				break;
		}
	}
	return 0;
}