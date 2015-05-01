/*
 * (C) Copyright 2015 Rodrigo Reyes https://github.com/reyesr
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: injection is a minimal dependency injection manager, in one-single-header
 *
 */
#ifndef INJECTIONMANAGER_H
#define INJECTIONMANAGER_H

#include <functional>
#include <memory>

namespace injection {

enum class InjectionScope {
    INVALID, TRANSIENT, SINGLETON, SINGLETON_EAGER
};

namespace injection_private {

enum class InjectionCommand {
    SET, GET
};

/**
 * The injection engine is where the instanciation really happens. It's really in the process() method, as it uses
 * static variable to hold the current injection configuration for a given type I.
 */
template<typename I> class InjectionEngine {
    static inline std::shared_ptr<I>& set(std::shared_ptr<I>& holder, std::function<I*(void)>& maker) {
        if (maker) {
            holder.reset(maker());
        }
        return holder;
    }

public:

    /**
     * @brief config get or set the injection configuration for type I (see template).
     * @param creationFunction the function used to create an instance of type I
     * @param cmd the command (basically GET or SET)
     * @param scope (only used for SET) the scope to apply on the injections of type I.
     * @return an instance of type I (for the GET command) or nothing that matters (for SET).
     */
    static std::shared_ptr<I> config(std::function<I*(void)> creationFunction, InjectionCommand cmd, InjectionScope scope = InjectionScope::INVALID) {

        static std::function<I*(void)> maker;
        static InjectionScope scopeType = InjectionScope::INVALID;
        static std::shared_ptr<I> shared;

        switch(cmd) {
            case InjectionCommand::SET:
                scopeType = scope;
                maker = creationFunction;
                if (scope == InjectionScope::SINGLETON_EAGER) {
                    set(shared, maker);
                } else {
                    shared.reset();
                }

                break;
            case InjectionCommand::GET:
                switch(scopeType) {
                    case InjectionScope::SINGLETON_EAGER:
                        return shared;
                    case InjectionScope::SINGLETON:
                        if (shared) {
                            return shared;
                        }
                    return set(shared, maker);

                case InjectionScope::TRANSIENT: {
                        std::shared_ptr<I> instance;
                        return set(instance, maker);
                    }
                default:
                        return std::shared_ptr<I>();
                }

        }
        return shared;
    }
};

/**
 * a utility method to instanciate a class using the zero-arg constructor
 */
template<typename C> C*simple_maker() {
    return new C();
}

}

/**
 * @brief The InjectionManager class manages the configuration for the injections.
 * It only _modifies_ (ie. create or update) the global configuration, to get an instance of a type, see Injected<T>
 */
struct InjectionManager
{
    template<typename I>
    static void bind(std::function<I*()> maker, InjectionScope scope) {
        static injection_private::InjectionEngine<I> phony;
        phony.config(maker, injection_private::InjectionCommand::SET, scope);
    }

    /**
     * Binds a type I with an instanciation function to be used for a singleton-scoped injection.
     */
    template<typename I> static void bind_singleton(std::function<I*()> maker) {
        InjectionManager::bind<I>(maker, InjectionScope::SINGLETON);
    }
    /**
     * Binds a type I with an instance of I, to be used for a singleton-scoped injection.
     * Once passed to this method, the instance of singleton is owned and managed by the injection engine
     */
    template<typename I> static void bind_singleton(I* singleton) {
        InjectionManager::bind<I>([&singleton](){return singleton;}, InjectionScope::SINGLETON_EAGER);
    }

    /**
     * Binds a type I with an default instanciation function to be used for a singleton-scoped injection.
     */
    template<typename I, typename C> static void bind_singleton() {
        bind<I>(injection_private::simple_maker<C>, InjectionScope::SINGLETON);
    }


    /**
     * Binds a type I with an instanciation function to be used for a transient-scoped injection (every injection gets a new instance).
     */
    template<typename I> static void bind_transient(std::function<I*()> maker) {
        InjectionManager::bind<I>(maker, InjectionScope::TRANSIENT);
    }
    /**
     * Binds a type I with an default instanciation function to be used for a transient-scoped injection (every injection gets a new instance).
     */
    template<typename I, typename C> static void bind_transient() {
        bind<I>(injection_private::simple_maker<C>, InjectionScope::TRANSIENT);
    }
};

/**
 * The Injected<T> class manages an injected type, and holds it for the duration of its scope.
 * Just declare a variable as Injected<MyBaseType> myvar, to have it available according to the
 * injection configuration set with the InjectionManager.
 */
template<typename I> class Injected
{
    std::shared_ptr<I> instance;
public:
    inline I* operator*() {
        return operator->();
    }

    I* operator->() {
        if (!instance) {
            instance = injection_private::InjectionEngine<I>::config(nullptr, injection_private::InjectionCommand::GET);
        }
        return instance.get();
    }

    operator bool() const {
        return instance;
    }
};

}

#endif // INJECTIONMANAGER_H
