#ifndef LJGS_ALLOCATOR_HPP
#define LJGS_ALLOCATOR_HPP
#include <map>
#include "FixedSizeBucket.hpp"

// Шаблонный класс аллокатора
template<typename T>
class LJGSAllocator {
public:
	using value_type							 = T;
	using size_type								 = unsigned long long;
	using difference_type						 = long long;
	using pointer								 = T*;
	using const_pointer							 = const T*;
	using void_pointer							 = void*;
	using const_void_pointer					 = const void*;
	using is_always_equal                        = std::false_type;
	using propagate_on_container_copy_assignment = std::true_type;
	using propagate_on_container_move_assignment = std::true_type;
	using propagate_on_container_swap            = std::true_type;
	template <typename V>
	struct rebind {
		using other = LJGSAllocator<V>;
	};

	// Конструктор класса от мапы, ключами которой являются размеры кусочков, а значениями - их количества
	explicit LJGSAllocator(std::map<std::size_t, std::size_t> const& chunks_info) 
	: size_groups_(new std::map<std::size_t, FixedSizeBucket*>()) {
		size_type total_size = 0;
		// Перебираем пары размеров и количеств кусочков и добавляем величину в размер общего пула
		for (auto const& [size, count] : chunks_info) {
			total_size += count * size;
			// Если размер кусочка не меньше размера указателя
			if (size >= sizeof(Chunk*)) {
				// Будем использовать соответствующую EqOrMoreThanPtrBucket стратегию
				total_size += sizeof(EqOrMoreThanPtrBucket);
			}
			// Иначе
			else {
				// Пользуемся классом LessThanPtrBucket
				total_size += sizeof(LessThanPtrBucket);
			}
		}
		// Пробуем выделить требуемое количество байт
		try {
			data_.reset(::operator new(total_size));
		}
		// Если не вышло - перебрасываем исключение
		catch (std::exception const& e) {
			std::cerr << e.what();
			throw;
		}
		// Указатель, по которому будут размещаться объекты в памяти
		char* tmp = static_cast<char*>(data_.get());
		// Располагаем кусочки по выделенным местам
		for (auto const& [size, count] : chunks_info) {
			// Размещение кусочков, размер которых меньше размера указателя
			if (size < sizeof(Chunk *)) {
				size_groups_->operator[](size) = new(static_cast<void*>(tmp)) \
					LessThanPtrBucket(static_cast<void*>(tmp + sizeof(LessThanPtrBucket)), count, size);
				tmp += count * size + sizeof(LessThanPtrBucket);
			}
			// Размещение кусочков, размер которых не меньше размера указателя
			else {
				size_groups_->operator[](size) = new(static_cast<void*>(tmp)) \
					EqOrMoreThanPtrBucket(static_cast<void*>(tmp + sizeof(EqOrMoreThanPtrBucket)), count, size);
				tmp += count * size + sizeof(EqOrMoreThanPtrBucket);
			}
		}
	}

	// Деструктор класса LJGSAllocator
	~LJGSAllocator() {
		// Если данный аллокатор - последний из указывающих на конкретную область памяти
		if (size_groups_.use_count() == 1) {
			destroy_blocks();
		}
	}

	// Конструктор копирования класса LJGSAllocator, не бросающий исключения
	LJGSAllocator(LJGSAllocator const& other) noexcept : size_groups_(other.size_groups_), data_(other.data_) {}

	// Конструктор копирования класса LJGSAllocator от аллокатора другого типа, не бросающий исключения
	template<typename V>
	LJGSAllocator(LJGSAllocator<V> const& other) noexcept : size_groups_(other.groups()), data_(other.data()) {}

	// Конструктор перемещения класса LJGSAllocator, не бросающий исключения
	LJGSAllocator(LJGSAllocator&& other) noexcept
		: size_groups_(std::move(other.size_groups_)), data_(std::move(other.data_)) {}

	// Конструктор перемещения класса LJGSAllocator от аллокатора другого типа, не бросающий исключения
	template<typename V>
	LJGSAllocator(LJGSAllocator&& other) noexcept
		: size_groups_(std::move(other.groups())), data_(std::move(other.data())) {}

	// Оператор присваивания класса LJGSAllocator
	LJGSAllocator& operator=(LJGSAllocator const& other) noexcept {
		if (*this != other) {
			destroy_blocks();
			size_groups_ = other.size_groups_;
			data_ = other.data_;
		}
		return *this;
	}

	// Оператор присваивания класса LJGSAllocator от аллокатора другого типа
	template<typename V>
	LJGSAllocator& operator=(LJGSAllocator<V> const& other) noexcept {
		if (*this != other) {
			destroy_blocks();
			size_groups_ = other.groups();
			data_ = other.data();
		}
		return *this;
	}

	// Перемещающий оператор присваивания класса LJGSAllocator
	LJGSAllocator& operator=(LJGSAllocator &&other) noexcept {
		if (*this != other) {
			destroy_blocks();
			size_groups_ = std::move(other.size_groups_);
			data_ = std::move(other.data_);
		}
		return *this;
	}

	// Перемещающий оператор присваивания класса LJGSAllocator от аллокатора другого типа
	template<typename V>
	LJGSAllocator& operator=(LJGSAllocator<V> && other) noexcept {
		if (*this != other) {
			destroy_blocks();
			size_groups_ = std::move(other.groups());
			data_ = std::move(other.data());
		}
		return *this;
	}

	// Метод swap класса LJGSAllocator
	void swap(LJGSAllocator& other) noexcept {
		using std::swap;
		swap(size_groups_, other.size_groups_);
		swap(data_, other.data_);
	}

	// Метод allocate, предоставляющий место для n подряд идущих элементов типа T в динамической памяти
	pointer allocate(std::size_t n) {
		// Просматриваем доступные блоки, начиная с наименьшего из тех, которые могут вместить эти элементы
		for (auto it = size_groups_->lower_bound(n * sizeof(T)); it != size_groups_->end(); ++it) {

			pointer ptr = static_cast<T*>(it->second->alloc(sizeof(T)));
			// Если внутренний alloc возвращает указатель, отличный от nullptr - место нашлось
			if (ptr != nullptr) {
				return ptr;
			}
		}
		// Если среди блоков не найдено место
		std::cerr << "Didn't manage to find " << n * sizeof(T) << " block\n";
		throw std::bad_alloc();
	}

	// Метод deallocate, освобождающее место, ранее предоставленное по указателю ptr из текущего хранилища
	void deallocate(T* ptr, std::size_t n) {
		// Ищем блок, которому мог принадлежать такой указатель, начиная с блоков размера >= n * sizeof(T)
		for (auto it = size_groups_->lower_bound(n * sizeof(T)); it != size_groups_->end(); ++it) {
			if (static_cast<void*>(it->second) <= static_cast<void*>(ptr)) {
				it->second->mark_free(static_cast<void*>(ptr), sizeof(T));
			}
		}
	}

	// Метод construct, создающий объект по выданному указателю
	template<typename V, typename... Args >
	void construct(V* ptr, Args&&... args) {
		// Placement new + Perfect forwarding
		new(static_cast<void*>(ptr)) V(std::forward<Args>(args)...);
	}

	// Метод destroy, вызывающий деструктор объекта, содержащегося по указателю
	template<typename V>
	void destroy(V* ptr) {
		ptr->~V();
	}

	// Метод max_size, возвращающий размер наибольшего чанка в байтах
	size_type max_size() const {
		if (!size_groups_->empty()) {
			return size_groups_->rbegin()->first;
		}
		return 0;
	}

	// Метод доступа к полю data_
	std::shared_ptr<void> const& data() const {
		return data_;
	}
	
	// Метод доступа к полю size_groups_
	std::shared_ptr<std::map<std::size_t, FixedSizeBucket*>> const& groups() const {
		return size_groups_;
	}

private:
	// Указатель на мапу, ключами которой являются размеры блоков, а соответсвующими значениями - указатели на эти блоки
	std::shared_ptr<std::map<std::size_t, FixedSizeBucket*>> size_groups_;
	// Указатель на выделенный блок памяти
	std::shared_ptr<void> data_;
	// Метод destroy_blocks
	void destroy_blocks() {
		// Вызываем деструкторы вместилищ чанков
		for (auto it = size_groups_->begin(); it != size_groups_->end(); ++it) {
			it->second->~FixedSizeBucket();
		}
	}
};

// Оператор сравнения аллокаторов (равны, если имеют доступ к одному и тому же блоку памяти)
template <class T, class U>
bool operator==(LJGSAllocator<T> const& lhs, LJGSAllocator<U> const& rhs) noexcept { return lhs.data() == rhs.data(); }

// Оператор сравнения аллокаторов (не равны, если работают с разными блоками памяти)
template <class T, class U>
bool operator!=(LJGSAllocator<T> const& lhs, LJGSAllocator<U> const& rhs) noexcept { return !(lhs == rhs); }


#endif
