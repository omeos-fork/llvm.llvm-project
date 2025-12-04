// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -emit-llvm -o - %s -O1 -fexceptions -fcxx-exceptions | FileCheck %s

extern "C" {

struct Trivial {
  int x[100];
};

void func_that_throws(Trivial t);

// CHECK-LABEL: define{{.*}} void @test()
void test() {
  // CHECK: %[[AGG1:.*]] = alloca %struct.Trivial
  // CHECK: %[[AGG2:.*]] = alloca %struct.Trivial

  // CHECK: call void @llvm.lifetime.start.p0(ptr{{.*}} %[[AGG1]])
  // CHECK: invoke void @func_that_throws(ptr{{.*}} %[[AGG1]])

  // CHECK: [[CONT1:.*]]:
  // CHECK-NEXT: call void @llvm.lifetime.end.p0(ptr{{.*}} %[[AGG1]])
  // CHECK-NEXT: call void @llvm.lifetime.start.p0(ptr{{.*}} %[[AGG2]])
  // CHECK: invoke void @func_that_throws(ptr{{.*}} %[[AGG2]])

  // CHECK: [[CONT2:.*]]:
  // CHECK-NEXT: call void @llvm.lifetime.end.p0(ptr{{.*}} %[[AGG2]])

  // CHECK: [[LPAD1:lpad.*]]:
  // CHECK: landingpad
  // CHECK: br label %[[EHCLEANUP:.*]]

  // CHECK: [[LPAD2:lpad.*]]:
  // CHECK: landingpad
  // CHECK: call void @llvm.lifetime.end.p0(ptr{{.*}} %[[AGG2]])
  // CHECK: br label %[[EHCLEANUP]]

  // CHECK: [[EHCLEANUP]]:
  // CHECK: call void @llvm.lifetime.end.p0(ptr{{.*}} %[[AGG1]])
  try {
    func_that_throws(Trivial{0});
    func_that_throws(Trivial{0});
  } catch (...) {
  }
}
} // end extern "C"
