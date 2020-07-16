# LLVM for [LLVMSQLite](https://github.com/KowalskiThomas/LLVMSQLite)

This repository is a modified version of the LLVM project repository as it was on 2020/04/21. 

It includes the following changes:

* Some `asserts` have been made clearer to help when debugging IR generation (only if `NDEBUG` is not defined);
*  Some MLIR operations have been added to the LLVMIR dialect:
   * Memory intrinsics (`memcmp`, `memcpy`, `memset`, `memcpyinline`) ;
   * `stackSave` and `stackRestore` 
   * `call` can now call values (and not only `LLVMFuncOp`'s)

## Getting started (for LLVMSQLite)

To get set for LLVMSQLite, you'll need to compile and install MLIR and Clang:

```
mkdir ~/llvm/
mkdir build && cd build
export CC=clang
export CXX=clang++
cmake -G Ninja ../llvm -DLLVM_ENABLE_PROJECTS="mlir;clang" -DLLVM_TARGETS_TO_BUILD="X86" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLLVM_ENABLE_ASSERTIONS=ON
DESTDIR=~/llvm ninja install
```
