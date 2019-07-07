#include "pager.h"
#include "utils.h"
#include "constants.h"

namespace sqlite {
	Pager::~Pager()
	{
		for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
			if (!isNull(pages[i]))
				::operator delete(pages[i]);
		}
	}

	int Pager::filesize(const std::string & filename)
	{
		std::ifstream in(filename, std::ifstream::binary);
		in.seekg(0, std::ios::end);
		return in.tellg();
	}

	Pager * Pager::create_pager(std::string & filename)
	{
		auto pager = new Pager(filename);
		int file_length = filesize(filename);
		pager->file_length = file_length;
		pager->n_pages = file_length / table_constants::PAGE_SIZE;
		for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
			pager->pages[i] = nullptr;
		}
		return pager;
	}

	void * Pager::get_page(uint32_t page_num)
	{
		using namespace table_constants;

		if (page_num > TABLE_MAX_PAGES) {
			throw std::exception("too many pages");
		}

		if (isNull(pages[page_num])) {
			auto page = ::operator new(PAGE_SIZE);
			if (page_num <= n_pages) {
				fs.seekg(page_num * PAGE_SIZE, std::ios::beg);
				fs.read(static_cast<char*>(page), PAGE_SIZE);
				std::streamsize bytes = fs.gcount();
				fs.clear();
				if (bytes == -1) {
					throw std::exception("error reading file:");
				}
			}
			pages[page_num] = page;
			if (page_num == n_pages)
				++n_pages;
		}

		return pages[page_num];
	}

	uint32_t Pager::get_free_page_num() const
	{
		return n_pages;
	}

	void Pager::flush(uint32_t page_num)
	{
		using namespace table_constants;

		if (isNull(pages[page_num])) {
			throw std::exception("attempt to flush nullptr page");
		}
		fs.seekp(page_num * PAGE_SIZE, std::ios::beg);
		fs.write(static_cast<char*>(pages[page_num]), PAGE_SIZE);
		fs.flush();
	}
}