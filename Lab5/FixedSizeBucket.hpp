#ifndef FIXED_SIZE_BUCKET_HPP
#define FIXED_SIZE_BUCKET_HPP
#include "Chunk.hpp"
#include <vector>
#include <memory>

// Интерфейс блоков - вместилищ кусочков одного размера
class FixedSizeBucket {
public:
	virtual ~FixedSizeBucket() {}
	virtual void* alloc(std::size_t) = 0;
	virtual void mark_free(void*, std::size_t) = 0;
};


// Класс вместилища кусочков размера, превышающего размер указателя
class EqOrMoreThanPtrBucket : public FixedSizeBucket {
public:
	EqOrMoreThanPtrBucket(void*, std::size_t, std::size_t);
	void* alloc(std::size_t) override;
	void mark_free(void*, std::size_t) override;
private:
	Chunk* free_list_;		// Голова списка указателей на свободные кусочки
};


// Класс вместилища кусочков размера, меньшего чем размер указателя
class LessThanPtrBucket : public FixedSizeBucket {
public:
	LessThanPtrBucket(void*, std::size_t, std::size_t);
	void* alloc(std::size_t typesize) override;
	void mark_free(void*, std::size_t) override;
private:
	std::unique_ptr<std::vector<bool>> free_list_;		// Свободные кусочки помечаются true в векторе
	void* begin_;										// Указатель на начало блока - нужен для получения индекса кусочка
};

#endif