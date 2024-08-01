# vf header-only C/C++ libraries
Small (hopefully portable) header-only libraries that can be useful for tiny C/C++ projects. All libraries follow the [stb conventions](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt). The aim of these libraries are not necessarily performance but rather convenience. **Currently under development. Don't use in production! (Though, they are being tested in production)**

## Current implementations
| Name        | Version | Description                           |
| ----------- | ------- | ------------------------------------- |
| [vf_binaryheap.h](/vf_binaryheap.h) | 0.10 | Container library for fixed size flexible binary heap. |
| [vf_darray.h](/vf_darray.h) | 0.21 | Container library for dynamic array. |
| [vf_hashmap.h](/vf_hashmap.h) | 0.20 | Hashmap library using 64-bit FNV-1a hash and open addressing with linear probing for collision resolution. |
| [vf_log.h](/vf_log.h) | 0.11 | Library for small logging needs. |
| [vf_memory.h](/vf_memory.h) | 0.21 | Recreation of some of the standard library memory functions, like `memcpy`, `memset`, etc... |
| [vf_queue.h](/vf_queue.h) | 0.30 | Container library for circular queue. |
| [vf_sparseset.h](/vf_sparseset.h) | 0.10 | Container library for sparse set. Can be used for sparse-set ECS component pools. |
| [vf_test.h](/vf_test.h) | 0.10 | Tiny unit test library for C/C++. |
