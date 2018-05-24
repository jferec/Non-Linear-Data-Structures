#include <cstddef>
#include <cstdlib>
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>

#include "TreeMap.h"
#include "HashMap.h"

namespace
{

template <typename K, typename V>
using Map = aisdi::HashMap<K, V>;
template <typename K, typename V>
using Tree = aisdi::TreeMap<K, V>;

void perfomTest(std::size_t repeatCount, std::size_t tableSize)
{
  (void)repeatCount;
  (void)tableSize;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  std::chrono::duration<double> elapsed_seconds;

  Map<int, std::string> map;
  Tree<int, std::string> tree;

  std::cout << "operacja przeprowadzona na:" << repeatCount << " elementow\n";

  {

    start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; i++)
      map[rand()%1000000]= "word";
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "\t dodawanie elementow w Hashmapie (losowo klucze)\t czas: " << elapsed_seconds.count() << "s\n";
  }

  {

    start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; i++)
      tree[rand()%1000000]= "word";
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "\t dodawanie elementow w Drzewie (losowo klucze)\t czas: " << elapsed_seconds.count() << "s\n";
  }
/*
  {

    start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; i++)
      tree[i]= "word";
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "\t dodawanie elementow w Drzewie (rosnace klucze)\t czas: " << elapsed_seconds.count() << "s\n";
  }

  {

    start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; i++)
      map[i]= "word";
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "\t dodawanie elementow w Drzewie (rosnace klucze)\t czas: " << elapsed_seconds.count() << "s\n";
  }

*/
}

} // namespace

int main(int argc, char** argv)
{
  const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 100000;
  const std::size_t tableSize   = argc > 2 ? std::atoll(argv[2]) : 100000;
  for (std::size_t i = 0; i < 5; ++i)
  perfomTest(repeatCount, tableSize);
  return 0;
}
