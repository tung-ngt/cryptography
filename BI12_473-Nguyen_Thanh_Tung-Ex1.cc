#include <iostream>
#include <bitset>
#include <algorithm>

// Macro for debugging log
#define DEBUG

#ifdef DEBUG
  #define LOG(msg) { std::cout << msg << '\n'; }
#else
  #define LOG(msg) { }
#endif

// Base class to represent a register in A5/1 algorithm
// Note that in this class the 0th index start on the left
// while the underlying bitset 0th index start from the right
template <size_t RegisterSize>
class Register
{
public:
  Register(int state)
  { m_state = std::bitset<RegisterSize>(state); }

  size_t size()
  { return RegisterSize; }

  void set(size_t index, bool bit)
  { m_state[RegisterSize - 1 - index] = bit; }

  bool at(size_t index)
  { return m_state.test(RegisterSize - 1 - index); }

  bool peek()
  { return at(RegisterSize - 1); }

  void show()
  { std::cout << "(0) " << m_state << " (" << (RegisterSize - 1) << ')' << '\n'; }

  void updateState()
  {}

protected:
  void shift(bool bit)
  {
    m_state >>= 1;
    set(0, bit);
  }

private:
  std::bitset<RegisterSize> m_state;
};


class RegisterX : public Register<19>
{
public:
  RegisterX(int state): Register(state)
  {}

  void updateState()
  {
    bool newBit = at(18) != at(17) != at(16) != at(13);
    shift(newBit);
   
    LOG("X[0] = X[18] ^ X[17] ^ X[16] ^ X[13] = " 
      << at(18) << " ^ " << at(17) << " ^ " << at(16) << " ^ " << at(13) << " = " << newBit);
  }
};

class RegisterY : public Register<22>
{
public:
  RegisterY(int state): Register(state)
  {}

  void updateState()
  {
    bool newBit = at(21) != at(20);
    shift(newBit);
   
    LOG("Y[0] = Y[21] ^ Y[20] = " << at(21) << " ^ " << at(20) << " = " << newBit);
  }
};

class RegisterZ : public Register<23>
{
public:
  RegisterZ(int state): Register(state)
  {}

  void updateState()
  {
    bool newBit = at(22) != at(21) != at(20) != at(7);
    shift(newBit);
 
    LOG("Z[0] = Z[22] ^ Z[21] ^ Z[20] ^ Z[7] = " 
      << at(22) << " ^ " << at(21) << " ^ " << at(20) << " ^ " << at(7) << " = " << newBit);
  }
};

class KeyStreamGenerator
{
public:
  KeyStreamGenerator(RegisterX X, RegisterY Y, RegisterZ Z) : m_X(X), m_Y(Y), m_Z(Z)
  {}

  RegisterX getX()
  { return m_X; }

  RegisterY getY()
  { return m_Y; }
  
  RegisterZ getZ()
  { return m_Z; }

  template <size_t StreamLength>
  std::bitset<StreamLength> getKeyStream()
  {
    std::bitset<StreamLength> keyStream(0b0);

    for (size_t i = 0; i < StreamLength; i++)
    {
      LOG("\n-----------------------------\n"
        << "ITERATION " << i);

      bool key = m_X.peek() != m_Y.peek() != m_Z.peek();
      keyStream.set(i, key);

      LOG("key[" << i << "] = X[18] ^ Y[22] ^ Z[23] = "
        << m_X.peek() << " ^ " << m_Y.peek() << " ^ " << m_Z.peek() << " = " << key);
      
      bool X_8 = m_X.at(8);
      bool Y_10 = m_Y.at(10);
      bool Z_10 = m_Z.at(10);

      bool majority = (X_8 & Y_10) | (Y_10 & Z_10) | (Z_10 & X_8);

      LOG("majority(X[8], Y[10], Z[10]) = majority(" << X_8 << ", " << Y_10 << ", " << Z_10 << ") = " << majority);

      if (X_8 == majority) m_X.updateState();
      if (Y_10 == majority) m_Y.updateState();
      if (Z_10 == majority) m_Z.updateState();

      #ifdef DEBUG
        LOG("X")
        m_X.show();
        LOG("Y")
        m_Y.show();
        LOG("Z")
        m_Z.show();
      #endif

    }

    return keyStream;
  }

private:
  RegisterX m_X;
  RegisterY m_Y;
  RegisterZ m_Z;
};

int main()
{
  // Initialize registers, 0th index on the left
  RegisterX X(0b1111101010101010101);
  RegisterY Y(0b1100110011001100110011);
  RegisterZ Z(0b11100001111000011110000);
  
  std::cout << "Inital registers:\n";
  std::cout << "X\n";
  X.show();
  std::cout << "Y\n";
  Y.show();
  std::cout << "Z\n";
  Z.show();

  // Create generator
  KeyStreamGenerator generater(X, Y, Z);

  // Generate key stream of length 10
  std::bitset<10> keyStream = generater.getKeyStream<10>();

  // Reverse to print the 0th index on the left
  std::string bitRep = keyStream.to_string();
  std::reverse(bitRep.begin(), bitRep.end());
  std::cout << "\nkey stream:\n";
  std::cout << "(0) " << bitRep << " (9)";

  return 0;
}