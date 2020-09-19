vtable-hook
===========

C++ Vtable Hooking Library. Only requires a header.
Supports Windows and Linux.
Validated in Linux, X86, g++

### Example ###
```C++

#include "vtablehook.h"
#include <stdio.h>

class Object {
        public:
        int member;

        Object(int init) {
           member = init;
        }

        virtual int Foo(int size) {
           int ret = 0;
           if (size <= 2048) {
             printf("Orig: input size:%d\n", size);
           } else ret = -1;
           return ret;
        }
};

typedef int (*FUNC_PTR)(void*, int size);
void *orig_func_p = NULL;

#ifdef WIN32
void __fastcall FooHook(Object* that) {
#elif __linux__
int FooHook(Object* that, int size) {
#endif
  int ret = 0;
  if (orig_func_p) {
    ret = (reinterpret_cast<FUNC_PTR>(orig_func_p))(that, size); 
  }
  printf("---- Hooked orig.size:%i orig.ret:%d\n", size, ret);
}

int main() {
  Object* o = new Object(123);

  orig_func_p = vtablehook_hook(o, (void*)FooHook, 0);
  o->Foo(2048);
  o->Foo(4096);
  o->Foo(2048);

  vtablehook_restore(o, (void*)orig_func_p, 0);
  o->Foo(2048);
  o->Foo(4096);
  o->Foo(2048);

  return 0;
}

```
