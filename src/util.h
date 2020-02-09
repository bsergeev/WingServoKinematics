#pragma once

// Typed double number
#define DECLARE_Value(NAME) struct NAME##Value final {     \
  explicit constexpr NAME##Value(double v=0):m_value(v) {} \
  constexpr operator double() const { return m_value; }    \
private:                                                   \
  double m_value;                                          \
}
DECLARE_Value(Radian); // => RadianValue

