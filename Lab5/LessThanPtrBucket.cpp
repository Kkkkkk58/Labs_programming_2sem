#include "FixedSizeBucket.hpp"

// Конструктор класса LessThanPtrBucket
LessThanPtrBucket::LessThanPtrBucket(void* data, std::size_t chunks_number, std::size_t chunk_size)
	: FixedSizeBucket(), free_list_(new std::vector<bool>(chunks_number, true)), begin_(data) {}


// Внутренняя реализация аллокации кусочка памяти
void* LessThanPtrBucket::alloc(std::size_t typesize) {
	// Ищем свободный кусочек
	auto founder = std::find_if(free_list_->begin(), free_list_->end(), [](bool elem) { return elem; });
	// Если такой имеется
	if (founder != free_list_->end()) {
		// Помечаем его занятым
		*founder = false;
		// Возвращаем указатель на него
		return static_cast<void*>(static_cast<char*>(begin_) + (founder - free_list_->begin()) * typesize);
	}
	// Если свободного кусочка не нашлось
	return nullptr;
}


// Реализация освобождения кусочка памяти - пометка об освобождении
void LessThanPtrBucket::mark_free(void* ptr, std::size_t typesize) {
	free_list_->operator[]((static_cast<char*>(ptr) - static_cast<char*>(begin_)) / typesize) = true;
}