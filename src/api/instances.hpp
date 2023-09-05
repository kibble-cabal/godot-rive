#ifndef _RIVEEXTENSION_API_INSTANCES_HPP_
#define _RIVEEXTENSION_API_INSTANCES_HPP_

// stdlib
#include <map>

// godot-cpp
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/typed_array.hpp>

// extension
#include "utils/memory.hpp"
#include "utils/types.hpp"

using namespace godot;
using namespace std;

template <class Instance>
struct Instances {
   private:
    map<int, Ref<Instance>> instances;
    Fn<Ref<Instance>, const int> instantiate;

   public:
    Instances(Fn<Ref<Instance>, const int> instantiate_fn) {
        instantiate = instantiate_fn;
    }

    Ref<Instance> get(const int index) {
        if (instances.count(index)) return instances.at(index);
        else if (index >= 0) {
            auto inst = instantiate(index);
            instances.insert(pair<int, Ref<Instance>>(index, inst));
            return inst;
        } else return nullptr;
    }

    Ref<Instance> reinstantiate(const int index) {
        if (instances.count(index)) unref(instances.at(index));
        auto inst = instantiate(index);
        instances.insert(pair<int, Ref<Instance>>(index, inst));
        return inst;
    }

    Ref<Instance> find(const Fn<bool, const Ref<Instance>, const int> check) const {
        for (auto const& [index, instance] : instances) {
            if (instance.is_valid() && check(instance, index)) return instance;
        }
        return nullptr;
    }

    void for_each(Fn<void, Ref<Instance>, const int> fn) {
        for (auto& [index, instance] : instances)
            if (instance.is_valid()) fn(instance, index);
    }

    void for_each(Fn<void, const Ref<Instance>, const int> fn) const {
        for (auto const& [index, instance] : instances) {
            if (instance.is_valid()) fn(instance, index);
        }
    }

    TypedArray<Instance> get_list() const {
        TypedArray<Instance> list;
        for (auto const& [index, instance] : instances) {
            list.push_back(instance);
        }
        return list;
    }

    size_t get_size() const {
        return instances.size();
    }
};

#endif