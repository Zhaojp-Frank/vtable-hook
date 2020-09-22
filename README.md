vtable-hook
===========

C++ Vtable Hooking Library. Only requires a header.

Validated in Linux, X86, g++

### Example ###
```bash

# one can use GCC extension to query member func addr
# then lookup vtable to confirm the offset
# Here're the sample's output 

offset:0 orig_func.addr:401140
offset:1 orig_func.addr:40117a
offset:2 orig_func.addr:4010fe
offset:3 orig_func.addr:40104a
offset:4 orig_func.addr:0
kid.foo1:0x4010fe foo2:0x40104a

call o->Foo()
Orig-Kid-Foo1: input size:1024
call o->*mfp1()
Orig-Kid-Foo1: input size:1024

offset:0 orig_func.addr:400fbe
offset:1 orig_func.addr:401024
offset:2 orig_func.addr:40108e
offset:3 orig_func.addr:40104a
offset:4 orig_func.addr:0
child.foo1:0x40108e foo2:0x40104a

-------to hook
offset:2 orig_func.addr:4010fe
Orig-Kid-Foo1: input size:2048
---- Hooked orig.size:2048 orig.ret:0
---- Hooked orig.size:4096 orig.ret:-1
Orig-Kid-Foo1: input size:2048
---- Hooked orig.size:2048 orig.ret:0

-------to restore
Orig-Kid-Foo1: input size:2048
Orig-Kid-Foo1: input size:2048

```
