#ifndef FIXED_SIZE_BUCKET_HPP
#define FIXED_SIZE_BUCKET_HPP
#include "Chunk.hpp"
#include <algorithm>
#include <vector>


class FixedSizeBucket {
public:
	virtual ~FixedSizeBucket() {}
	template<typename T>
	T* do_allocate() { 
		std::cout << "Cringe\n";
		return nullptr; }
	template<typename T>
	bool contains(T* ptr) { return false; }
	template<typename T>
	void do_destroy(T* ptr) {}
	template<typename T>
	void do_deallocate(T* ptr) {}
	template<typename T, typename... Args>
	void do_construct(T* ptr, Args&&... args) {
		if (contains(ptr)) {
			new(static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
		}
	}
};


class LessThanPtrBucket : public FixedSizeBucket {
public:
	LessThanPtrBucket(void* data, size_t chunks_number, size_t chunk_size)
		: FixedSizeBucket(), free_list_(new std::vector<bool>(chunks_number, true)), begin_(data),
		end_(static_cast<void*>(static_cast<char*>(data) + chunks_number * chunk_size)) {}
	~LessThanPtrBucket() = default;

	template<typename T>
	T* do_allocate()  {
		std::cout << "smallbkt\n";
		auto founder = std::find_if(free_list_->begin(), free_list_->end(), [](bool elem) { return elem; });
		if (founder != free_list_.end()) {
			*founder = false;
			return static_cast<T*>(begin_ + (founder - free_list_.begin()) * sizeof(T));
		}
		return nullptr;
	}

	template<typename T>
	bool contains(T* ptr) {
		return static_cast<void*>(ptr) >= begin_ && static_cast<void*>(ptr) <= end_;;
	}

	template<typename T>
	void do_destroy(T* ptr)  {
		if (contains(ptr)) {
			ptr->~T();
		}
		else {
			throw std::logic_error("CRINGE\n");
		}
	}

	template<typename T>
	void do_deallocate(T* ptr, size_t n)  {
		try {
			do_destroy(ptr);
			free_list_->operator[](ptr - static_cast<T*>(begin_)) = true;
		}
		catch (std::exception const& e) {
			std::cerr << "BRUH\n" << e.what();
			throw;
		}
	}

private:
	std::vector<bool>* free_list_;
	void* begin_, *end_;
};


class MoreOrEqThanPtrBucket : public FixedSizeBucket {
public:
	MoreOrEqThanPtrBucket(void* data, size_t chunks_number, size_t chunk_size)
		: FixedSizeBucket(), begin_(static_cast<void*>(static_cast<char*>(data) + sizeof(Chunk *))), 
		end_(static_cast<void*>(static_cast<char*>(begin_) + chunks_number * chunk_size)),
		free_list_(static_cast<Chunk*>(begin_)) {
		Chunk* prev = static_cast<Chunk*>(begin_);
		Chunk* curr = prev + 1;
		while (prev != static_cast<Chunk*>(end_)) {
			prev->next_ = curr;
			prev = curr;
			++curr;
		}
	}
	template<typename T>
	T* do_allocate()  {
		std::cout << "bigbkt\n";
		Chunk* ptr = free_list_;
		if (ptr != nullptr) {
			free_list_ = ptr->next_;
		}
		return reinterpret_cast<T*>(ptr);
	}
	template<typename T>
	bool contains(T* ptr)  {
		return static_cast<void*>(ptr) >= begin_ && static_cast<void*>(ptr) <= end_;
	}
	template<typename T>
	void do_destroy(T* ptr)  {
		if (contains(ptr)) {
			ptr->~T();
		}
		else {
			throw std::logic_error("CRINGE2\n");
		}
	}
	template<typename T>
	void do_deallocate(T* ptr)  {
		try {
			do_destroy(ptr);
			reinterpret_cast<Chunk*>(ptr)->next_ = free_list_;
			free_list_ = reinterpret_cast<Chunk*>(ptr);
		}
		catch (std::exception const& e) {
			std::cerr << "BRUH\n" << e.what();
			throw;
		}
	}	
private:
	void* begin_, *end_;
	Chunk* free_list_;
};
#endif