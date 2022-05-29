#ifndef CHUNK_HPP
#define CHUNK_HPP

// Структура, хранящая указатель на свободный чанк для реализации концепции Freelist
struct Chunk {
	Chunk* next_ = nullptr;
};

#endif