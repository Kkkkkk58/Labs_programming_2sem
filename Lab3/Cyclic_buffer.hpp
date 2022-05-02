#ifndef CYCLIC_BUFFER_HPP
#define CYCLIC_BUFFER_HPP
#include <shared_mutex>
#include <iostream>
#include <type_traits>
#include <memory>
#include <functional>
#include <iterator>
#include <concepts>

// Шаблонный класс-функтор, призванный деллоцировать память, выделенную аллокатором
template<typename T, typename Allocator>
class Default_delete {
public:
	void operator()(T* p, Allocator a = Allocator(), size_t n = 0) {
		a.deallocate(p, n);
	}
};


// Шаблонный класс циклического буфера, аргументы шаблона - его тип, настраиваемый аллокатор, 
// настраиваемый очиститель данных
template<typename T, typename Allocator = std::allocator<T>, typename Deleter = Default_delete<T, Allocator>>
class Cyclic_buffer {
public:
	// Информация об используемых псевдонимах названий типов
	template<typename V>
	class Iterator;
	template<typename Iter>
	class Reverse_iterator;

	using value_type			 = T;
	using size_type				 = size_t;
	using difference_type		 = ptrdiff_t;
	using reference				 = T&;
	using const_reference        = T const&;
	using pointer                = typename std::allocator_traits<Allocator>::pointer;
	using const_pointer          = typename std::allocator_traits<Allocator>::const_pointer;
	using allocator_type         = Allocator;
	using deleter_type           = Deleter;
	using iterator               = Iterator<value_type>;
	using const_iterator         = Iterator<const value_type>;
	using reverse_iterator       = Reverse_iterator<iterator>;
	using const_reverse_iterator = Reverse_iterator<const_iterator>;

	// Конструктор класса от двух входных итераторов, аллокатора по умолчанию, очистителя по умолчанию
	template<std::input_iterator InputIter>
	Cyclic_buffer(InputIter first, InputIter last, allocator_type const& alloc = Allocator(), \
		deleter_type const& del = Deleter())
	try	: capacity_(std::abs(std::distance(first, last))), 
		size_(capacity_), head_(0), tail_(size_ - 1), alloc_(alloc),
		del_(del), mutex_(), data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		while (first != last) {
			std::allocator_traits<Allocator>::construct(alloc_, p, *first);
			++first;
			++p;
		}
	}
	catch (std::exception& e) {
		std::cerr << "Error while creating the object! " << e.what();
		throw;
	}

	// Конструктор класса от аллокатора по умолчанию, очистителя по умолчанию
	explicit Cyclic_buffer(allocator_type const& alloc = Allocator(), \
		deleter_type const& del = Deleter())
		: capacity_(0), size_(0), head_(0), tail_(0), alloc_(alloc),
		del_(del), mutex_(), data_(nullptr, del) {}

	// Конструктор класса от вместимости, аллокатора по умолчанию, очистителя по умолчанию
	explicit Cyclic_buffer(size_type capacity, allocator_type const& alloc = Allocator(), \
		deleter_type const& del = Deleter())
	try : capacity_(capacity), size_(0), head_(0), tail_(0), alloc_(alloc), del_(del), mutex_(), 
		data_(alloc_.allocate(capacity_), del_) {}
	catch (std::exception &e) {
			std::cerr << "Error while creating the object! " << e.what();
			throw;
	}

	// Конструктор класса от размера, значений, инициализирующих выделенное пространство,
	// аллокатора по умолчанию, очистителя по умолчанию
	Cyclic_buffer(size_type size, const_reference value, allocator_type const& alloc = Allocator(),\
		deleter_type const& del = Deleter())
	try : capacity_(size), size_(capacity_), head_(0), tail_(size_ - 1),
		alloc_(alloc), del_(del), mutex_(),
		data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		for (size_t i = 0; i < size_; ++i, ++p) {
			std::allocator_traits<Allocator>::construct(alloc_, p, value);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Error while creating the object! " << e.what();
		throw;
	}

	// Конструктор от требуемой вместимости, размера, значения, инициализирующего выделенное место,
	// аллокатора по умолчанию, очистителя по умолчанию
	Cyclic_buffer(size_type capacity, size_type size, const_reference value, \
		allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter()) 
	try	: capacity_(capacity), size_((size > capacity) ? capacity_ : size), 
		head_(0), tail_(size_ - 1), alloc_(alloc), del_(del), 
		mutex_(), data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		for (size_t i = 0; i < size_; ++i, ++p) {
			std::allocator_traits<Allocator>::construct(alloc_, p, value);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Error while creating the object! " << e.what();
		throw;
	}

	// Конструктор класса от initializer_list (не explicit), аллокатора по умолчанию, 
	// очистителя по умолчанию
	Cyclic_buffer(std::initializer_list<value_type> const& list, \
		allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter())
	try	: capacity_(list.size()), size_(capacity_), head_(0), tail_(size_ - 1), alloc_(alloc), 
		del_(del), mutex_(), data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		for (auto i = list.begin(); i != list.end(); ++i, ++p) {
			std::allocator_traits<Allocator>::construct(alloc_, p, *i);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Error while creating the object! " << e.what();
		throw;
	}

	// Конструктор класса от вместимости, initializer_list,
	// аллокатора по умолчанию, очистителя по умолчанию
	Cyclic_buffer(size_type capacity, std::initializer_list<value_type> const& list, \
		allocator_type const& alloc = Allocator(), deleter_type const& del = Deleter())
	try	: capacity_(capacity), size_((list.size() > capacity_) ? capacity_ : list.size()),
		head_(0), tail_(size_ - 1), alloc_(alloc), del_(del), mutex_(),
		data_(alloc_.allocate(capacity_), del_) {
		pointer p = data_.get();
		auto it = list.begin();
		for (size_t i = 0; i < size_; ++i, ++p, ++it) {
			std::allocator_traits<Allocator>::construct(alloc_, p, *it);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Error while creating the object! " << e.what();
		throw;
	}

	// Конструктор копирования
	Cyclic_buffer(Cyclic_buffer const& other)
		: size_(other.size_), capacity_(other.capacity_), head_(other.head_), tail_(other.tail_),
		alloc_(other.alloc_), del_(other.del_) {
		try {
			write_lock lock(mutex_);
			pointer data = alloc_.allocate(capacity_);
			pointer p = data;
			for (size_t i = 0; i < size_; ++i, ++p) {
				std::allocator_traits<Allocator>::construct(alloc_, p, other[i]);
			}
			data_ = { data, del_ };
		}
		catch (std::exception& e) {
			std::cerr << "Error while copying the object! " << e.what();
			throw;
		}
	}

	// Метод swap
	void swap(Cyclic_buffer& other) {
		using std::swap;
		write_lock lock(mutex_);
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

	// Конструктор перемещения
	Cyclic_buffer(Cyclic_buffer&& other) {
		swap(other);
	}

	// Оператор присваивания
	Cyclic_buffer& operator=(Cyclic_buffer const& other) {
		if (this != &other) {
			try {
				write_lock lock(mutex_);
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
			catch (std::exception& e) {
				std::cerr << "Error while assigning the object! " << e.what();
			}
		}
		return *this;
	}
	
	// Перемещающий оператор присваивания
	Cyclic_buffer& operator=(Cyclic_buffer&& other) noexcept {
		swap(other);
		return *this;
	}
	
	// Метод, присваивающий буферу значение, находящееся между двумя входными итераторами
	template<std::input_iterator InputIter>
	void assign(InputIter first, InputIter last) {
		try {
			write_lock lock(mutex_);
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
		catch (std::exception& e) {
			std::cerr << "Error while assigning the object! " << e.what();
		}

	}

	// Присвоение буферу значения массива, заполненного выбранным значением
	void assign(size_type count, const_reference value) {
		try {
			write_lock lock(mutex_);
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
		catch (std::exception& e) {
			std::cerr << "Error while assigning the object! " << e.what();
		}
	}

	// Присваивание буферу значения initializer_list
	void assign(std::initializer_list<value_type> const& ilist) {
		*this = ilist;
	}

	// Деструктор
	~Cyclic_buffer() {
		destroy_data(capacity_);
		if (data_ != nullptr) {
			write_lock lock(mutex_);
			data_.reset();
		}
	}

	// Метод, возвращающий размер, используемый буфером
	size_type size() const {
		return size_;
	}

	// Метод, возвращающий вместимость буфера
	size_type capacity() const {
		return capacity_;
	}

	// Метод, проверяющий буфер на пустоту
	bool empty() const {
		return size_ == 0;
	}

	// Метод, проверяющий буфер на заполненность
	bool full() const {
		return size_ == capacity_;
	}

	// Метод, возвращающий максимально возможный размер буфера
	size_type max_size() const {
		return std::allocator_traits<Allocator>::max_size();
	}

	// Константный метод получения аллокатора
	allocator_type get_allocator() const {
		return alloc_;
	}

	// Метод получения ссылки на аллокатор
	allocator_type& get_allocator() {
		return alloc_;
	}

	// Константный метод получения очистителя
	deleter_type get_deleter() const {
		return del_;
	}

	// Метод получения ссылки на очиститель
	deleter_type& get_deleter() {
		return del_;
	}

	// Метод для проверки, содержит ли буфер элемент
	bool contains(const_reference value) {
		read_lock lock(mutex_);
		for (auto const& val : *this) {
			if (value == val) {
				return true;
			}
		}
		return false;
	}

	// Оператор произвольного доступа к элементам по индексу
	reference operator[](size_type pos) {
		if (size_ != 0) {
			return data_[(head_ + pos % size_) % capacity_];
		}
		else {
			return data_[head_];
		}
	}
	
	// Константный оператор произвольного доступа к элементам по индексу
	const_reference operator[](size_type pos) const {
		if (size_ != 0) {
			return data_[(head_ + pos % size_) % capacity_];
		}
		else {
			return data_[head_];
		}
	}
	
	// Метод для доступа к элементу по индексу, вызывающий исключение в случае несуществующего индекса
	reference at(size_type pos) {
		if (pos > size_) {
			throw std::out_of_range("Out of range\n");
		}
		return operator[](pos);
	}
	
	// Константный метод, аналогичный предыдущему
	const_reference at(size_type pos) const {
		if (pos > size_) {
			throw std::out_of_range("Out of range\n");
		}
		return operator[](pos);
	}
	
	// Метод, позволяющий заменить элемент по итератору на конкретное значение
	void replace(iterator it, const_reference value) {
		write_lock lock(mutex_);
		*it = value;
	}
	
	// Метод, позволяющий заменить элемент по индексу на конкретное значение
	void replace(size_type pos, const_reference value) {
		write_lock lock(mutex_);
		operator[](pos) = value;
	}
	
	// Метод, позволяющий заменить элементы между двумя итераторами на конкретное значение
	void replace(iterator start, iterator end, const_reference value) {
		write_lock lock(mutex_);
		while (start != end) {
			*start = value;
			++start;
		}
	}
	
	// Метод, позволяющий заменить элемент между двумя индексами на конкретное значение
	void replace(size_type start, size_type end, const_reference value) {
		write_lock lock(mutex_);
		while (start != end) {
			operator[](start) = value;
			++start;
		}
	}
	
	// Метод, позволяющий заменить элемент между двумя итераторами на значения,
	// заданные входными итераторами
	template<std::input_iterator InputIt>
	void replace(iterator start, iterator end, InputIt first, InputIt last) {
		write_lock lock(mutex_);
		while (start != end && first != last) {
			*start = *first;
			++start;
			++first;
		}
	}

	// Метод, позволяющий заменить элемент между двумя индексами на значения,
	// заданные входными итераторами
	template<std::input_iterator InputIt>
	void replace(size_type start, size_type end, InputIt first, InputIt last) {
		write_lock lock(mutex_);
		while (start != end && first != last) {
			operator[](start) = *first;
			++start;
			++first;
		}
	}
	
	// Метод получения доступа к первому элементу
	reference front() {
		return at(0);
	}
	
	// Константный метод получения доступа к первому элементу
	const_reference front() const {
		return at(0);
	}
	
	// Метод получения доступа к последнему элементу
	reference back() {
		return at(size_ - 1);
	}
	
	// Константный метод получения доступа к последнему элементу
	const_reference back() const {
		return at(size_ - 1);
	}
	
	// Вставка в конец буфера
	void push_back(const_reference value) {
		if (capacity_ != 0) {
			write_lock lock(mutex_);
			tail_ = (tail_ + 1) % capacity_;
			if (size_ == capacity_) {
				operator[](size_) = value;
			}
			else {
				operator[](size_ - 1) = value;
				size_ += 1;
			}
			if (head_ == tail_) {
				head_ = (head_ + 1) % capacity_;
			}
		}
	}
	
	// Вставка в начало буфера
	void push_front(const_reference value) {
		if (capacity_ != 0) {
			write_lock lock(mutex_);
			size_ += (size_ == capacity_) ? 0 : 1;
			head_ = (head_ == 0) ? capacity_ - 1 : head_ - 1;
			operator[](0) = value;
			if (head_ == tail_) {
				tail_ = (tail_ == 0) ? capacity_ - 1 : tail_ - 1;
			}
		}
	}
	
	// Удаление из конца буфера
	void pop_back() {
		if (size_ != 0) {
			write_lock lock(mutex_);
			size_ -= 1;
			if (size_ > 0) {
				tail_ = (tail_ == 0) ? capacity_ - 1 : tail_ - 1;
			}
			else {
				tail_ = head_ = 0;
			}
		}
	}
	
	// Удаление из начала буфера
	void pop_front() {
		if (size_ != 0) {
			write_lock lock(mutex_);
			size_ -= 1;
			if (size_ > 0) {
				head_ = (head_ + 1) % capacity_;
			}
			else {
				head_ = tail_ = 0;
			}
		}
	}
	
	// Вставка с определенной позиции значений, полученных от входных итераторов
	template<std::input_iterator InputIter>
	void insert(size_type pos, InputIter first, InputIter last) {
		size_type n = std::abs(std::distance(first, last));
		write_lock lock(mutex_);
		if (pos >= size_ && pos + n < capacity_) {
			pointer p = end();
			tail_ = (tail_ + pos + n - size_) % capacity_;
			for (size_t i = size_; i < pos; ++i, ++p) {
				std::allocator_traits<Allocator>::construct(alloc_, p, T());
			}
			for (; first != last; ++first, ++p) {
				std::allocator_traits<Allocator>::construct(alloc_, p, *first);
			}
			size_ = pos + n;
			return;
		}
		if (n >= capacity_) {
			lock.unlock();
			assign(first, last);
			return;
		}
		if (full()) {
			std::rotate(begin() + pos, end() - n, end());
		}
		else {
			if (size_ + n > capacity_) {
				lock.unlock();
				resize(capacity_);
				lock.lock();
			}
			else {
				lock.unlock();
				resize(size_ + n);
				lock.lock();
			}
			std::rotate(begin() + pos, end() - n, end());
		}
		for (auto it = begin() + pos; first != last; ++it, ++first) {
			*it = *first;
		}
	}
	
	// Вставка с расширением с определенной позиции значений, полученных от входных итераторов
	template<std::input_iterator InputIter>
	void insert_extend(size_type pos, InputIter first, InputIter last) {
		try {
			size_t distance = std::distance(first, last);
			if (pos >= size_ && pos + distance >= capacity_) {
				reserve(pos + distance + 1);
			}
			else if (size_ + distance - 1 >= capacity_) {
				reserve(size_ + distance);
			}
			insert(pos, first, last);
		}
		catch (std::exception& e) {
			std::cerr << "Error while inserting into the object! " << e.what();
		}
	}
	
	// Вставка с определенной позиции значений, полученных от входных итераторов
	template<std::input_iterator InputIter>
	void insert(iterator pos, InputIter first, InputIter last) {
		insert(pos - begin(), first, last);
	}
	
	// Вставка с расширением с определенной позиции значений, полученных от входных итераторов
	template<std::input_iterator InputIter>
	void insert_extend(iterator pos, InputIter first, InputIter last) {
		insert_extend(pos - begin(), first, last);
	}
	
	// Вставка с определенной позиции n значений, равных value
	void insert(size_type pos, size_type n, const_reference value) {
		write_lock lock(mutex_);
		if (pos >= size_ && pos + n < capacity_) {
			pointer p = end();
			tail_ = (tail_ + pos + n - size_) % capacity_;
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
			lock.unlock();
			assign(capacity_, value);
			return;
		}
		if (full()) {
			std::rotate(begin() + pos, end() - n, end());
		}
		else {
			if (size_ + n > capacity_) {
				lock.unlock();
				resize(capacity_);
				lock.lock();
			}
			else {
				lock.unlock();
				resize(size_ + n);
				lock.lock();
			}
			std::rotate(begin() + pos, end() - n, end());
		}
		for (auto it = begin() + pos; it != begin() + (pos + n) % size_; ++it) {
			*it = value;
		}
	}
	
	// Вставка с расширением с определенной позиции n значений, равных value
	void insert_extend(size_type pos, size_type n, const_reference value) {
		try {
			if (size_ + n >= capacity_) {
				reserve(size_ + n + 1);
			}
			else if (pos + n + 1 >= capacity_) {
				reserve(pos + n + 2);
			}
			insert(pos, n, value);
		}
		catch (std::exception& e) {
			std::cerr << "Error while inserting into the object! " << e.what();
		}
		
	}
	
	// Вставка с определенной позиции n значений, равных value
	void insert(iterator pos, size_type n, const_reference value) {
		insert(pos - begin(), n, value);
	}
	
	// Вставка с расширением с определенной позиции n значений, равных value
	void insert_extend(iterator pos, size_type n, const_reference value) {
		insert_extend(pos - begin(), n, value);
	}
	
	// Вставка с определенной позиции значения, равного value
	void insert(size_type pos, const_reference value) {
		insert(pos, 1, value);
	}
	
	// Вставка с определенной позиции значения, равного value
	void insert(iterator pos, const_reference value) {
		insert(pos - begin(), value);
	}
	
	// Вставка с расширением с определенной позиции значения, равного value
	void insert_extend(size_type pos, const_reference value) {
		try {
			if (pos >= capacity_) {
				reserve(pos + 1);
			}
			else if (full()) {
				reserve(capacity_ * 9 / 8 + 1);
			}
			insert(pos, value);
		}
		catch (std::exception& e) {
			std::cerr << "Error while inserting into the object! " << e.what();
		}
	}
	
	// Вставка с расширением с определенной позиции значения, равного value
	void insert_extend(iterator pos, const_reference value) {
		insert_extend(pos - begin(), value);
	}
	
	// Удаление элемента по индексу
	iterator erase(size_type pos) {
		write_lock lock(mutex_);
		for (auto it = begin() + pos; it != end(); ++it) {
			*it = *(it + 1);
		}
		size_ -= 1;
		tail_ = (tail_ == 0) ? capacity_ - 1 : tail_ - 1;
		return begin() + pos;
	}
	
	// Удаление элемента по итератору
	iterator erase(iterator pos) {
		return erase(pos - begin());
	}
	
	// Удаление нескольких элементов по итераторам
	iterator erase(iterator first, iterator last) {
		size_t initial_pos = first.index_;
		size_t del_volume = last - first;
		if (del_volume >= size_) {
			clear();
			return begin();
		}
		write_lock lock(mutex_);
		while (first != end()) {
			*first = *(first + del_volume);
			++first;
		}
		size_ = size_ - del_volume;
		tail_ = (tail_ < del_volume) ? capacity_ + tail_ - del_volume : tail_ - del_volume;
		return begin() + initial_pos;
	}
	
	// Удаление нескольких элементов по индексам
	iterator erase(size_type first, size_type last) {
		return erase(iterator(this, first), iterator(this, last));
	}
	
	// Удаление элементов из начала
	void erase_begin(size_type n) {
		if (n < size_) {
			write_lock lock(mutex_);
			head_ = (head_ + n % size_) % capacity_;
			size_ -= n;
		}
		else {
			clear();
		}
	}
	
	// Удаление элементов из конца
	void erase_end(size_type n) {
		if (n < size_) {
			write_lock lock(mutex_);
			tail_ = (n <= tail_) ? tail_ - n : capacity_ + tail_ - n;
			size_ -= n;
		}
		else {
			clear();
		}
	}
	
	// Метод, очищающий буфер
	void clear() {
		write_lock lock(mutex_);
		size_ = head_ = tail_ = 0;
	}

	// Метод, позволяющий зарезервировать определённую вместимость для буфера
	void reserve(size_type new_capacity) {
		if (new_capacity <= capacity_) {
			return;
		}
		try {
			write_lock lock(mutex_);
			pointer new_data = alloc_.allocate(new_capacity);
			pointer p = new_data;
			for (auto it = cbegin(); it != cend(); ++it, ++p) {
				std::allocator_traits<Allocator>::construct(alloc_, p, *it);
			}
			lock.unlock();
			destroy_data(size_);
			lock.lock();
			data_.reset(new_data);
			capacity_ = new_capacity;
			head_ = 0;
			tail_ = size_ - 1;
		}
		catch (std::exception& e) {
			std::cerr << "Error while reserving capacity for the object! " << e.what();
			throw;
		}
	}

	// Метод, позволяющий манипулировать доступным размером буфера, при желании заполняя
	// место конкретными значениями
	void resize(size_type new_size, const_reference value = T()) {
		try {
			write_lock lock(mutex_);
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
			lock.unlock();
			destroy_data(size_);
			lock.lock();
			data_.reset(new_data);
			capacity_ = size_ = new_size;
			head_ = 0;
			tail_ = size_ - 1;
		}
		catch (std::exception& e) {
			std::cerr << "Error while resizing the object! " << e.what();
			throw;
		}
	}

	// Метод, ограничивающий вместимость буфера до текущего размера
	void shrink_to_fit() {
		resize(size_);
	}

	// Оператор сравнения ==
	friend bool operator==(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		if (lhs.size() == rhs.size()) {
			auto l = lhs.begin();
			auto r = rhs.begin();
			for (; l != lhs.end() && r != rhs.end(); ++l, ++r) {
				if (*l != *r) {
					return false;
				}
			}
			return true;
		}
		return false;
	}

	// Оператор сравнения !=
	friend bool operator!=(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return !(lhs == rhs);
	}

	// Оператор сравнения <
	friend bool operator<(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	// Оператор сравнения >
	friend bool operator>(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return rhs < lhs;
	}

	// Оператор сравнения <=
	friend bool operator<=(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return !(lhs > rhs);
	}

	// Оператор сравнения >=
	friend bool operator>=(Cyclic_buffer& lhs, Cyclic_buffer& rhs) {
		return !(lhs < rhs);
	}

	// Перечисление с типами ячейки, на которую указывает итератор
	enum class Iter_type {
		HEAD,
		TAIL,
		MID
	};
	using iter_type = Iter_type;
	
	// Вложенный класс итератора
	template<typename V>
	class Iterator {
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef V value_type;
		typedef long long difference_type;
		typedef V* pointer;
		typedef V& reference;
		
		// Конструктор класса
		explicit Iterator(Cyclic_buffer *instance, difference_type index = 0,
			iter_type const& type = iter_type::MID)
			: instance_(instance), ptr_(instance_->data_.get()), index_(index), type_(type) {}
		
		// Деструктор
		~Iterator() {}
		
		// Конструктор копирования
		Iterator(Iterator const& other)
			: instance_(other.instance_), ptr_(other.ptr_), \
			index_(other.index_), type_(other.type_) {}
		
		// Метод swap
		void swap(Iterator& other) {
			using std::swap;
			swap(instance_, other.instance_);
			swap(ptr_, other.ptr_);
			swap(index_, other.index_);
			swap(type_, other.type_);
		}
		
		// Перемещающий конструктор
		Iterator(Iterator&& other) {
			swap(other);
		}

		// Оператор присваивания
		Iterator& operator=(Iterator const& other) {
			if (this != &other) {
				instance_ = other.instance_;
				ptr_ = other.ptr_;
				index_ = other.index_;
				type_ = other.type_;
			}
			return *this;
		}
		
		// Перемещающий оператор присваивания
		Iterator& operator=(Iterator&& other) noexcept {
			swap(other);
			return *this;
		}
		
		// Оператор разыменования
		reference operator*() const {
			return *(ptr_ + index_);
		}
		
		// Доступ по указателю
		pointer operator->() const {
			return &(ptr_[index_]);
		}
		
		// Префиксный инкремент
		Iterator& operator++() {
			if (index_ == instance_->tail_) {
				index_ = instance_->head_;
				type_ = iter_type::TAIL;
			}
			else {
				index_ = (index_ + 1) % instance_->capacity_;
			}
			if (type_ != iter_type::TAIL) {
				type_ = iter_type::MID;
			}
			return *this;
		}
		
		// Постфиксный инкремент
		Iterator operator++(int) {
			Iterator tmp(*this);
			++(*this);
			return tmp;
		}
		
		// Префиксный декремент
		Iterator& operator--() {
			if (index_ == instance_->head_ && type_ != iter_type::TAIL) {
				index_ = instance_->tail_;
				type_ = iter_type::HEAD;
			}
			else {
				index_ = (index_ == 0) ? instance_->capacity_ - 1 : index_ - 1;
			}
			if (type_ != iter_type::HEAD) {
				type_ = iter_type::MID;
			}
			return *this;
		}
		
		// Постфиксный декремент
		Iterator operator--(int) {
			Iterator tmp(*this);
			--(*this);
			return tmp;
		}
		
		// Оператор +=
		Iterator& operator+=(difference_type i) {
			if (i != 0) {
				size_type rel_index = (static_cast<size_type>(index_) >= instance_->head_) ? \
					static_cast<size_type>(index_) - instance_->head_ : \
					instance_->capacity_ - instance_->head_ + static_cast<size_type>(index_);
				size_type dist = std::abs(i) % instance_->size_;
				size_type pos = 0;
				if (i >= 0) {
					pos = (instance_->head_ + (rel_index + i) % instance_->size_)\
						% instance_->capacity_;
				}
				else {
					if (instance_->head_ < instance_->tail_) {
						pos = (rel_index >= dist) ? rel_index - dist : \
							instance_->tail_ + 1 + rel_index - dist;
					}
					else {
						pos = (static_cast<size_type>(index_) >= dist) ? index_ - dist : \
							instance_->capacity_ + index_ - dist;
					}
				}
				if (i >= 0 && (static_cast<size_type>(i) + rel_index >= instance_->size_
					|| type_ == iter_type::TAIL)) {
					type_ = iter_type::TAIL;
				}
				else if (i < 0 && (static_cast<size_type>(std::abs(i)) >= rel_index 
					|| type_ == iter_type::HEAD)) {
					type_ = iter_type::HEAD;
				}
				else {
					type_ = iter_type::MID;
				}
				index_ = pos;
			}
			return *this;
		}
		
		// Оператор -=
		Iterator& operator-=(difference_type i) {
			*this += -i;
			return *this;
		}
		
		// Оператор сложения с числом
		friend Iterator operator+(Iterator lhs, difference_type rhs) {
			lhs += rhs;
			return lhs;
		}
		
		// Оператор сложения с числом
		friend Iterator operator+(difference_type lhs, Iterator rhs) {
			return rhs + lhs;
		}
		
		// Оператор разности с числом
		friend Iterator operator-(Iterator lhs, difference_type rhs) {
			lhs -= rhs;
			return lhs;
		}
		
		// Оператор []
		reference operator[](difference_type i) const {
			return *(*this + i);
		}
		
		// Оператор разности с итератором
		difference_type operator-(Iterator const& rhs) const {
			if (index_ == rhs.index_) {
				if (type_ == rhs.type_) {
					return 0;
				}
				return instance_->size_;
			}
			return relative_index() - rhs.relative_index();
		}

		// Получение индекса относительно начала буфера
		size_type relative_index() const {
			return (static_cast<size_type>(index_) >= instance_->head_) ? \
				static_cast<size_type>(index_) - instance_->head_ : \
				instance_->capacity_ - instance_->head_ + static_cast<size_type>(index_);
		}
		
		// Проверка на равенство
		bool operator==(Iterator const& other) const {
			if (instance_ == other.instance_) {
				return (type_ == iter_type::MID) ? index_ == other.index_ : type_ == other.type_;
			}
			return false;
		}
		
		// Проверка на неравенство
		bool operator!=(Iterator const& other) const {
			return !(*this == other);
		}
		
		// Оператор сравнения <
		bool operator<(Iterator const& other) const {
			return other - *this > 0;
		}
		
		// Оператор сравнения >
		bool operator>(Iterator const& other) const {
			return other < *this;
		}
		
		// Оператор сравнения >=
		bool operator>=(Iterator const& other) const {
			return !(*this < other);
		}

		// Оператор сравнения <=
		bool operator<=(Iterator const& other) const {
			return !(*this > other);
		}

		// Оператор приведения к константному итератору
		operator Iterator<const V>() const {
			return Iterator<const V>(instance_, index_, type_);
		}
		
		// Оператор приведения к указателю
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

	// Вложенный класс обратного итератора
	template<typename Iter>
	class Reverse_iterator {
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type        = typename Iter::value_type;
		using difference_type   = typename Iter::difference_type;
		using pointer           = typename Iter::pointer;
		using reference         = typename Iter::reference;

		// Конструктор класса от обычного итератора
		explicit Reverse_iterator(Iter iter) : iter_(iter.instance_, \
			static_cast<difference_type>(iter.index_) - 1, iter.type_) {}
		
		// Конструктор класса, подобный обычному итератору
		explicit Reverse_iterator(Cyclic_buffer* instance, difference_type index = 0,
			iter_type const& type = iter_type::MID)
			: iter_(instance, index - 1, type) {}
		
		// Метод swap
		void swap(Reverse_iterator& other) {
			using std::swap;
			swap(iter_, other.iter_);
		}
		
		// Деструктор
		~Reverse_iterator() {}

		// Конструктор копирования
		Reverse_iterator(Reverse_iterator const& other) : iter_(other.iter_) {}

		// Перемещающий конструктор
		Reverse_iterator(Reverse_iterator&& other) {
			swap(other);
		}
		
		// Оператор присваивания
		Reverse_iterator& operator=(Reverse_iterator const& other) {
			iter_ = other.iter_;
			return *this;
		}
		
		// Перемещающий оператор присваивания
		Reverse_iterator& operator=(Reverse_iterator&& other) {
			swap(other);
			return *this;
		}
		
		// Оператор разыменования
		reference operator*() const {
			return *iter_;
		}

		// Получение указателя
		pointer operator->() const {
			return iter_.operator->();
		}
		
		// Префиксный инкремент
		Reverse_iterator& operator++() {
			if (iter_.index_ == iter_.instance_->head_) {
				iter_.index_ = iter_.instance_->tail_;
				iter_.type_ = iter_type::HEAD;
			}
			else {
				if (iter_.type_ == iter_type::HEAD) {
					--iter_;
					iter_.type_ = iter_type::HEAD;
				}
				else {
					--iter_;
					if (iter_.type_ == iter_type::HEAD) {
						iter_.type_ = iter_type::MID;
					}
				}
			}
			return *this;
		}
		
		// Постфиксный инкремент
		Reverse_iterator operator++(int) {
			Reverse_iterator tmp(*this);
			++(*this);
			return tmp;
		}
		
		// Префиксный декремент
		Reverse_iterator& operator--() {
			iter_type old_type = iter_.type_;
			++iter_;
			if (iter_.type_ != iter_type::TAIL || old_type == iter_type::HEAD) {
				iter_.type_ = iter_type::MID;
			}
			return *this;
		}
		
		// Постфиксный декремент
		Reverse_iterator operator--(int) {
			Reverse_iterator tmp(*this);
			--(*this);
			return tmp;
		}
		
		// Оператор +=
		Reverse_iterator& operator+=(difference_type i) {
			size_type rel_index = iter_.relative_index();
			iter_type old_type = iter_.type_;
			iter_ -= i;
			if (i < 0 && (static_cast<size_type>(std::abs(i)) + rel_index >= iter_.instance_->size_ 
				|| old_type == iter_type::TAIL)) {
				iter_.type_ = iter_type::TAIL;
			}
			else if (i >= 0 && (static_cast<size_type>(i) > rel_index || old_type == iter_type::HEAD)) {
				iter_.type_ = iter_type::HEAD;
			}
			else {
				iter_.type_ = iter_type::MID;
			}
			return *this;
		}

		// Оператор -=
		Reverse_iterator& operator-=(difference_type i) {
			*this += -i;
			return *this;
		}

		// Оператор сложения с числом
		friend Reverse_iterator operator+(Reverse_iterator lhs, difference_type rhs) {
			lhs += rhs;
			return lhs;
		}
		
		// Оператор сложения с числом
		friend Reverse_iterator operator+(difference_type lhs, Reverse_iterator rhs) {
			return rhs + lhs;
		}
		
		// Оператор вычитания числа
		friend Reverse_iterator operator-(Reverse_iterator lhs, difference_type rhs) {
			lhs -= rhs;
			return lhs;
		}
		
		// Оператор []
		reference operator[](difference_type i) const {
			return *(*this + i);
		}

		// Оператор разности с итератором
		difference_type operator-(Reverse_iterator const& rhs) const {
			return rhs.iter_ - iter_;
		}

		// Оператор сравнения ==
		bool operator==(Reverse_iterator const& other) const {
			return iter_ == other.iter_;
		}

		// Оператор сравнения !=
		bool operator!=(Reverse_iterator const& other) const {
			return !(*this == other);
		}

		// Оператор сравнения <
		bool operator<(Reverse_iterator const& other) const {
			return other - *this > 0;
		}
		
		// Оператор сравнения >
		bool operator>(Reverse_iterator const& other) const {
			return other < *this;
		}
		
		// Оператор сравнения >=
		bool operator>=(Reverse_iterator const& other) const {
			return !(*this < other);
		}
		
		// Оператор сравнения <=
		bool operator<=(Reverse_iterator const& other) const {
			return !(*this > other);
		}
		
		// Приведение к обычному итератору
		Iter base() const {
			return Iter(iter_.instance_, iter_.ptr_, iter_.index_ + 1, iter_.type_);
		}
		
		// Приведение к константному обратному итератору
		operator Reverse_iterator<const Iter>() const {
			return Reverse_iterator<const Iter>(iter_);
		}

		friend class Cyclic_buffer;
	private:
		Iter iter_;
	};
	
	// Итератор начала
	iterator begin() {
		return iterator(this, head_, iter_type::HEAD);
	}
	
	// Итератор конца
	iterator end() {
		if (empty()) {
			return begin();
		}
		return iterator(this, tail_ + 1, iter_type::TAIL);
	}
	
	// Константный итератор начала
	const_iterator cbegin() {
		return const_iterator(this, head_, iter_type::HEAD);
	}
	
	// Константный итератор конца
	const_iterator cend() {
		if (empty()) {
			return cbegin();
		}
		return const_iterator(this, tail_ + 1, iter_type::TAIL);
	}
	
	// Обратный итератор начала
	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}
	
	// Обратный итератор конца
	reverse_iterator rend() {
		return reverse_iterator(begin());
	}
	
	// Константный обратный итератор начала
	const_reverse_iterator crbegin() {
		return const_reverse_iterator(cend());
	}
	
	// Константный обратный итератор конца
	const_reverse_iterator crend() {
		return const_reverse_iterator(cbegin());
	}


private:
	size_type capacity_;						   // Вместительность буфера
	size_type size_;							   // Размер буфера
	size_type head_;							   // Индекс начала буфера
	size_type tail_;							   // Индекс конца буфера
	allocator_type alloc_;						   // Настриваемый аллокатор
	deleter_type del_;							   // Настраиваемый очиститель
	std::shared_mutex mutex_;					   // Мьютекс для безопасной работы с многопоточностью
	std::unique_ptr<value_type[], Deleter> data_;  // Само хранилище буфера

	// Функция очистки выделенной памяти
	void destroy_data(size_t n) {
		write_lock lock(mutex_);
		pointer p = data_.get();
		for (size_t i = 0; i < n; ++i, ++p) {
			std::allocator_traits<Allocator>::destroy(alloc_, p);
		}
	}

	// Псевдонимы типов для внутреннего использования
	using lock_t = std::shared_mutex;
	using write_lock = std::unique_lock<lock_t>;
	using read_lock = std::shared_lock<lock_t>;
};




#endif
