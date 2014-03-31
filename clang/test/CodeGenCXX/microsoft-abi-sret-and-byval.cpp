// RUN: %clang_cc1 -emit-llvm %s -o - -triple=i386-pc-linux | FileCheck -check-prefix LINUX %s
// RUN: %clang_cc1 -emit-llvm %s -o - -triple=i386-pc-win32 -mconstructor-aliases -fno-rtti | FileCheck -check-prefix WIN32 %s
// RUN: %clang_cc1 -emit-llvm %s -o - -triple=x86_64-pc-win32 -mconstructor-aliases -fno-rtti | FileCheck -check-prefix WIN64 %s

struct Empty {};

struct EmptyWithCtor {
  EmptyWithCtor() {}
};

struct Small {
  int x;
};

// This is a C++11 trivial and standard-layout struct but not a C++03 POD.
struct SmallCpp11NotCpp03Pod : Empty {
  int x;
};

struct SmallWithCtor {
  SmallWithCtor() {}
  int x;
};

struct SmallWithDtor {
  SmallWithDtor();
  ~SmallWithDtor();
  int x;
};

struct SmallWithVftable {
  int x;
  virtual void foo();
};

struct Medium {
  int x, y;
};

struct MediumWithCopyCtor {
  MediumWithCopyCtor();
  MediumWithCopyCtor(const struct MediumWithCopyCtor &);
  int x, y;
};

struct Big {
  int a, b, c, d, e, f;
};

// WIN32: declare void @"{{.*take_bools_and_chars.*}}"
// WIN32:       (<{ i8, [3 x i8], i8, [3 x i8], %struct.SmallWithDtor,
// WIN32:           i8, [3 x i8], i8, [3 x i8], i32, i8 }>* inalloca)
void take_bools_and_chars(char a, char b, SmallWithDtor c, char d, bool e, int f, bool g);
void call_bools_and_chars() {
  take_bools_and_chars('A', 'B', SmallWithDtor(), 'D', true, 13, false);
}

// Returning structs that fit into a register.
Small small_return() { return Small(); }
// LINUX-LABEL: define void @_Z12small_returnv(%struct.Small* noalias sret %agg.result)
// WIN32: define i32 @"\01?small_return@@YA?AUSmall@@XZ"()
// WIN64: define i32 @"\01?small_return@@YA?AUSmall@@XZ"()

Medium medium_return() { return Medium(); }
// LINUX-LABEL: define void @_Z13medium_returnv(%struct.Medium* noalias sret %agg.result)
// WIN32: define i64 @"\01?medium_return@@YA?AUMedium@@XZ"()
// WIN64: define i64 @"\01?medium_return@@YA?AUMedium@@XZ"()

// Returning structs that fit into a register but are not POD.
SmallCpp11NotCpp03Pod small_non_pod_return() { return SmallCpp11NotCpp03Pod(); }
// LINUX-LABEL: define void @_Z20small_non_pod_returnv(%struct.SmallCpp11NotCpp03Pod* noalias sret %agg.result)
// WIN32: define void @"\01?small_non_pod_return@@YA?AUSmallCpp11NotCpp03Pod@@XZ"(%struct.SmallCpp11NotCpp03Pod* noalias sret %agg.result)
// WIN64: define void @"\01?small_non_pod_return@@YA?AUSmallCpp11NotCpp03Pod@@XZ"(%struct.SmallCpp11NotCpp03Pod* noalias sret %agg.result)

SmallWithCtor small_with_ctor_return() { return SmallWithCtor(); }
// LINUX-LABEL: define void @_Z22small_with_ctor_returnv(%struct.SmallWithCtor* noalias sret %agg.result)
// WIN32: define void @"\01?small_with_ctor_return@@YA?AUSmallWithCtor@@XZ"(%struct.SmallWithCtor* noalias sret %agg.result)
// WIN64: define void @"\01?small_with_ctor_return@@YA?AUSmallWithCtor@@XZ"(%struct.SmallWithCtor* noalias sret %agg.result)

SmallWithVftable small_with_vftable_return() { return SmallWithVftable(); }
// LINUX-LABEL: define void @_Z25small_with_vftable_returnv(%struct.SmallWithVftable* noalias sret %agg.result)
// WIN32: define void @"\01?small_with_vftable_return@@YA?AUSmallWithVftable@@XZ"(%struct.SmallWithVftable* noalias sret %agg.result)
// WIN64: define void @"\01?small_with_vftable_return@@YA?AUSmallWithVftable@@XZ"(%struct.SmallWithVftable* noalias sret %agg.result)

MediumWithCopyCtor medium_with_copy_ctor_return() { return MediumWithCopyCtor(); }
// LINUX-LABEL: define void @_Z28medium_with_copy_ctor_returnv(%struct.MediumWithCopyCtor* noalias sret %agg.result)
// WIN32: define void @"\01?medium_with_copy_ctor_return@@YA?AUMediumWithCopyCtor@@XZ"(%struct.MediumWithCopyCtor* noalias sret %agg.result)
// WIN64: define void @"\01?medium_with_copy_ctor_return@@YA?AUMediumWithCopyCtor@@XZ"(%struct.MediumWithCopyCtor* noalias sret %agg.result)

// Returning a large struct that doesn't fit into a register.
Big big_return() { return Big(); }
// LINUX-LABEL: define void @_Z10big_returnv(%struct.Big* noalias sret %agg.result)
// WIN32: define void @"\01?big_return@@YA?AUBig@@XZ"(%struct.Big* noalias sret %agg.result)
// WIN64: define void @"\01?big_return@@YA?AUBig@@XZ"(%struct.Big* noalias sret %agg.result)


void small_arg(Small s) {}
// LINUX-LABEL: define void @_Z9small_arg5Small(%struct.Small* byval align 4 %s)
// WIN32: define void @"\01?small_arg@@YAXUSmall@@@Z"(%struct.Small* byval align 4 %s)
// WIN64: define void @"\01?small_arg@@YAXUSmall@@@Z"(i32 %s.coerce)

void medium_arg(Medium s) {}
// LINUX-LABEL: define void @_Z10medium_arg6Medium(%struct.Medium* byval align 4 %s)
// WIN32: define void @"\01?medium_arg@@YAXUMedium@@@Z"(%struct.Medium* byval align 4 %s)
// WIN64: define void @"\01?medium_arg@@YAXUMedium@@@Z"(i64 %s.coerce)

void small_arg_with_ctor(SmallWithCtor s) {}
// LINUX-LABEL: define void @_Z19small_arg_with_ctor13SmallWithCtor(%struct.SmallWithCtor* byval align 4 %s)
// WIN32: define void @"\01?small_arg_with_ctor@@YAXUSmallWithCtor@@@Z"(%struct.SmallWithCtor* byval align 4 %s)
// WIN64: define void @"\01?small_arg_with_ctor@@YAXUSmallWithCtor@@@Z"(i32 %s.coerce)

// Test that dtors are invoked in the callee.
void small_arg_with_dtor(SmallWithDtor s) {}
// WIN32: define void @"\01?small_arg_with_dtor@@YAXUSmallWithDtor@@@Z"(<{ %struct.SmallWithDtor }>* inalloca) {{.*}} {
// WIN32:   call x86_thiscallcc void @"\01??1SmallWithDtor@@QAE@XZ"
// WIN32: }
// WIN64: define void @"\01?small_arg_with_dtor@@YAXUSmallWithDtor@@@Z"(%struct.SmallWithDtor* %s) {{.*}} {
// WIN64:   call void @"\01??1SmallWithDtor@@QEAA@XZ"
// WIN64: }

// Test that references aren't destroyed in the callee.
void ref_small_arg_with_dtor(const SmallWithDtor &s) { }
// WIN32: define void @"\01?ref_small_arg_with_dtor@@YAXABUSmallWithDtor@@@Z"(%struct.SmallWithDtor* %s) {{.*}} {
// WIN32-NOT:   call x86_thiscallcc void @"\01??1SmallWithDtor@@QAE@XZ"
// WIN32: }

// Test that temporaries passed by reference are destroyed in the caller.
void temporary_ref_with_dtor() {
  ref_small_arg_with_dtor(SmallWithDtor());
}
// WIN32: define void @"\01?temporary_ref_with_dtor@@YAXXZ"() {{.*}} {
// WIN32:   call x86_thiscallcc %struct.SmallWithDtor* @"\01??0SmallWithDtor@@QAE@XZ"
// WIN32:   call void @"\01?ref_small_arg_with_dtor@@YAXABUSmallWithDtor@@@Z"
// WIN32:   call x86_thiscallcc void @"\01??1SmallWithDtor@@QAE@XZ"
// WIN32: }

void takes_two_by_val_with_dtor(SmallWithDtor a, SmallWithDtor b);
void eh_cleanup_arg_with_dtor() {
  takes_two_by_val_with_dtor(SmallWithDtor(), SmallWithDtor());
}
//   When exceptions are off, we don't have any cleanups.  See
//   microsoft-abi-exceptions.cpp for these cleanups.
// WIN32: define void @"\01?eh_cleanup_arg_with_dtor@@YAXXZ"() {{.*}} {
// WIN32:   call x86_thiscallcc %struct.SmallWithDtor* @"\01??0SmallWithDtor@@QAE@XZ"
// WIN32:   call x86_thiscallcc %struct.SmallWithDtor* @"\01??0SmallWithDtor@@QAE@XZ"
// WIN32:   call void @"\01?takes_two_by_val_with_dtor@@YAXUSmallWithDtor@@0@Z"
// WIN32-NOT: call x86_thiscallcc void @"\01??1SmallWithDtor@@QAE@XZ"
// WIN32: }

void small_arg_with_vftable(SmallWithVftable s) {}
// LINUX-LABEL: define void @_Z22small_arg_with_vftable16SmallWithVftable(%struct.SmallWithVftable* %s)
// WIN32: define void @"\01?small_arg_with_vftable@@YAXUSmallWithVftable@@@Z"(<{ %struct.SmallWithVftable }>* inalloca)
// WIN64: define void @"\01?small_arg_with_vftable@@YAXUSmallWithVftable@@@Z"(%struct.SmallWithVftable* %s)

void medium_arg_with_copy_ctor(MediumWithCopyCtor s) {}
// LINUX-LABEL: define void @_Z25medium_arg_with_copy_ctor18MediumWithCopyCtor(%struct.MediumWithCopyCtor* %s)
// WIN32: define void @"\01?medium_arg_with_copy_ctor@@YAXUMediumWithCopyCtor@@@Z"(<{ %struct.MediumWithCopyCtor }>* inalloca)
// WIN64: define void @"\01?medium_arg_with_copy_ctor@@YAXUMediumWithCopyCtor@@@Z"(%struct.MediumWithCopyCtor* %s)

void big_arg(Big s) {}
// LINUX-LABEL: define void @_Z7big_arg3Big(%struct.Big* byval align 4 %s)
// WIN32: define void @"\01?big_arg@@YAXUBig@@@Z"(%struct.Big* byval align 4 %s)
// WIN64: define void @"\01?big_arg@@YAXUBig@@@Z"(%struct.Big* %s)

// FIXME: Add WIN64 tests. Currently, even the method manglings are wrong (sic!).
class Class {
 public:
  Small thiscall_method_small() { return Small(); }
  // LINUX: define {{.*}} void @_ZN5Class21thiscall_method_smallEv(%struct.Small* noalias sret %agg.result, %class.Class* %this)
  // WIN32: define {{.*}} x86_thiscallcc void @"\01?thiscall_method_small@Class@@QAE?AUSmall@@XZ"(%struct.Small* noalias sret %agg.result, %class.Class* %this)

  SmallWithCtor thiscall_method_small_with_ctor() { return SmallWithCtor(); }
  // LINUX: define {{.*}} void @_ZN5Class31thiscall_method_small_with_ctorEv(%struct.SmallWithCtor* noalias sret %agg.result, %class.Class* %this)
  // WIN32: define {{.*}} x86_thiscallcc void @"\01?thiscall_method_small_with_ctor@Class@@QAE?AUSmallWithCtor@@XZ"(%struct.SmallWithCtor* noalias sret %agg.result, %class.Class* %this)

  Small __cdecl cdecl_method_small() { return Small(); }
  // LINUX: define {{.*}} void @_ZN5Class18cdecl_method_smallEv(%struct.Small* noalias sret %agg.result, %class.Class* %this)
  // WIN32: define {{.*}} void @"\01?cdecl_method_small@Class@@QAA?AUSmall@@XZ"(%struct.Small* noalias sret %agg.result, %class.Class* %this)

  Big __cdecl cdecl_method_big() { return Big(); }
  // LINUX: define {{.*}} void @_ZN5Class16cdecl_method_bigEv(%struct.Big* noalias sret %agg.result, %class.Class* %this)
  // WIN32: define {{.*}} void @"\01?cdecl_method_big@Class@@QAA?AUBig@@XZ"(%struct.Big* noalias sret %agg.result, %class.Class* %this)

  void thiscall_method_arg(Empty s) {}
  // LINUX: define {{.*}} void @_ZN5Class19thiscall_method_argE5Empty(%class.Class* %this)
  // WIN32: define {{.*}} void @"\01?thiscall_method_arg@Class@@QAEXUEmpty@@@Z"(%class.Class* %this, %struct.Empty* byval align 4 %s)

  void thiscall_method_arg(EmptyWithCtor s) {}
  // LINUX: define {{.*}} void @_ZN5Class19thiscall_method_argE13EmptyWithCtor(%class.Class* %this)
  // WIN32: define {{.*}} void @"\01?thiscall_method_arg@Class@@QAEXUEmptyWithCtor@@@Z"(%class.Class* %this, %struct.EmptyWithCtor* byval align 4 %s)

  void thiscall_method_arg(Small s) {}
  // LINUX: define {{.*}} void @_ZN5Class19thiscall_method_argE5Small(%class.Class* %this, %struct.Small* byval align 4 %s)
  // WIN32: define {{.*}} void @"\01?thiscall_method_arg@Class@@QAEXUSmall@@@Z"(%class.Class* %this, %struct.Small* byval align 4 %s)

  void thiscall_method_arg(SmallWithCtor s) {}
  // LINUX: define {{.*}} void @_ZN5Class19thiscall_method_argE13SmallWithCtor(%class.Class* %this, %struct.SmallWithCtor* byval align 4 %s)
  // WIN32: define {{.*}} void @"\01?thiscall_method_arg@Class@@QAEXUSmallWithCtor@@@Z"(%class.Class* %this, %struct.SmallWithCtor* byval align 4 %s)

  void thiscall_method_arg(Big s) {}
  // LINUX: define {{.*}} void @_ZN5Class19thiscall_method_argE3Big(%class.Class* %this, %struct.Big* byval align 4 %s)
  // WIN32: define {{.*}} void @"\01?thiscall_method_arg@Class@@QAEXUBig@@@Z"(%class.Class* %this, %struct.Big* byval align 4 %s)
};

void use_class() {
  Class c;
  c.thiscall_method_small();
  c.thiscall_method_small_with_ctor();

  c.cdecl_method_small();
  c.cdecl_method_big();

  c.thiscall_method_arg(Empty());
  c.thiscall_method_arg(EmptyWithCtor());
  c.thiscall_method_arg(Small());
  c.thiscall_method_arg(SmallWithCtor());
  c.thiscall_method_arg(Big());
}

struct X {
  X();
  ~X();
};
void g(X) {
}
// WIN32: define void @"\01?g@@YAXUX@@@Z"(<{ %struct.X }>* inalloca) {{.*}} {
// WIN32:   call x86_thiscallcc void @"\01??1X@@QAE@XZ"(%struct.X* {{.*}})
// WIN32: }
void f() {
  g(X());
}
// WIN32: define void @"\01?f@@YAXXZ"() {{.*}} {
// WIN32-NOT: call {{.*}} @"\01??1X@@QAE@XZ"
// WIN32: }


namespace test2 {
// We used to crash on this due to the mixture of POD byval and non-trivial
// byval.

struct NonTrivial {
  NonTrivial();
  NonTrivial(const NonTrivial &o);
  ~NonTrivial();
  int a;
};
struct POD { int b; };

int foo(NonTrivial a, POD b);
void bar() {
  POD b;
  b.b = 13;
  int c = foo(NonTrivial(), b);
}
// WIN32-LABEL: define void @"\01?bar@test2@@YAXXZ"() {{.*}} {
// WIN32:   %[[argmem:[^ ]*]] = alloca inalloca [[argmem_ty:<{ %"struct.test2::NonTrivial", %"struct.test2::POD" }>]]
// WIN32:   getelementptr inbounds [[argmem_ty]]* %[[argmem]], i32 0, i32 1
// WIN32:   call void @llvm.memcpy
// WIN32:   getelementptr inbounds [[argmem_ty]]* %[[argmem]], i32 0, i32 0
// WIN32:   call x86_thiscallcc %"struct.test2::NonTrivial"* @"\01??0NonTrivial@test2@@QAE@XZ"
// WIN32:   call i32 @"\01?foo@test2@@YAHUNonTrivial@1@UPOD@1@@Z"([[argmem_ty]]* inalloca %argmem)
// WIN32:   ret void
// WIN32: }

}

// We would crash here because the later definition of ForwardDeclare1 results
// in a different IR type for the value we want to store.  However, the alloca's
// type will use the argument type selected by fn1.
struct ForwardDeclare1;

typedef void (*FnPtr1)(ForwardDeclare1);
void fn1(FnPtr1, SmallWithDtor) {}

struct ForwardDeclare1 {};

void fn2() { fn1(0, SmallWithDtor()); };
// WIN32-LABEL: define void @"\01?fn2@@YAXXZ"
// WIN32:   %[[argmem:[^ ]*]] = alloca inalloca [[argmem_ty:<{ {}\*, %struct.SmallWithDtor }>]]
// WIN32:   getelementptr inbounds [[argmem_ty]]* %[[argmem]], i32 0, i32 1
// WIN32:   call x86_thiscallcc %struct.SmallWithDtor* @"\01??0SmallWithDtor@@QAE@XZ"(%struct.SmallWithDtor* %0)
// WIN32:   getelementptr inbounds [[argmem_ty]]* %[[argmem]], i32 0, i32 0
// WIN32:   %[[addr:[^ ]*]] = bitcast {}** %1 to void [[dst_ty:\(%struct.ForwardDeclare1\*\)\*]]*
// WIN32:   store void [[dst_ty]] null, void [[dst_ty]]* %[[addr]], align 4
// WIN32:   call void @"\01?fn1@@YAXP6AXUForwardDeclare1@@@ZUSmallWithDtor@@@Z"([[argmem_ty]]* inalloca %[[argmem]])
