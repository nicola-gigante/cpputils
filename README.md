cpputils
========

This is a little collection of various C++ utilities that I frequently use and I 
don't want to duplicate in every project anymore. It is composed only by 
header files that can be included as is, and compiled both in C++11 and C++14.

I've tested the code mainly in clang, but it should work also on recent versions
of g++. Please let me know if this is not true.

## has_member.h
This header declares a utility trait to check if a class possess a member 
function that can be called with a given signature. Actually, the header 
declares a macro that generate the trait for each member name that one wants 
to check.

The macro is called ```DECLARE_HAS_MEMBER_TRAIT``` and given a name, 
e.g. ```get```, it generates a trait class called, for instance 
 ```has_member_get``` . 

The trait wants a signature as the template parameter and provides a static 
boolean constant ```value``` which will be true if a member with the given 
name exists and is callable with the given signature.

The macro also declares two tag ```struct```s called 
 ```has_member_<name>_tag``` and ```doesnt_have_<name>_tag```.
The trait provides a typedef ```type``` which can be one of the two tag 
structs, useful for calling different implementation of a function with tag 
dispatching.

## meta.h

This header provides a few functions that are useful when writing function 
templates need to be enabled only when complex conditions are met.

### REQUIRES

The ```REQUIRES``` macro encapsulates the common idiom of using 
 ```std::enable_if```, but providing extra convenience. 
It is used as follows:
```cpp
template<typename T, REQUIRES(std::is_integral<T>())>
T sum(T v1, T v2) { return v1 + v2; }
```

The macro accepts a list of requirements that have to be fulfilled
all at the same time to enable the instantiation of the function template.

The first thing you can notice is that I've not used the ```::value```
member of the type trait, but I've instead instantiated the type and passed 
the instance to the macro. This is because the ```REQUIRES``` macro is
built to accept directly instances of types that have a ```value``` member,
to avoid the need to manually type it.

It also accepts directly booleans, so you can mix different types of
requirements. The fact that it accepts a _list_ of requirements, instead
of a single one, makes very easy to check the same property on all the
elements of a variadic pack.

Here follows an example that shows a combination of the available options:
```cpp
template<typename ...Args,
         REQUIRES(sizeof...(Args) % 2 == 0, std::is_integral<Args>()...)>
void func(Args ...args) { ... }
```
Here, the function is enabled only if the number of arguments supplied is even,
and if their types are all integral types. You see that applying a condition
on all the elements of a pack is as simple as adding an ellipsis.

Another commonly useful example of this kind is the implementation
of a function that takes a variable number of arguments, but of fixed type.
In C++ there is currently no way to directly constraint the type of
the elements of a variadic pack, so you have to rely on SFINAE, but
the amount of code required to implement this seemingly simple task is
ridiculous.
Here is how easily it can be done with this facility. 
Suppose we want a function that concatenates an arbitrary number of strings:
```cpp
template<typename ...String,
         REQUIRES(std::is_same<String, std::string>()...)>
std::string concatenate(String s, String ...args) {
   return s + concatenate(args...);
}
```
(yes, they should be passed by forwarding reference, but who cares now)

Multiple macro invocations can be used in the same template, as long as they're
put on different lines. This is because the macro internally uses the 
 ```__LINE__``` preprocessor symbol, to be able to implement the following
feature. 

When using SFINAE to disable some function overload, the condition of 
```enable_if```, in order to work, has to depend on one of the template
parameters of the function itself. Otherwise, the condition expression is
expanded at the first phase of the template parsing (rather than before
the instantiation) and the substitution failure results in an error.
The ```REQUIRES``` macro, on the other hand, can always be used with
non type-dependent conditions as well, as shown in the following example:
```cpp
template<typename T>
class example {
public:
   /*
    * This doesn't work. It triggers an hard error if T is not integral,
    * because the condition is not type-dependent
    */
   template<typename U, 
            typename std::enable_if<std::is_integral<T>::value>::type>
   U member(U u) { return u; }

   /*
    * This works as expected, even if the condition doesn't depend on the
    * template parameter
    */
   template<typename U, REQUIRES(std::is_integral<T>())>
   U member(U u) { return u; }   
};
```

The header provides also a couple of functions, useful to compose complex SFINAE conditions. These
are ```all()``` and ```any()```, that accept a variable number of constraints (again, both booleans
or instances of type traits), and return true respectively if all are fullfilled (which is implicit
in the ```REQUIRES``` macro anyway), or if at least one of them is fulfulled.

The last element is the ```neg()``` function, which negates a constraint condition. This is useful
to negate type traits, since you can't type, e.g., ```!std::is_integral<T>()```, because the type
doesn't provide an overloaded not operator, but you can then type ```neg(std::is_integral<T>())```.

### Other utilities 

* The ```void_t``` type trait is very simply a template typedef that always 
  corresponds to ```void```, no matter what you pass as the argument. The
  usefulness of this little utility in a lot of template metaprogramming
  techniques is well known. See for example, 
  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3911.pdf

* The ```true_t``` type trait is similar to ```void_t```, but is a value
  type trait that always return ```true```, no matter what.

## invoke.h
This header provides two utilities that are useful when dealign with Callable
objects, as defined by the standard.

The ```invoke()``` function takes a callable object and any number of arguments
and invoke it with them, following the definition of the Callable concept from
the C++ Standard. This means you can call functions, function objects, 
pointers to member functions, and pointers to data members, with a uniform
syntax. Moreover, you can invoke pointers to data members and pointers to 
member functions using a reference, a pointer or any smart pointer as the instance.

The ```invokable()``` function builds on ```invoke()``` and wraps any Callable
into a function object, which is useful as an alternative to ```std::bind``` to
conveniently use pointers to data members or pointers to member functions.

A little example:
```cpp
struct S {
    int member = 42;
};

auto f = invokable(&S::member);

S s;
std::cout << f(s) << std::endl; // Prints '42'

```

## std14 namespace

The headers in the ```std14/``` subdirectory provide a replacement for some of 
the missing pieces of the C++11 standard library that have been added in the 
recently published C++14 standard. Of all the things, I've added mostly only those 
things that could have been directly added without modifying existing entities
(for example, there's not an entirely rewritten ```std::tuple``` library only
to add ```constexpr``` member functions, but there is a ```constexpr``` enabled
```std::array```). Of all of them, I've selected what I think are the must-haves.

Each header under ```std14/``` subdirectory is one-to-one with a standard 
header, and includes the standard one. All the declarations are enclosed in a 
namespace called ```std14```, because it's forbidden to add things to 
namespace ```std```. For convenience, the entire ```std``` namespace is 
imported inside it. However, when you compile your project in C++14 mode 
(because in the future you'll want to), all the declarations disappear and 
namespace ```std14``` becomes an alias for ```std```, so you can use the 
library-provided symbols out-of-the-box.

Provided entities are:
- ```std::make_unique``` in the ```<std14/memory>``` header
- ```constexpr``` enabled ```std::array``` in ```<std14/array>```
- Type traits aliases (e.g. ```enable_if_t<>``` instead of ```enable_if<>```)
  and SFINAE-friendly ```std::result_of``` in ```<std14/type_traits>```
- ```std::integer_sequence``` and related facilities in the ```<std14/utility>```
  header.
- ```std::experimental::string_view``` in the ```<std14/experimental/string_view>```
  header.
  It's a fairly accurate implementation of string_view from the Library
  Foundamentals TS, even if I've not tested anything for compliance with the
  actual specification (and I've left out some boring bits). 
- ```std::experimental::array_view```, in ```<std14/experimental/array_view>```
  is a simplistic analogue of ```llvm::ArrayRef```, or of the upcoming
  ```array_view``` standard proposal, but only a simple unidimensional view,
  without all the fancy multidimensional stuff.
