#include "stdlib.h"

int fact(int n)
{
  if (n <= 1)
    return 1;
  else
    return n * fact(n - 1);
}

int main(void)
{
  int a = 0;
  int i = 6;
  a = fact(i);
  print_string("fact(");
  print_int(i);
  print_string(") = ");
  print_int(a);
  print_string("\n");

  return 0;
}
