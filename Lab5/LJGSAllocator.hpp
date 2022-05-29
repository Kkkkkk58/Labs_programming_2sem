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
				total_size += sizeof(Chunk *);
			}
		}
		try {
			//data_ = std::make_shared<void*>(operator new(total_size));
			data_ = operator new(total_size);
		}
		catch (std::exception const& e) {
			std::cerr << "Cringe\n" << e.what();
			throw;
		}
		std::cout << total_size << "\n";
		//char* tmp = static_cast<char*>(data_.get());
		char* tmp = static_cast<char*>(data_);
		for (auto const& [size, count] : chunks_info) {
			std::cout << size << "\n";
			if (size < sizeof(Chunk *)) {
				size_groups_->operator[](size) = new(static_cast<void*>(tmp)) LessThanPtrBucket(static_cast<void*>(tmp), count, size);
				tmp += count * size;
			}
			else {
				size_groups_->operator[](size) = new(static_cast<void*>(tmp)) MoreOrEqThanPtrBucket(static_cast<void*>(tmp), count, size);
				tmp += count * size + sizeof(Chunk*);
			}
		}
	}

	LJGSAllocator(LJGSAllocator const& other) : size_groups_(other.size_groups_), data_(other.data_) {}
	//~LJGSAllocator() {
	//	delete data;
	//}
	pointer allocate(std::size_t n) {
		auto it = size_groups_->lower_bound(n * sizeof(T));
		std::cout << it->first;
		for ( ; it != size_groups_->end(); ++it) {
			std::cout << "HEEEE\n";
			pointer ptr = it->second->do_allocate<T>();
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
			if (it->second->contains<T>(ptr)) {
				it->second->do_deallocate<T>(ptr);
			}
		}
	}
	template<typename V, typename... Args >
	void construct(V* ptr, Args&&... args) {
		for (auto it = size_groups_->lower_bound(sizeof(V)); it != size_groups_->end(); ++it) {
			if (it->second->contains<T>(ptr)) {
				it->second->do_construct<V, Args...>(ptr, std::forward<Args>(args)...);
			}
		}
	}
	template<typename V>
	void destroy(V* ptr) {
		for (auto it = size_groups_->lower_bound(sizeof(V)); it != size_groups_->end(); ++it) {
			if (it->second->contains<T>(ptr)) {
				it->second->do_destroy<T>(ptr);
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
	void* data() const {
		return data_;
	}

	template<typename V>
	operator LJGSAllocator<V>() {
		return LJGSAllocator(size_groups_, data_);
	}

private:
	//template<typename V> friend class LJGSAllocator;
	std::map<std::size_t, FixedSizeBucket*> *size_groups_;
	//std::shared_ptr<void> data_;
	void* data_;

	LJGSAllocator(std::map<std::size_t, FixedSizeBucket*> *size_groups, void* data) 
		: size_groups_(size_groups), data_(data) {}
};


template <class T, class U>
bool operator==(LJGSAllocator<T> const& lhs, LJGSAllocator<U> const& rhs) { return lhs.data() == rhs.data(); }
template <class T, class U>
bool operator!=(LJGSAllocator<T> const& lhs, LJGSAllocator<U> const& rhs) { return !(lhs == rhs); }
#endif
