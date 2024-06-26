#include <vector>
#include <string_view>
#include <span>
#include <array>
#include <ranges>
#include <utility>
#include <thread>
#include <cmath>
#include <algorithm>
#include <iterator>

#include "wss_export.h"

// LabVIEW on Windows 32-bit expects data to have
// a byte-alignment of 1
#ifdef ALIGN_BYTES_1
#pragma pack(push, 1)
#pragma warning(disable : 4103)
#endif

// define types to exchange with LabVIEW

// row/col cluster
using lv_element_t = struct
{
	int32_t row, col;
};

// LabVIEW string
using lv_str_t = struct
{
	int32_t length;
	char buffer[1];
};

// A LabVIEW array of LabVIEW string-handles
using lv_words_list_t = struct
{
	int32_t length;
	lv_str_t **word_handles[1];
};

// A LabVIEW array of row/col clusters
using lv_result_t = struct
{
	int32_t length;
	lv_element_t elements[1];
};

// A LabVIEW array of row/cluster array-handles
using lv_results_list_t = struct
{
	int32_t length;
	lv_result_t **result_array_handles[1];
};

#ifdef ALIGN_BYTES_1
#pragma pack(pop)
#endif


// find a single word in the board
void inline find_word(
	const std::span<char> &board,
	const int32_t rows,
	const int32_t cols,
	const std::string_view &word,
	std::span<lv_element_t> &result)
{
	if (word.empty())
	{
		return;
	}

	// use std::find to search for the next character in the board-span that matches the start character of the word
	// initialize start with location of first match
	// pre-body action: check that the first match location is not equal to the end of board (board.end() is the first out-of-bounds element)
	// post-body action: search again (the potential_match_iterator iterator will have been incremented by the body if the word wasn't found in any search direction)
	for (
		std::span<char>::iterator potential_match_iterator = std::find(board.begin(), board.end(), word[0]);
		potential_match_iterator != board.end();
		potential_match_iterator = std::find(potential_match_iterator, board.end(), word[0]))
	{
		// compute row and col
		// start with the distance that potential_match_iterator is now at (this is the index offset)
		int32_t distance = static_cast<int32_t>(std::distance(board.begin(), potential_match_iterator));
		// use quotient/remainder divide the distance by the number of columns to get the row and column index
		div_t position = std::div(distance, cols);
		int32_t row = position.quot;
		int32_t col = position.rem;

		result[0].row = row;
		result[0].col = col;

		// define a list of directions to look in
		const std::array<const lv_element_t,8> directions{{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

		// check the possible directions to see if one matches
		for (auto const &dir : directions)
		{
			bool found = true;
			for (size_t ch = 1; ch < word.length(); ch++)
			{
				// compute the index of the board to check next
				int32_t row_index = row + ch * dir.row;
				int32_t col_index = col + ch * dir.col;
				// check if the index is in bounds and then do the comparison
				// these OR operations are chained so the board[index] access will only happen if we are in bounds
				if (row_index < 0 || row_index >= rows || col_index < 0 || col_index >= cols || board[row_index * cols + col_index] != word[ch])
				{
					// no-match in this direction
					found = false;
					break;
				}
				// still matching
				result[ch].row = row_index;
				result[ch].col = col_index;
			}
			if (found)
			{
				return;
			}
		}
		// increment the potential_match_iterator position
		potential_match_iterator++;
	}
}

// find multiple words
void inline find_words(
	const std::span<char> &board,
	const int32_t rows,
	const int32_t cols,
	const lv_words_list_t **word_list_handle,
	lv_results_list_t **result_list_handle,
	int32_t from,
	int32_t up_to)
{
	// for each word
	for (int32_t i = from; i < up_to; i++)
	{
		// get the length of the string handle relating to this word
		lv_str_t **word = (*word_list_handle)->word_handles[i];
		int32_t word_length = (word && *word) ? (*word)->length : 0;

		// get the handle to the array that this word's result should be place into
		lv_result_t **result_array_handle = (*result_list_handle)->result_array_handles[i];

		// we should probably check that the results_array_handle->length == word_length but lets just hope that the LabVIEW code has allocated it correctly

		// wrap the string data into a std::string view (non-owning c++ collection wrapping the underlying LabVIEW-allocated memory)
		std::string_view word_string_view(&((*word)->buffer[0]), word_length);
		// wrap the result-array into a std::span (non-owning c++ collection wrapping the underlying LabVIEW-allocated memory)
		std::span<lv_element_t> result_span(&((*result_array_handle)->elements[0]), word_length);

		// find the word, write the result into the result_span (so directly into the memory that LabVIEW has already allocated)
		find_word(board, rows, cols, word_string_view, result_span);
	}
}

extern "C"
{
	WSS_EXPORT void solve(char *board_ptr, const lv_words_list_t **word_list_handle, lv_results_list_t **result_list_handle, const int32_t board_rows, const int32_t board_cols, const int32_t n_threads)
	{
		// wrap the board_ptr into a std::span container so we can use the std::collection API on it
		std::span<char> board(board_ptr, board_rows * board_cols);

		// read the length of the word list from the supplied handle (check handle is valid first)
		int32_t length_of_word_list = (word_list_handle && *word_list_handle) ? (*word_list_handle)->length : 0;

		// create a collection of threads and reserve the memory ready to have threads inserted
		std::vector<std::thread> threads;
		threads.reserve(n_threads);

		// calculate roughly how the string list should be split between the threads
		std::div_t qr = std::div(length_of_word_list, n_threads);
		int32_t words_per_thread = qr.quot + std::min(qr.rem, 1);

		// create each thread in the threads-vector
		int32_t from = 0;
		while (from < length_of_word_list)
		{
			int32_t up_to = std::min(from + words_per_thread, length_of_word_list);
			threads.emplace_back(find_words, board, board_rows, board_cols, word_list_handle, result_list_handle, from, up_to);
			from = up_to;
		}

		// wait on all the threads to finish
		for (auto &th : threads)
		{
			th.join();
		}
	}
}