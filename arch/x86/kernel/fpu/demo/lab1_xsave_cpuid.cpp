#include <iostream>

#define XSTATE_CPUID		0x0000000d

/// __cpuIdCount - Execute the specified cpuid and return the 4 values in
/// the specified arguments.  If we can't run cpuid on the host, return true.
static bool __cpuIdCount(unsigned id, unsigned count, 
                         unsigned *rEAX, unsigned *rEBX, unsigned *rECX, unsigned *rEDX) {
#if defined(__x86_64__)
  // gcc doesn't know cpuid would clobber ebx/rbx. Preserve it manually.
  // FIXME: should we save this for Clang?
  __asm__("movq\t%%rbx, %%rsi\n\t"
          "cpuid\n\t"
          "xchgq\t%%rbx, %%rsi\n\t"
          : "=a"(*rEAX), "=S"(*rEBX), "=c"(*rECX), "=d"(*rEDX)
          : "a"(id), "c"(count));
  return true;
#elif defined(__i386__)
  std::cout << "[[ i386 is retired... Use X64 instead ]]\n"; 
  return false;
#endif
}


int main(){
    std::cout << " Hello \n"; 

    unsigned eax, ebx, ecx, edx;
    __cpuIdCount(XSTATE_CPUID, 2,  &eax, &ebx, &ecx, &edx); 

    std::cout << "State Component 2:  \n"; 
    std::cout << "flag: " << ecx <<  "\n"; 
    std::cout << "offset: " << ebx << "\n"; 
    std::cout << "size: " << eax << "\n"; 
    return 0; 
}