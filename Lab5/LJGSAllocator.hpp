#ifndef LJGS_ALLOCATOR_HPP
#define LJGS_ALLOCATOR_HPP
#include <unordered_map>
#include <map>
#include <memory>
#include "FixedSizeBucket.hpp"


template<typename T>
class LJGSAllocator {
public:
	using value_type = T;
	using size_type = unsigned long long;
	using difference_type = long long;
	using pointer = T*;
	using const_pointer = const T*;
	using void_pointer = void*;
	using const_void_pointer = const void*;
	template <typename V>
	struct rebind {
		using other = LJGSAllocator<V>;
	};
	explicit LJGSAllocator(std::map<size_t, size_t> const& chunks_info) 
	: size_groups_(new std::map<std::size_t, FixedSizeBucket*>()) {
		size_type total_size = 0;
		
		for (auto const& [size, count] : chunks_info) {
			total_size += count * size;
			if (size >= sizeof(Chunk*)) {
				total_size += sizeof(MoreOrEqThanPtrBucket);
			}
			else {
				total_size += sizeof(LessThanPtrBucket);
			}
		}
		try {
			data_.reset(::operator new(total_size));
		}
		catch (std::exception const& e) {
			std::cerr << "Cringe\n" << e.what();
			throw;
		}
		char* tmp = static_cast<char*>(data_.get());
		for (auto const& [size, count] : chunks_info) {
			std::cout << size << "\n";
			if (size < sizeof(Chunk *)) {
				size_groups_->operator[](size) = new(static_cast<void*>(tmp)) LessThanPtrBucket(static_cast<void*>(tmp + sizeof(LessThanPtrBucket)), count, size);
				tmp += count * size + sizeof(LessThanPtrBucket);
			}
			else {
				size_groups_->operator[](size) = new(static_cast<void*>(tmp)) MoreOrEqThanPtrBucket(static_cast<void*>(tmp + sizeof(MoreOrEqThanPtrBucket)), count, size);
				tmp += count * size + sizeof(MoreOrEqThanPtrBucket);
			}
		}
	}
	~LJGSAllocator() {
		if (size_groups_.use_count() == 1) {
			for (auto it = size_groups_->begin(); it != size_groups_->end(); ++it) {
				it->second->~FixedSizeBucket();
			}
		}
	}
	LJGSAllocator(LJGSAllocator const& other) : size_groups_(other.size_groups_), data_(other.data_) {}

	template<typename V>
	LJGSAllocator(LJGSAllocator<V> const& other) : size_groups_(other.groups()), data_(other.data()) {}

	pointer allocate(std::size_t n) {
		
		for (auto it = size_groups_->lower_bound(n * sizeof(T)); it != size_groups_->end(); ++it) {
			std::cout << "HEEEE\n";
			pointer ptr = static_cast<T*>(it->second->alloc(sizeof(T)));
			if (ptr != nullptr) {
				std::cout << "Allocated " << it->first << "bytes at " << ptr << "\n";
				return ptr;
			}
		}
		std::cout << "Didn't manage to find " << n * sizeof(T) << "block\n";
		throw std::bad_alloc();
	}
	void deallocate(T* ptr, std::size_t n) {
		for (auto it = size_groups_->lower_bound(n * sizeof(T)); it != size_groups_->end(); ++it) {
			if (static_cast<void*>(it->second) <= static_cast<void*>(ptr)) {
				it->second->mark_free(static_cast<void*>(ptr), sizeof(T));
			}
		}
	}
	template<typename V, typename... Args >
	void construct(V* ptr, Args&&... args) {
		std::cout << "CONSTR " << ptr << "\n";
		new(static_cast<void*>(ptr)) V(std::forward<Args>(args)...);
	}

	template<typename V>
	void destroy(V* ptr) {
		for (auto it = size_groups_->lower_bound(sizeof(V)); it != size_groups_->end(); ++it) {
			if (static_cast<void*>(it->second) <= static_cast<void*>(ptr)) {
				ptr->~V();
			}
		}
	}
	size_type max_size() const {
		size_type size = 0;
		if (!size_groups_->empty()) {
			for (auto it = size_groups_->begin(); it != size_groups_->end(); ++it) {
				size = std::max(it->first, size);
			}
		}
		return size;
	}
	std::shared_ptr<void>const& data() const {
		return data_;
	}
	std::shared_ptr<std::map<std::size_t, FixedSizeBucket*>> const& groups() const {
		return size_groups_;
	}

private:
	std::shared_ptr<std::map<std::size_t, FixedSizeBucket*>> size_groups_;
	std::shared_ptr<void> data_;
};


template <class T, class U>
bool operator==(LJGSAllocator<T> const& lhs, LJGSAllocator<U> const& rhs) { return lhs.data() == rhs.data(); }
template <class T, class U>
bool operator!=(LJGSAllocator<T> const& lhs, LJGSAllocator<U> const& rhs) { return !(lhs == rhs); }


#endif
