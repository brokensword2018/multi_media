#pragma once
#include <string>


#define DECLARE_SINGLETON(class_name) \
static class_name& instance();


#define IMPL_SINGLETON(class_name) \
class_name& class_name::instance() { \
    static class_name obj;\
    return obj; \
}
