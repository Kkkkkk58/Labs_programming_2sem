#include "FixedSizeBucket.hpp"

// Конструктор класса EqOrMoreThanPtrBucket
EqOrMoreThanPtrBucket::EqOrMoreThanPtrBucket(void* data, size_t chunks_number, size_t chunk_size)
	: FixedSizeBucket(), free_list_(static_cast<Chunk*>(data)) {
	// Инициализуируем free_list
	free_list_->next_ = nullptr;
	Chunk* prev = free_list_;
	Chunk* curr = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(prev) + chunk_size);
	// Расставляем указатели на свободные блоки
	for (size_t i = 1; i < chunks_number; ++i) {
		prev->next_ = curr;
		prev = curr;
		curr = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(prev) + chunk_size);
	}
}


// Внутренняя реализация аллокации кусочка памяти
void* EqOrMoreThanPtrBucket::alloc(std::size_t) {
	// Получаем указатель на первый свободный элемент
	Chunk* ptr = free_list_;
	if (ptr != nullptr) {
		// Меняем текущий указатель на первый свободный элемент
		free_list_ = ptr->next_;
	}
	return static_cast<void*>(ptr);
}


// Реализация освобождения кусочка памяти - пометка об освобождении
void EqOrMoreThanPtrBucket::mark_free(void* ptr, std::size_t) {
	// Переставляем указатели головы и текущего освобожденного элемента
	static_cast<Chunk*>(ptr)->next_ = free_list_;
	free_list_ = static_cast<Chunk*>(ptr);
}