#pragma once

namespace sqlite {
	enum MetaCommandResult {
		META_COMMAND_SUCCESS,
		META_COMMAND_UNRECOGNIZED_COMMAND
	};

	enum PrepareResult {
		PREPARE_SUCCESS,
		PREPARE_SYNTAX_ERROR,
		PREPARE_UNRECOGNIZED_STATEMENT
	};

	enum ExecuteResult {
		EXECUTE_SUCCESS,
		EXECUTE_TABLE_FULL,
		EXECUTE_DUPLICATE_KEY
	};

	enum StatementType {
		STATEMENT_INSERT,
		STATEMENT_SELECT
	};

	enum NodeType {
		NODE_INTERNAL,
		NODE_LEAF
	};
}