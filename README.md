cpputils
========

This little collection of various C++ utilities that I frequently use and I don't want to 
duplicate in every project anymore. It is composed by self-contained header files 
that can be used stand-alone.

## has_member.h
This header declares a utility trait to check if a class possess a member function 
that can be called with a given signature. Actually, the header declares a macro 
that generate the trait for each member name that one wants to check.

The macro is called ```DECLARE_HAS_MEMBER_TRAIT``` and given a name, 
e.g. ```get```, it generates a trait class called, for instance ```has_member_get```. 

The trait wants a signature as the template parameter and provides a static 
boolean constant ```value``` which will be true if a member with the given name 
exists and is callable with the given signature.

The macro also declares two tag ```struct```s called 
```has_member_<name>_tag``` and ```doesnt_have_<name>_tag```.
The trait provides a typedef ```type``` which can be one of the two tag ```struct```s,
useful for calling different implementation of a function with tag dispatching.


## static_table.h

This header declares a couple of utility functions to help the creation of instances 
of ```std::array``` at compile time. Given the limitations of C++11 ```constexpr``` 
functions and the lack of ```constexpr``` member functions in the C++11 
```std::array```, the generation of compile-time tables for precomputing values of 
any sort is much more difficult than how could be. These little utilities can help by 
providing a mean to generate tables with progressive indexes and transforming 
them in a functional way to create the desired table.

Provided functions are:
- ```irange<T, Begin, End>()``` returns a sequence filled of values of type ```T``` in 
the range ```[Begin, End)```.
- ```iota<T, N>()``` returns a sequence filled of values of type ```T``` in the range 
```[0, End)```.
- ```map(<function>, <sequence>)``` maps the function over the sequence, 
returning the mapped sequence.
- ```foldr(<function>, <zero>, <sequence>)``` and 
```foldl(<function>, <zero>, <sequence>)``` work exactly like the corresponding 
functions in any functional language. They accumulate from the right (or the left) 
the results of applying a binary function to the elements of the sequence, 
returning the accumulated result.

The result of ```irange()```, ```iota()```, and ```map()``` can be converted to a 
```std::array```, thus allowing to obtain the final compile-time computed table.
