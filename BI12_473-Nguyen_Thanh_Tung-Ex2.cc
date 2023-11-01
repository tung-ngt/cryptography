#include <iostream>
#include <vector>
#include <algorithm>

// Macro for debugging log
#define DEBUG

#ifdef DEBUG
  #define LOG(msg) {std::cout << msg << '\n';}
#else
  #define LOG(msg) {}
#endif

std::vector<bool> convert(int x)
{
  std::vector<bool> ret;

  while (x)
  {
    if (x & 1)
      ret.push_back(1);
    else
      ret.push_back(0);
    x >>= 1;
  }

  return ret;
}

int fastModExp(unsigned int x, unsigned int exp, unsigned int modular)
{
  std::vector<bool> bitRep = convert(exp);

  LOG("Exponential bit representation");

  #ifdef DEBUG
    for (int i = bitRep.size() - 1; i >= 0; i--)
    { std::cout << bitRep[i]; }
  #endif

  LOG("");

  int r = x % modular;
  for (int i = bitRep.size() - 2; i >= 0; i--)
  {
    LOG("Remainder: " << r)
    LOG("exponential bit e[" << i << "] = " << bitRep[i])
    r = r * r % modular;
    if (bitRep[i] == 1)
      r = r * x % modular;
  }
  return r;
}

int main(int argc, char * argv[])
{
  int x = 856;
  int exp = 25;
  int modular = 7;

  if (argc == 4)
  {
    x = atoi(argv[1]);
    exp = atoi(argv[2]);
    modular = atoi(argv[3]);
  }

  int r = fastModExp(x, exp, modular);
  std::cout << "final remainder: " << r << "\n";
  return 0;
}

// 856^25 mod 7
// 25 = 11001 base 2
//
// #0  856 mod 7 = 2            h4 = 1
//
// #1a 856^2 mod 7 = 4          h3 = 1
// #1b 856^3 mod 7 = 1
//
// #2  856^6 mod 7 = 1          h2 = 0
//
// #3  856^12 mod 7 = 1         h1 = 0
//
// #4a 856^24 mod 7 = 1         h0 = 1
// #4b 856^25 mod 7 = 2
