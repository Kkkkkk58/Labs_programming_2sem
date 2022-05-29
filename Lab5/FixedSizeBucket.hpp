#ifndef FIXED_SIZE_BUCKET_HPP
#define FIXED_SIZE_BUCKET_HPP
#include "Chunk.hpp"
#include <algorithm>
#include <vector>


class FixedSizeBucket {
public:
	virtual ~FixedSizeBucket() {}
	virtual void* alloc(std::size_t typesize) = 0;
	virtual void mark_free(void* ptr, std::size_t typesize) = 0;
};


class LessThanPtrBucket : public FixedSizeBucket {
public:
	LessThanPtrBucket(void* data, size_t chunks_number, size_t chunk_size)
		: FixedSizeBucket(), free_list_(new std::vector<bool>(chunks_number, true)), begin_(data) {}

	void* alloc(std::size_t typesize) override {
		std::cout << "smallbkt\n";
		auto founder = std::find_if(free_list_->begin(), free_list_->end(), [](bool elem) { return elem; });
		if (founder != free_list_->end()) {
			*founder = false;
			return static_cast<void*>(static_cast<char*>(begin_) + (founder - free_list_->begin()) * typesize);
		}
		return nullptr;
	}

	void mark_free(void* ptr, std::size_t typesize) override {
		free_list_->operator[]((static_cast<char*>(ptr) - static_cast<char*>(begin_)) / typesize) = true;
	}
private:
	std::unique_ptr<std::vector<bool>> free_list_;
	void* begin_;
};


class MoreOrEqThanPtrBucket : public FixedSizeBucket {
public:
	MoreOrEqThanPtrBucket(void* data, size_t chunks_number, size_t chunk_size)
		: FixedSizeBucket(), free_list_(static_cast<Chunk*>(data)) {
		free_list_->next_ = nullptr;
		Chunk* prev = free_list_;
		for (size_t i = 1; i < chunks_number; ++i) {
			Chunk* curr = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(prev) + chunk_size);
			prev->next_ = curr;
			prev = curr;
		}
	}
	void* alloc(std::size_t typesize) override {
		std::cout << "bigbkt\n";
		Chunk* ptr = free_list_;
		if (ptr != nullptr) {
			std::cout  << ptr << "\n";
			free_list_ = ptr->next_;
		}
		return static_cast<void*>(ptr);
	}
	void mark_free(void* ptr, std::size_t typesize) override {
		static_cast<Chunk*>(ptr)->next_ = free_list_;
		free_list_ = static_cast<Chunk*>(ptr);
	}
private:
	Chunk* free_list_;
};
#endif