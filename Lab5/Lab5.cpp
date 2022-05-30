#include <iostream>
#include "LJGSAllocator.hpp"
#include "Cyclic_buffer.hpp"
#include <vector>
#include <list>
#include <chrono>
#include "benchmark/benchmark.h"

void RegularSingleAllocation(std::size_t size) {
    std::allocator<int> a;
    int* res = a.allocate(size);
    a.deallocate(res, size);
}

static void BM_RegularSingleAllocation(benchmark::State& state) {
    for (auto _ : state)
        RegularSingleAllocation(state.range(0));
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_RegularSingleAllocation)->Range(8, 8<<10)->Complexity(benchmark::oN);

void CustomSingleAllocation(std::size_t size) {
    LJGSAllocator<int> a(std::map<std::size_t, std::size_t>{{size * sizeof(int), 1}});
    int* res = a.allocate(size);
    a.deallocate(res, size);
}

static void BM_CustomSingleAllocation(benchmark::State& state) {
    for (auto _ : state)
        RegularSingleAllocation(state.range(0));
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_CustomSingleAllocation)->Range(8, 8 << 10)->Complexity(benchmark::oN);

void RegularVectorCreate(std::size_t size) {
    std::allocator<int> a;
    std::vector<int, std::allocator<int>> v(size, a);
    //benchmark::DoNotOptimize(v.data());
    std::vector<int, std::allocator<int>> k(v);
}

static void BM_RegularVectorCreate(benchmark::State& state) {
    for (auto _ : state)
        RegularVectorCreate(state.range(0));
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_RegularVectorCreate)->Range(8, 8<<10)->Complexity(benchmark::oN);


void CustomVectorCreate(std::size_t size, benchmark::State& state) {
    LJGSAllocator<int> a({ {size * sizeof(int), 2}});
    std::vector<int, LJGSAllocator<int>> v(size, a);
    std::vector<int, LJGSAllocator<int>> k(v);
}

static void BM_CustomVectorCreate(benchmark::State& state) {
    for (auto _ : state)
        CustomVectorCreate(state.range(0), state);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_CustomVectorCreate)->Range(8, 8<<10)->Complexity(benchmark::oN);


void RegularVectorPushBack(std::size_t size) {
    std::allocator<int> a;
    std::vector<int, std::allocator<int>> v(1024, a);

    for (std::size_t _ = 0; _ < size; ++_) {
        v.push_back(42);
    }
}

static void BM_RegularVectorPushBack(benchmark::State& state) {
    for (auto _ : state)
        RegularVectorPushBack(state.range(0));
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_RegularVectorPushBack)->RangeMultiplier(2)->Range(8, 1024)->Complexity(benchmark::oN);

void CustomVectorPushBack(std::size_t size, benchmark::State &state) {
    LJGSAllocator<int> a(std::map<std::size_t, std::size_t>{ {1024 * sizeof(int), 1}, { 6144, 1 }, {9216, 1}});
    std::vector<int, LJGSAllocator<int>> v(1024, a);
    auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t _ = 0; _ < size; ++_) {
        v.push_back(42);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

    state.SetIterationTime(elapsed_seconds.count());
}

static void BM_CustomVectorPushBack(benchmark::State& state) {
    for (auto _ : state)
        CustomVectorPushBack(state.range(0), state);
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_CustomVectorPushBack)->RangeMultiplier(2)->Range(8, 1024)->Complexity(benchmark::oN);


BENCHMARK_MAIN();
