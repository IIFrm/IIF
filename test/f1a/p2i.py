from z3 import *

x = Int('x')
y = Int('y')
s = Solver()
s.add(x + y >= 0)
s.add(x + y > 0)
print(s.check())
print(s.model())
