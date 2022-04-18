#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP
#include <concepts>
#include <iterator>

namespace algorithms {

	// �������� all_of, ���������� true, ���� ��� �������� ��������� �������������
	// ���������� ���������. ����� false
	template<std::input_iterator InputIt, typename Pred>
	bool all_of(InputIt first, InputIt last, Pred func) {
		while (first != last) {
			if (!func(*first)) {
				return false;
			}
			++first;
		}
		return true;
	}

	// �������� any_of - ���������� true, ���� ���� �� ���� �� ��������� ���������
	// ������������� ���������� ���������. ����� false
	template<std::input_iterator InputIt, typename Pred>
	bool any_of(InputIt first, InputIt last, Pred func) {
		while (first != last) {
			if (func(*first)) {
				return true;
			}
			++first;
		}
		return false;
	}

	// �������� none_of - ���������� true, ���� ��� �������� ��������� �� �������������
	// ���������� ���������. ����� false
	template<std::input_iterator InputIt, typename Pred>
	bool none_of(InputIt first, InputIt last, Pred func) {
		while (first != last) {
			if (func(*first)) {
				return false;
			}
			++first;
		}
		return true;
	}

	// �������� one_of - ���������� true, ���� ����� ���� ������� ��������� �������������
	// ���������� ���������. ����� false
	template<std::input_iterator InputIt, typename Pred>
	bool one_of(InputIt first, InputIt last, Pred func) {
		int counter = 0;
		while (first != last) {
			if (func(*first)) {
				if (counter != 0) {
					return false;
				}
				++counter;
			}
			++first;
		}
		if (counter == 1) {
			return true;
		}
		return false;
	}
	
	// �������� is_sorted - ���������� true, ���� ��� �������� ��������� ��������� �
	// ��������������� ������� ������������ ���������� ��������
	template<std::forward_iterator ForwardIt, typename Comp = std::less<>>
	bool is_sorted(ForwardIt first, ForwardIt last, Comp func = Comp()) {
		if (first != last) {
			ForwardIt curr = first;
			while (++curr != last) {
				if (func(*curr, *first)) {
					return false;
				}
				first = curr;
			}
		}
		return true;
	}

	// �������� is_partitioned - ���������� true, ���� � ��������� ���� �������, ������� ���
	// �������� �� ��������������� � �� ��������������� ���������� ���������. ����� false.
	template<std::input_iterator InputIt, typename Pred>
	bool is_partitioned(InputIt first, InputIt last, Pred func) {
		if (first != last) {
			bool satisfies = func(*first);
			++first;
			while (first != last) {
				if (func(*first) != satisfies) {
					break;
				}
				++first;
			}
			if (first == last) {
				return false;
			}
			while (first != last) {
				if (func(*first) == satisfies) {
					return false;
				}
				++first;
			}
		}
		return true;
	}

	// �������� find_not - ������� ������ �������, �� ������ ���������
	template<typename T, std::input_iterator InputIt>
	InputIt find_not(InputIt first, InputIt last, T const& value) {
		while (first != last) {
			if (*first != value) {
				return first;
			}
			++first;
		}
		return last;
	}

	// �������� find_backward - ������� ������ �������, ������ ���������, � �����
	template<typename T, std::bidirectional_iterator BidirectIt>
	BidirectIt find_backward(BidirectIt first, BidirectIt last, T const& value) {
		BidirectIt last_iter = last;
		if (first != last) {
			while (--last != first) {
				if (*last == value) {
					return last;
				}
			}
		}
		return last_iter;
	}

	// �������� is_palindrome - ���������� true, ���� �������� ������������������ ��������
	// ����������� ������������ ���������� �������. ����� false.
	template<std::bidirectional_iterator BidirectIt>
	bool is_palindrome(BidirectIt first, BidirectIt last) {
		if (first != last) {
			BidirectIt saver = first;
			do {
				--last;
				if (*first != *last) {
					return false;
				}
				++first;
			} while (last != saver);
		}
		return true;
	}

	// �������� is_palindrome - ���������� true, ���� �������� ������������������ ��������
	// ����������� ������������ ���������� �������. ����� false.
	template<std::bidirectional_iterator BidirectIt, typename Pred>
	bool is_palindrome(BidirectIt first, BidirectIt last, Pred func) {
		if (first != last) {
			BidirectIt saver = first;
			do {
				--last;
				if (func(*first) != func(*last)) {
					return false;
				}
				++first;
			} while (last != saver);
		}
		return true;
	}

}

#endif
