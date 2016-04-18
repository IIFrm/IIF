#!/usr/bin/python
from z3 import *
import sys
#def toSMT2Benchmark(f, status="unknown", name="benchmark", logic=""): 
#    v = (Ast * 0)() 
#    return Z3_benchmark_to_smtlib_string(f.ctx_ref(), name, logic, status, "", 0, v, f.as_ast())

#x = Real('x')
#y = Real('y')
#ctx = Context()
#s = Solver(ctx=ctx)
#f = parse_smt2_file("2.smt2", ctx=ctx)
if len(sys.argv) < 2:
    print 'Need 1 more paramater to specifiy the smt2 file'
    os._exit(1)
else:
    s = Solver()
    s.add(parse_smt2_file(sys.argv[1]))
    #print s.to_smt2() 
    if s.check() == sat:
	print 'sat'
	print(s.model())
        sys.exit(0)
    else:
	print 'unsat'
        sys.exit(1)
