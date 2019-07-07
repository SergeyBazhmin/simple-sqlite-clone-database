#pragma once
#include <cstdint>
#include <fstream>
#include <array>
#include "definitions.h"

namespace sqlite {
	class Pager {
	private:
		std::fstream fs;
		uint32_t file_length;
	public:
		uint32_t n_pages;
		std::array<void *, TABLE_MAX_PAGES> pages;

		Pager(const std::string& filename) : fs(filename, std::ios::in | std::ios::app) {}
		~Pager();

		static int filesize(const std::string& filename);

		static Pager * create_pager(std::string& filename);

		void * get_page(uint32_t page_num);

		uint32_t get_free_page_num() const;

		void flush(uint32_t page_num);
	};
}