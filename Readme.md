# Injection

A minimalist dependency injection system in one-single-header and under 100 lines of C++ code.

It supports singleton and transient values. No thread-local or anything, but it's on purpose to keep it small.

Only tiny drawback: your compiler must support a basic subset of c+11 (lambda, shared_ptr, enum class), but check the date: we're talking c++-11 like the year 2011.

## Rationale

Adding a full blown framework in a c++ project is always a painful mess, and most project have simple needs. This is what happened here, so here is a simple yet fully functional dependency injection system all in one header.

Just move the include/injectionmanager.h file in your project, and you're ready to go.

## How to use it

Somewhere early in your program startup sequence, add the configuration for the injection.

For instance:

    using namespace injection;
    InjectationManager::bind_singleton<MyInterface, MyImplementation>();

Then, anywhere in your program:

    Injected<MyInterface> value;
    value->someMethod();

Once the configuration is done, everytime you use the Injected<BaseClass> type, an instance of a class based on BaseClass is going to be injected. As the binding specified is bind_singleton<I,C>(), only one single instance of C is created, and every Injected<I> automatically receives this C instance.

## Namespace

Everything is in the `injection` namespace. add `using namespace injection;` if required.

## Scope Types

There are 2 kinds of scopes

- Transient: every Injected<*> receives a new instance.
- Singleton: every Injected<*> receives the same instance.

## Injection configuration

The configuration is done using the bind_* methods available in the InjectionManager class.

### Binding singleton instances

    InjectionManager::bind_singleton<I>(std::function<I*()> maker);
    InjectionManager::bind_singleton<I>(I* i);
    InjectionManager::bind_singleton<I, C>();

Those three flavours of singleton binding can be used to configure the injection for a unique instance of a base class injected everywhere.

Example:

Just specify the implementation class, and the injection engine instanciate it using the empty constructor.

    InjectionManager::bind_singleton<I, Test>();

Or pass a new instance either directly or using a lambda/function.

    InjectionManager::bind_singleton<I>(new Test(2, 0.22));
    InjectionManager::bind_singleton<ITest>([](){
        return new Test(3, 0.33);
    });


Note that bindings can be overwritten:

    InjectionManager::bind_singleton<ITest, Test1>();
    Injected<ITest> foo;
    InjectionManager::bind_singleton<ITest, Test2>();
    Injected<ITest> bar;

foo and bar respecively get an instance of Test1 and an instance of Test2.

### Binding Transient instances

This is basically the same than singleton's, except you obviously can't pass directly an instance.

    InjectionManager::bind_transient<BaseClass,Implementation>();
    InjectionManager::bind_transient<BaseClass>([](){
        return new Implementation();
    });

### Receiving the injected instances

The `Injected<BaseClass>` automatically receives an instance of `BaseClass` as defined in the configuration. This has the same semantic than `std::shared_ptr` (actually, it's mainly a thin wrapper around a shared_ptr).

If there is no configuration for it, it just points to null (same semantic than shared_ptr). You can test if an instance has been rightfully injected using the implicit bool cast:

    Injected<Something> foo;
    if (!foo) { ... } // foo was not injected !

Using the injected instance is just a `->` away:

    Injected<Something> foo;
    foo->myMethod();


## Running the unit tests

There's a cmake project ready to run:

    cmake . && make && ./injection

## What if something is missing

Just fork it, or modify it in-place in your project, it's really just a small piece of code.

## License

(C) Copyright 2015 Rodrigo Reyes https://github.com/reyesr

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
