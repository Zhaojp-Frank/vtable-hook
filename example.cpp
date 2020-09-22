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
     //printf("Base init\n");
     member = init;
   }

   virtual ~Object() {printf(" base out\n");}

   virtual int Foo(int size) {
     int ret = 0;
     if (size <= 2048) {
       printf("Base-foo1: input size:%d\n", size);
      } else ret = -1;
     return ret;
   }

   virtual int Foo(int size, int alignment) {
     int ret = 0;
     if (size <= 2048) {
       printf("Base-foo2: input size:%d\n", size);
      } else ret = -1;
     return ret;
    }
};

class Child : public Object {
 public:
   Child (int init): Object(init){
     //printf("Child init\n");
   }

   virtual ~Child () {printf("child out\n");}

   virtual int Foo(int size, int alignment) {
     int ret = 0;
     if (size <= 2048) {
       printf("Orig-child-Foo2: input size:%d\n", size);
      } else ret = -1;
     return ret;
    }
   virtual int Foo(int size) {
     int ret = 0;
     if (size <= 2048) {
       printf("Orig-child-Foo1: input size:%d\n", size);
      } else ret = -1;
     return ret;
    }
};

class Kid : public Child {
 public:
   Kid (int init): Child(init){
     //printf("Kid init\n");
   }

   virtual int Foo(int size) {
     int ret = 0;
     if (size <= 2048) {
       printf("Orig-Kid-Foo1: input size:%d\n", size);
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
  Object* o = new Kid(123);
  vtable_print(o, 0);
  vtable_print(o, 1);
  vtable_print(o, 2);
  vtable_print(o, 3);
  vtable_print(o, 4);

  // ok:
  int (Object::*mfp1)(int) = &Object::Foo;
  int (Object::*mfp2)(int, int) = &Object::Foo;
  printf("kid.foo1:%p foo2:%p\n", (void *)(o->*mfp1), (void *)(o->*mfp2));

  // ok 
  printf("\ncall o->Foo()\n");
  o->Foo(1024); 
  printf("call o->*mfp1()\n");
  (o->*mfp1)(1024);

  // ok 
  Object * oo = new Child(123);
  vtable_print(oo, 0);
  vtable_print(oo, 1);
  vtable_print(oo, 2);
  vtable_print(oo, 3);
  vtable_print(oo, 4);

  int (Object::*mfp3)(int) = &Object::Foo;
  int (Object::*mfp4)(int, int) = &Object::Foo;
  printf("child.foo1:%p foo2:%p\n", (void *)(oo->*mfp3), (void *)(oo->*mfp4));

  printf("\n-------to hook\n");
  orig_func_p = vtablehook_hook(o, (void*)FooHook, 2);
  o->Foo(2048);
  o->Foo(4096);
  o->Foo(2048);

  printf("\n-------to restore\n");
  vtablehook_restore(o, (void*)orig_func_p, 2);
  o->Foo(2048);
  o->Foo(4096);
  o->Foo(2048);

  return 0;
}
