#ifndef CYCLIC_BUFFER_HPP
#define CYCLIC_BUFFER_HPP
#include <mutex>
#include <iostream>
#include <type_traits>
#include <memory>
#include <functional>
#include <iterator>
#include <concepts>

// 
template<typename T, typename Allocator>
class Default_delete {
public:
	void operator()(T* p, Allocator a = Allocator(), size_t n = 0) {
		std::cout << "\nDEALLOCS " << typeid(T*).name() << "\n";
		T* tmp = p;
		a.deallocate(p, n);
		std::cout << "DAMN IT\n";
	}
};


template<typename T, typename Allocator = std::allocator<T>, typename Deleter = Default_delete<T, Allocator>>
class Cyclic_buffer {
public:
	template<typename V>
	class Iterator;
	template<typename Iter>
	class Reverse_iterator;
	typedef T value_type;	
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T& reference;
	typedef T const& const_reference;
	//typedef T* pointer;
	typedef typename std::allocator_traits<Allocator>::pointer pointer;
	typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer;
	typedef Allocator allocator_type;
	typedef Deleter deleter_type;
	typedef Iterator<value_type> iterator;
	typedef Iterator<const value_type> const_iterator;
	typedef Reverse_iterator<iterator> reverse_iterator;
	typedef Reverse_iterator<const_iterator> const_reverse_iterator;
	//typedef pointer iterator_type;

	//
	template<std::input_iterator InputIter>
	Cyclic_buffer(InputIter first, InputIter last, allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter())
		: capacity_(std::abs(std::distance(first, last))), size_(capacity_), head_(0), tail_(size_ - 1), alloc_(alloc),
		del_(del), mutex_(), data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		while (first != last) {
			std::allocator_traits<Allocator>::construct(alloc_, p, *first);
			++first;
			++p;
		}
	}

	//
	explicit Cyclic_buffer(allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter())
		: capacity_(0), size_(0), head_(0), tail_(0), alloc_(alloc), del_(del), mutex_(), data_(nullptr, del) {}

	// 
	explicit Cyclic_buffer(size_type capacity, allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter())
		: capacity_(capacity), size_(0), head_(0), tail_(0), alloc_(alloc), del_(del), mutex_(), 
		data_(alloc_.allocate(capacity_), del_) {}

	Cyclic_buffer(size_type size, const_reference value, allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter())
		: capacity_(size), size_(capacity_), head_(0), tail_(size_ - 1), alloc_(alloc), del_(del), mutex_(),
		data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		for (size_t i = 0; i < size_; ++i, ++p) {
			std::allocator_traits<Allocator>::construct(alloc_, p, value);
		}
	}
	//
	Cyclic_buffer(size_type capacity, size_type size, const_reference value, allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter()) 
		: capacity_(capacity), size_((size > capacity) ? capacity_ : size), head_(0), tail_(size_ - 1), alloc_(alloc), del_(del), 
		mutex_(), data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		for (size_t i = 0; i < size_; ++i, ++p) {
			std::allocator_traits<Allocator>::construct(alloc_, p, value);
		}
	}

	//
	Cyclic_buffer(std::initializer_list<value_type> const& list, allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter())
		: capacity_(list.size()), size_(capacity_), head_(0), tail_(size_ - 1), alloc_(alloc), del_(del),
		mutex_(), data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		for (auto i = list.begin(); i != list.end(); ++i, ++p) {
			std::allocator_traits<Allocator>::construct(alloc_, p, *i);
		}
	}

	//
	Cyclic_buffer(size_type capacity, std::initializer_list<value_type> const& list, allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter())
		: capacity_(capacity), size_((list.size() > capacity_) ? capacity_ : list.size()),
		head_(0), tail_(size_ - 1), alloc_(alloc), del_(del), mutex_(),
		data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		auto it = list.begin();
		for (size_t i = 0; i < size_; ++i, ++p, ++it) {
			std::allocator_traits<Allocator>::construct(alloc_, p, *it);
		}
	}

	//
	Cyclic_buffer(Cyclic_buffer const& other)
		: size_(other.size_), capacity_(other.capacity_), head_(other.head_), tail_(other.tail_),
		alloc_(other.alloc_), del_(other.del_) {
		pointer data = alloc_.allocate(capacity_);
		pointer p = data;
		for (size_t i = 0; i < size_; ++i, ++p) {
			std::allocator_traits<Allocator>::construct(alloc_, p, other[i]);
		}
		data_ = { data, del_ };
	}
	//
	void swap(Cyclic_buffer& other) {
		using std::swap;
		if (data_.get() != other.data_.get()) {
			swap(data_, other.data_);
		}
		else {
			other.data_ = nullptr;
		}
		swap(size_, other.size_);
		swap(capacity_, other.capacity_);
		swap(head_, other.head_);
		swap(tail_, other.tail_);
		swap(alloc_, other.alloc_);
		swap(del_, other.del_);
	}
	//
	Cyclic_buffer(Cyclic_buffer&& other) {
		swap(other);
	}
	//
	Cyclic_buffer& operator=(Cyclic_buffer const& other) {
		if (this != &other) {
			size_ = other.size_;
			capacity_ = other.capacity_;
			head_ = other.head_;
			tail_ = other.tail_;
			alloc_ = other.alloc_;
			del_ = other.del_;
			data_ = { alloc_.allocate(capacity_), del_ };
			pointer p = data_.get();
			for (size_t i = 0; i < size_; ++i, ++p) {
				std::allocator_traits<Allocator>::construct(alloc_, p, other[i]);
			}
		}
		return *this;
	}
	//
	Cyclic_buffer& operator=(Cyclic_buffer&& other) noexcept {
		swap(other);
		return *this;
	}
	//
	template<std::input_iterator InputIter>
	void assign(InputIter first, InputIter last) {
		size_type distance = std::abs(std::distance(first, last));
		reserve(distance);
		if (distance != size_) {
			size_ = distance;
			tail_ = (head_ + size_ - 1) % capacity_;
		}
		for (auto it = begin(); first != last; ++it, ++first) {
			*it = *first;
		}

	}
	//
	void assign(size_type count, const_reference value) {
		reserve(count);
		if (count != size_) {
			size_ = count;
			tail_ = (head_ + size_ - 1) % capacity_;
		}
		auto it = begin();
		for (size_t i = 0; i < count; ++it, ++i) {
			*it = value;
		}
	}
	//
	void assign(std::initializer_list<value_type> ilist) {
		*this = ilist;
	}
	//
	~Cyclic_buffer() {
		destroy_data(capacity_);
		if (data_ != nullptr) {
			data_.reset();
		}
	}
	//
	size_type size() const {
		return size_;
	}
	//
	size_type capacity() const {
		return capacity_;
	}
	//
	bool empty() const {
		return size_ == 0;
	}
	//
	bool full() const {
		return size_ == capacity_;
	}
	//
	size_type max_size() const {
		return std::allocator_traits<Allocator>::max_size();
	}
	//
	reference operator[](size_type pos) {
		if (size_ != 0) {
			return data_[(head_ + pos % size_) % capacity_];
		}
		else {
			return data_[head_];
		}
	}
	//
	const_reference operator[](size_type pos) const {
		if (size_ != 0) {
			return data_[(head_ + pos % size_) % capacity_];
		}
		else {
			return data_[head_];
		}
	}
	//
	reference at(size_type pos) {
		if (pos > size_) {
			throw std::out_of_range("Out of range\n");
		}
		return operator[](pos);
	}
	//
	const_reference at(size_type pos) const {
		if (pos > size_) {
			throw std::out_of_range("Out of range\n");
		}
		return operator[](pos);
	}
	//
	reference front() {
		return at(0);
	}
	//
	const_reference front() const {
		return at(0);
	}
	//
	reference back() {
		return at(size_ - 1);
	}
	//
	const_reference back() const {
		return at(size_ - 1);
	}
	//
	void push_back(const_reference value) {
		if (capacity_ != 0) {
			size_ += (size_ == capacity_) ? 0 : 1;
			tail_ = (tail_ + 1) % capacity_;
			size_type pos = (tail_ >= head_) ? tail_ - head_ : tail_ + capacity_ - head_;
			operator[](pos) = value;
			if (head_ == tail_) {
				head_ = (head_ + 1) % capacity_;
			}
		}
	}
	//
	void push_front(const_reference value) {
		if (capacity_ != 0) {
			size_ += (size_ == capacity_) ? 0 : 1;
			head_ = (head_ == 0) ? capacity_ - 1 : head_ - 1;
			operator[](0) = value;
			if (head_ == tail_) {
				tail_ = (tail_ == 0) ? capacity_ - 1 : tail_ - 1;
			}
		}
	}
	//
	void pop_back() {
		if (size_ != 0) {
			size_ -= 1;
			if (size_ > 0) {
				tail_ = (tail_ == 0) ? capacity_ - 1 : tail_ - 1;
			}
			else {
				tail_ = head_ = 0;
			}
		}
	}
	//
	void pop_front() {
		if (size_ != 0) {
			size_ -= 1;
			if (size_ > 0) {
				head_ = (head_ + 1) % capacity_;
			}
			else {
				head_ = tail_ = 0;
			}
		}
	}

	void insert(size_type pos, const_reference value) {
		//if (pos >= size_ && pos < capacity_) {
		//	pointer p = end();
		//	tail_ += pos - size_ + 1;
		//	for (size_t i = size_; i < pos; ++i, ++p) {
		//		std::allocator_traits<Allocator>::construct(alloc_, p, T());
		//	}
		//	std::allocator_traits<Allocator>::construct(alloc_, p, value);
		//	size_ = pos + 1;
		//	return;
		//}
		if (!full()) {
			size_ += 1;
		}
		else {
			head_ = (head_ + 1) % capacity_;
		}
		tail_ = (tail_ + 1) % capacity_;
		value_type prev = operator[](pos);
		operator[](pos) = value;
		value_type pred = operator[](pos + 1);
		for (size_t i = pos + 2; i < size_; ++i) {
			operator[](i) = pred;
			pred = operator[](i + 1);
		}
		operator[](pos + 1) = prev;
	}
	void insert(iterator pos, const_reference value) {
		insert(pos.index_, value);
	}
	void insert_extend(size_type pos, const_reference value) {
		if (pos >= capacity_) {
			reserve(pos + 1);
		}
		else if (full()) {
			reserve(capacity_ * 9 / 8 + 1);
		}
		insert(pos, value);
	}
	void insert_extend(iterator pos, const_reference value) {
		insert_extend(pos.index_, value);
	}

	template<std::input_iterator InputIter>
	void insert(size_type pos, InputIter first, InputIter last) {
		size_type distance = std::abs(std::distance(first, last));
		//if (pos + distance >= size_ && size_ + distance < capacity_) {
		//	pointer p = end();
		//	tail_ += pos + distance - size_;
		//	for (size_t i = size_; i < pos; ++i, ++p) {
		//		std::allocator_traits<Allocator>::construct(alloc_, p, T());
		//	}
		//	for ( ; first != last; ++first, ++p) {
		//		std::allocator_traits<Allocator>::construct(alloc_, p, *first);
		//	}
		//	size_ = pos + distance;
		//	return;
		//}
		//if (distance >= capacity_) {
		//	auto saver = first;
		//	std::advance(first, capacity_);
		//	assign(saver, first);
		//	return;
		//}

		std::rotate(begin(), begin() + (size_ - pos - distance), end());
		//for (auto it = begin() + pos; first != last;  ++it, ++first) {
		//	*it = *first;
		//}
	}

	template<std::input_iterator InputIter>
	void insert_extend(size_type pos, InputIter first, InputIter last) {
		size_t distance = std::distance(first, last);
		if (pos >= size_ && pos + distance >= capacity_) {
			reserve(pos + distance + 1);
		}
		else if (size_ + distance - 1 >= capacity_) {
			reserve(size_ + distance);
		}
		insert(pos, first, last);
	}
	template<std::input_iterator InputIter>
	void insert(iterator pos, InputIter first, InputIter last) {
		insert(pos.index_, first, last);
	}
	template<std::input_iterator InputIter>
	void insert_extend(iterator pos, InputIter first, InputIter last) {
		insert_extend(pos.index_, first, last);
	}
	void insert(size_type pos, size_type n, const_reference value) {
		if (pos + n >= size_ && pos + n < capacity_) {
			pointer p = end();
			tail_ += pos + n - size_;
			for (size_t i = size_; i < pos; ++i, ++p) {
				std::allocator_traits<Allocator>::construct(alloc_, p, T());
			}
			for (size_t i = 0; i < n; ++i, ++p) {
				std::allocator_traits<Allocator>::construct(alloc_, p, value);
			}
			size_ = pos + n;
			return;
		}
		if (n >= capacity_) {
			assign(capacity_, value);
			return;
		}
		std::rotate(begin(), begin() + (pos + n + 1) % size_, end());
		for (auto it = begin() + pos; it != begin() + (pos + n) % size_; ++it) {
			*it = value;
		}
	}
	void insert_extend(size_type pos, size_type n, const_reference value) {
		if (pos >= size_ && pos + n >= capacity_) {
			reserve(pos + n + 1);
		}
		if (size_ + n - 1 >= capacity_) {
			reserve(size_ + n);
		}
		insert(pos, n, value);
		
	}
	void insert(iterator pos, size_type n, const_reference value) {
		insert(pos.index_, n, value);
	}

	iterator erase(size_type pos) {
		for (auto it = begin() + pos; it != end(); ++it) {
			*it = *(it + 1);
		}
		size_ -= 1;
		tail_ = (tail_ == 0) ? size_ - 1 : tail_ - 1;
		return begin() + pos;
	}
	iterator erase(iterator pos) {
		erase(pos.index_);
	}
	iterator erase(size_type first, size_type last) {
		size_t del_volume = last - first;
		if (del_volume >= size_) {
			clear();
			return begin();
		}
		for (auto it = begin() + first; it != begin() + last; ++it) {
			*it = *(it + del_volume);
		}
		size_ = size_ - del_volume;
		tail_ = (tail_ < del_volume) ? size_ + tail_ - del_volume : tail_ - del_volume;
		return begin() + first;
	}
	iterator erase(iterator first, iterator last) {
		erase(first.index_, last.index_);
	}
	void erase_begin(size_type n) {
		if (n < size_) {
			head_ = (head_ + n) % size_;
			size_ -= n;
		}
		else {
			clear();
		}
	}
	void erase_end(size_type n) {
		if (n < size_) {
			tail_ = (n <= tail_) ? tail_ - n : size_ - tail_ - n - 1;
			size_ -= n;
		}
		else {
			clear();
		}
	}
	// STARTS HERE
	//
	enum class Iter_type {
		HEAD,
		TAIL,
		MID
	};
	using iter_type = Iter_type;
	//
	template<typename V>
	class Iterator {
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef V value_type;
		typedef long long difference_type;
		typedef V* pointer;
		typedef V& reference;
		//
		explicit Iterator(Cyclic_buffer *instance, difference_type index = 0,
			iter_type const& type = iter_type::MID)
			: instance_(instance), ptr_(instance_->data_.get()), index_(index), type_(type) {}
		//
		~Iterator() {}
		//
		Iterator(Iterator const& other) : instance_(other.instance_), ptr_(other.ptr_), index_(other.index_), type_(other.type_) {}
		//
		void swap(Iterator& other) {
			using std::swap;
			swap(instance_, other.instance_);
			swap(ptr_, other.ptr_);
			swap(index_, other.index_);
			swap(type_, other.type_);
		}
		//
		Iterator(Iterator&& other) noexcept {
			swap(other);
		}
		//
		Iterator& operator=(Iterator const& other) {
			if (this != &other) {
				instance_ = other.instance_;
				ptr_ = other.ptr_;
				index_ = other.index_;
				type_ = other.type_;
			}
			return *this;
		}
		//
		Iterator& operator=(Iterator&& other) noexcept {
			swap(other);
			return *this;
		}
		//
		reference operator*() const {
			return *(ptr_ + index_);
		}
		//
		pointer operator->() const {
			return &(ptr_[index_]);
		}
		//
		Iterator& operator++() {
			if (index_ == instance_->tail_ || type_ == iter_type::TAIL) {
				type_ = iter_type::TAIL;
				index_ += 1;
			}
			else {
				type_ = iter_type::MID;
				index_ = (index + 1) % instance_->capacity_;
			}
			return *this;
		}
		//
		Iterator operator++(int) {
			Iterator tmp(*this);
			++(*this);
			return tmp;
		}
		//
		Iterator& operator--() {
			size_type pos = (index_ == 0) ? instance_->capacity_ - 1 : index_ - 1;
			if (pos == instance_->head_ || type_ == iter_type::HEAD) {
				type_ = iter_type::HEAD;
				--index_;
			}
			else {
				type_ = iter_type::MID;
				index_ = pos;
			}
			return *this;
		}
		//
		Iterator operator--(int) {
			Iterator tmp(*this);
			--(*this);
			return tmp;
		}
		//
		Iterator& operator+=(difference_type i) {
			size_type pos = (i >= 0) ? index_ + i : (index_ >= i) ? index_ - i : instance_->size_ - 1 + index_ - i;
			if (i >= 0 && (i > (instance_->head_ + instance_->size_) - index_ || type_ == iter_type::TAIL)) {
				type_ = iter_type::TAIL;
			}
			else if (i < 0 && (std::abs(i) >= index_ - head_ || type_ == iter_type::HEAD)) {
				type_ = iter_type::HEAD;
			}
			else {
				type_ = iter_type::MID;
			}
			index_ = pos;
			return *this;
		}
		//
		Iterator& operator-=(difference_type i) {
			*this += -i;
			return *this;
		}
		//
		friend Iterator operator+(Iterator lhs, difference_type rhs) {
			lhs += rhs;
			return lhs;
		}
		//
		friend Iterator operator+(difference_type lhs, Iterator rhs) {
			return rhs + lhs;
		}
		//
		friend Iterator operator-(Iterator lhs, difference_type rhs) {
			lhs -= rhs;
			return lhs;
		}
		//
		reference operator[](difference_type i) const {
			return *(*this + i);
		}
		//
		difference_type operator-(Iterator const& rhs) const {
			return (index_ >= rhs.index_) ? index_ - rhs.index_ : index_ + (instance_->size_ - rhs.index_);
		}
		//
		bool operator==(Iterator const& other) const {
			if (instance_ == other.instance_) {
				return (type == iter_type::MID) ? index_ == other.index_ : type_ == other.type_;
			}
			return false;
		}
		//
		bool operator!=(Iterator const& other) const {
			return !(*this == other);
		}
		//
		bool operator<(Iterator const& other) const {
			return other - *this > 0;
		}
		//
		bool operator>(Iterator const& other) const {
			return other < *this;
		}
		//
		bool operator>=(Iterator const& other) const {
			return !(*this < other);
		}
		//
		bool operator<=(Iterator const& other) const {
			return !(*this > other);
		}
		//
		operator Iterator<const V>() const {
			return Iterator<const V>(instance_, index_, type_);
		}
		//
		operator pointer() const {
			return operator->();
		}
		friend class Reverse_iterator<Iterator>;
		friend class Cyclic_buffer;
	private:
		Cyclic_buffer* instance_;
		pointer ptr_;
		difference_type index_;
		iter_type type_;
	};

	template<typename Iter>
	class Reverse_iterator {
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef typename Iter::value_type value_type;
		typedef Iter::difference_type difference_type;
		typedef typename Iter::pointer pointer;
		typedef typename Iter::reference reference;

		explicit Reverse_iterator(Iter iter) : iter_(iter.instance_, iter.index_ - 1, iter.type_) {}
		Reverse_iterator(Cyclic_buffer* instance, difference_type index = 0,
			iter_type const& type = iter_type::MID)
			: iter_(instance, index - 1, type) {}
		void swap(Reverse_iterator& other) {
			using std::swap;
			swap(iter_, other.iter_);
		}
		~Reverse_iterator() {}
		Reverse_iterator(Reverse_iterator const& other) : iter_(other.iter_) {}
		Reverse_iterator& operator=(Reverse_iterator const& other) {
			iter_ = other.iter_;
			return *this;
		}
		Reverse_iterator& operator=(Reverse_iterator&& other) noexcept {
			swap(other);
			return *this;
		}
		reference operator*() const {
			return *iter_;
		}
		pointer operator->() const {
			return iter_.operator->();
		}
		Reverse_iterator& operator++() {
			Iter tmp(iter_);
			--tmp;
			if (iter_.index_ == iter_.instance_->head_) {
				--iter_.index_;
				iter_.type_ = iter_type::HEAD;
				return *this;
			}
			else {
				iter_.type_ = iter_type::MID;
			}
			iter_.index_ = tmp.index_;
			return *this;
		}
		Reverse_iterator operator++(int) {
			Reverse_iterator tmp(*this);
			++(*this);
			return tmp;
		}
		Reverse_iterator& operator--() {
			++iter_;
			if (iter_.index_ == iter_.instance_->tail_) {
				iter_.type_ = iter_type::TAIL;
			}
			else {
				iter_.type_ = iter_type::MID;
			}
			return *this;
		}
		Reverse_iterator operator--(int) {
			Reverse_iterator tmp(*this);
			--(*this);
			return tmp;
		}
		Reverse_iterator& operator+=(difference_type i) {
			iter_ -= i;
			return *this;
		}
		Reverse_iterator& operator-=(difference_type i) {
			iter_ += i;
			return *this;
		}
		friend Reverse_iterator operator+(Reverse_iterator lhs, difference_type rhs) {
			lhs += rhs;
			return lhs;
		}
		friend Reverse_iterator operator+(difference_type lhs, Reverse_iterator rhs) {
			return rhs + lhs;
		}
		friend Reverse_iterator operator-(Reverse_iterator lhs, difference_type rhs) {
			lhs -= rhs;
			return lhs;
		}
		reference operator[](difference_type i) const {
			return *(*this + i);
		}
		difference_type operator-(Reverse_iterator const& rhs) const {
			return rhs.iter_ - iter_;
		}
		bool operator==(Reverse_iterator const& other) const {
			return iter_ == other.iter_;
		}
		bool operator!=(Reverse_iterator const& other) const {
			return !(*this == other);
		}
		bool operator<(Reverse_iterator const& other) const {
			return other - *this > 0;
		}
		bool operator>(Reverse_iterator const& other) const {
			return other < *this;
		}
		bool operator>=(Reverse_iterator const& other) const {
			return !(*this < other);
		}
		bool operator<=(Reverse_iterator const& other) const {
			return !(*this > other);
		}
		Iter &base() const {
			return iter_;
		}
		operator Reverse_iterator<const Iter>() const {
			return Reverse_iterator<const Iter>(iter_);
		}
		operator pointer() const {
			return operator->();
		}
		friend class Cyclic_buffer;
	private:
		Iter iter_;
	};
	//
	iterator begin() {
		return iterator(this, head_, iter_type::HEAD);
	}
	//
	iterator end() {
		return iterator(this, tail_ + 1, iter_type::TAIL);
	}
	//
	const_iterator cbegin() {
		return const_iterator(this, head_, iter_type::HEAD);
	}
	//
	const_iterator cend() {
		return const_iterator(this, tail_ + 1, iter_type::TAIL);
	}
	//
	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}
	//
	reverse_iterator rend() {
		return reverse_iterator(begin());
	}
	//
	const_reverse_iterator crbegin() {
		return const_reverse_iterator(cend());
	}
	//
	const_reverse_iterator crend() {
		return const_reverse_iterator(cbegin());
	}
	//
	allocator_type get_allocator() const {
		return alloc_;
	}
	//
	allocator_type& get_allocator() {
		return alloc_;
	}
	//
	deleter_type get_deleter() const {
		return del_;
	}
	//
	deleter_type& get_deleter() {
		return del_;
	}

	//
	void clear() {
		size_ = head_ = tail_ = 0;
	}

	//
	void reserve(size_type new_capacity) {
		if (new_capacity <= capacity_) {
			return;
		}
		pointer new_data = alloc_.allocate(new_capacity);
		pointer p = new_data;
		for (auto it = cbegin(); it != cend(); ++it, ++p) {
			std::allocator_traits<Allocator>::construct(alloc_, p, *it);
		}
		destroy_data(capacity_);
		data_.reset(new_data);
		capacity_ = new_capacity;
		head_ = 0;
		tail_ = size_ - 1;
	}

	//
	void resize(size_type new_size, const_reference value = T()) {
		pointer new_data = alloc_.allocate(new_size);
		pointer p = new_data;
		for (size_t i = 0; i < new_size; ++i, ++p) {
			if (i < size_) {
				std::allocator_traits<Allocator>::construct(alloc_, p, operator[](i));
			}
			else {
				std::allocator_traits<Allocator>::construct(alloc_, p, value);
			}
		}
		destroy_data(capacity_);
		data_.reset(new_data);
		capacity_ = size_ = new_size;
		head_ = 0;
		tail_ = size_ - 1;
	}

	//
	void shrink_to_fit() {
		resize(size_);
	}

	//
	friend bool operator==(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		if (lhs.size() == rhs.size()) {
			auto l = lhs.begin();
			auto r = rhs.begin();
			for ( ; l != lhs.end() && r != rhs.end(); ++l, ++r) {
				if (*l != *r) {
					return false;
				}
			}
			return true;
		}
		return false;
	}
	//
	friend bool operator!=(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return !(lhs == rhs);
	}
	//
	friend bool operator<(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
	//
	friend bool operator>(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return rhs < lhs;
	}
	//
	friend bool operator<=(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return !(lhs > rhs);
	}
	//
	friend bool operator>=(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return !(lhs < rhs);
	}
private:
	size_type capacity_;
	size_type size_;
	size_type head_;
	size_type tail_;
	allocator_type alloc_;
	deleter_type del_;
	std::mutex mutex_;
	std::unique_ptr<value_type[], Deleter> data_;
	//
	void destroy_data(size_t n) {
		pointer p = data_.get();
		for (size_t i = 0; i < n; ++i) {
			std::allocator_traits<Allocator>::destroy(alloc_, p);
		}
	}
	//static const size_t DEFAULT_CAPACITY = 256 * sizeof(value_type);
};




#endif
