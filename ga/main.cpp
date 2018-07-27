#include <bitset>
#include <iostream>
#include <random>
#include <vector>


template<size_t size>
class Person {
public:
  std::bitset<size> genome;
  friend std::ostream& operator << (std::ostream& os, const Person& m)
  {
    os << m.genome;
    return os ;
  }
};


template<size_t size>
typename std::bitset<size> random_bitset( double p = 0.5) {

  typename std::bitset<size> bits;
  std::random_device rd;
  std::mt19937 gen( rd());
  std::bernoulli_distribution d(p);
  for(int n = 0; n < size; ++n){
    bits[n] = d(gen);
  }
  return bits;
}

template<size_t size>
std::vector<Person<size>> new_population(size_t sz, int seed)
{
  std::vector<Person<size>> pops(sz);
  for(size_t i = 0; i < sz; ++i){
    Person<size> p;
    p.genome = random_bitset<size>();
    pops[i] = p;
  }
  return pops;
}



int main()
{
  auto pop =  new_population<10>(30, -1);
  size_t i = 1;
  for(const Person<10>& p : pop){
    std::cout << i << ": " << p << std::endl;
    i++;
  }
  return 0;
}
