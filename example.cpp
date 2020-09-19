#include "vtablehook.h"
#include <stdio.h>
// you can get the VTable location either by dereferencing the
// // first pointer in the object or by analyzing the compiled binary.
// unsigned long VTableLocation = 0U;
// // then you have to figure out which slot the function is in. this is easy
// // since they're in the same order as they are declared in the class definition.
// // just make sure to update the index if 1) the function declarations are
// // re-ordered and/or 2) virtual methods are added/removed from any base type.
// unsigned VTableOffset = 0U;
// typedef void (__thiscall Base::*FunctionType)(const Base*);
// FunctionType* vtable = reinterpret_cast<FunctionType*>(VTableLocation);
//
// bool hooked = false;
// HANDLE process = ::GetCurrentProcess();
// DWORD protection = PAGE_READWRITE;
// DWORD oldProtection;
// if ( ::VirtualProtectEx( process, &vtable[VTableOffset], sizeof(int), protection, &oldProtection ) )
// {
//     vtable[VTableOffset] = static_cast<FunctionType>(&ReplacementMethod);
//
//     if ( ::VirtualProtectEx( process, &vtable[VTableOffset], sizeof(int), oldProtection, &oldProtection ) )
//       hooked = true;
// }
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
