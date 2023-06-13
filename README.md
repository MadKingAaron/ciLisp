# CILisp
CILisp is a Lisp-spinoff [interpreter](https://en.wikipedia.org/wiki/Interpreter_(computing)) written in C using Flex and Bison.

### Example Programs
```
(add ((let (abcd 1)) (sub 3 abcd)) 4)
```
```
(add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))
```
```
((let (integer a (read)) (real b (read))) (print a b))
```

### Supported Operations
  - negation
  - absolute value
  - exponential
  - square root
  - addition
  - subtract
  - multiplication
  - division
  - remainder
  - log
  - power
  - max
  - min
  - exp2 (base-2 exponential)
  - cube root
  - hypotenuse
  - print (output variable)
  - read (input from user)
  - equal ( == )
  - smaller ( < )
  - larger ( > )
  
### Features
CILisp supports user-defined functions as well as recursion. On top of mathematical operations, 
there is support for input/output, conditionals, and variables. Op lists allow for multiple inputs into functions that can be run on multiple inputs. The two supported data types are reals and integers. CILisp employs static scoping, in which it will search for a variable in the symbol table in the current scope first and then each previous parent scope until it finds the variable in question.

### What CILisp Does
CILisp performs both syntactic and semantic analysis to produce an intermediate form of the compiled code. This intermediate
form is made up of various types of nodes that carry different pieces of information. CILisp uses a context-free grammar,
meaning that all transition rules are available at any point.
