#include <iostream>

/*

    Linux Kernel: 
        https://elixir.bootlin.com/linux/v6.8-rc5/source/arch/x86/kernel/fpu/xstate.c#L230
        https://elixir.bootlin.com/linux/v6.8-rc5/source/arch/x86/include/asm/fpu/xstate.h#L15

    LLVM: 
        https://github.com/llvm/llvm-project/blob/release/13.x/llvm/lib/Support/Host.cpp#L487

*/


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

    // State Component 2 is AVX, read Intel SDM 13.5.3 AVX State
    // its Offset is 576 and size is 256. 
    __cpuIdCount(XSTATE_CPUID, 2,  &eax, &ebx, &ecx, &edx); 
    std::cout << "State Component 2:  \n"; 
    std::cout << "\tflag: " << ecx <<  "\n"; 
    std::cout << "\toffset: " << ebx << "\n"; 
    std::cout << "\tsize: " << eax << "\n"; 


    // State Component 3 is MPX
    __cpuIdCount(XSTATE_CPUID, 3,  &eax, &ebx, &ecx, &edx); 
    std::cout << "State Component 3:  \n"; 
    std::cout << "\tflag: " << ecx <<  "\n"; 
    std::cout << "\toffset: " << ebx << "\n"; 
    std::cout << "\tsize: " << eax << "\n"; 



    // State Component 4 is MPX
    __cpuIdCount(XSTATE_CPUID, 4,  &eax, &ebx, &ecx, &edx); 
    std::cout << "State Component 4:  \n"; 
    std::cout << "\tflag: " << ecx <<  "\n"; 
    std::cout << "\toffset: " << ebx << "\n"; 
    std::cout << "\tsize: " << eax << "\n"; 


    // State Component 5 is AVX-512
    __cpuIdCount(XSTATE_CPUID, 5,  &eax, &ebx, &ecx, &edx); 
    std::cout << "State Component 5:  \n"; 
    std::cout << "\tflag: " << ecx <<  "\n"; 
    std::cout << "\toffset: " << ebx << "\n"; 
    std::cout << "\tsize: " << eax << "\n"; 

    return 0; 
}