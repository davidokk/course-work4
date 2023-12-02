#pragma once 

void RunTests();

template <typename SufTree>
void SimpleTest();

void SimpleTestHash();
void SimpleTestArray();

void UnicodeStringTest();
void SequenceTest();

template <typename SufTree>
void StressTest();

void StressTestHash();
void StressTestTree();
void StressTestArray(); 

void IndexerTest();