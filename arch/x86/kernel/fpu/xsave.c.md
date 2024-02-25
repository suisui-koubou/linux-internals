# XSAVE 

<https://elixir.bootlin.com/linux/v6.8-rc5/source/arch/x86/kernel/fpu/xstate.c>

## cpuid_count

<https://doc.rust-lang.org/beta/core/arch/x86_64/fn.__cpuid_count.html>

```rust
pub unsafe fn __cpuid_count(leaf: u32, sub_leaf: u32) -> CpuidResult
```

> The CPUID Wikipedia page contains how to query which information using the `EAX`(leaf) and `ECX`(sub-leaf) registers, 
> and the interpretation of the results returned in `EAX`, `EBX`, `ECX`, and `EDX`.

<https://en.wikipedia.org/wiki/CPUID>

> The `CPUID` opcode is `0F A2`.
> In assembly language, the `CPUID` instruction takes no parameters as `CPUID` implicitly uses the `EAX` register to determine the main category of information returned. 
> In Intel's more recent terminology, this is called the `CPUID` **leaf**. 
> CPUID should be called with `EAX` = 0 first, as this will store in the EAX register the highest EAX calling parameter (leaf) that the CPU implements.
> Some of the more recently added leaves also have **sub-leaves**, which are selected via the `ECX` register before calling `CPUID`.

There is more readable formate 

<http://www.bricktou.com/arch/x86/include/asm/processorcpuid_count_src.html>

```c
/* Some CPUID calls want 'count' to be placed in ecx */
static inline void cpuid_count(unsigned int id, int count,
                   unsigned int *eax, unsigned int *ebx,
                   unsigned int *ecx, unsigned int *edx)
{
    *eax = id;
    *ecx = count;
    __cpuid(eax, ebx, ecx, edx);
}
```

Actually, the code looks like 

```c 
void cpuid_count(u32 id, u32 count, u32 *a, u32 *b, u32 *c, u32 *d)
{
	asm volatile("cpuid"
		     : "=a" (*a), "=b" (*b), "=c" (*c), "=d" (*d)
		     : "0" (id), "2" (count)
	);
}
```

The input is `RAX` (`"0"`) and `RCX` (`"2"`) (I think `RBX` is `"1"`?). 


## setup_xstate_cache

> 13.4.3 Extended Region of an XSAVE Area
> 
> **Standard format**. 
> 
> Each state component i (`i >= 2`) is located at the byte offset from the base address of the XSAVE area enumerated in `CPUID.(EAX=0DH,ECX=i):EBX`. 
> 
> `(CPUID.(EAX=0DH,ECX=i):EAX` enumerates the number of bytes required for state component i.


```c
/*
 * Record the offsets and sizes of various xstates contained
 * in the XSAVE state memory layout.
 */
static void __init setup_xstate_cache(void)
{
	u32 eax, ebx, ecx, edx, i;
	/* start at the beginning of the "extended state" */
	unsigned int last_good_offset = offsetof(struct xregs_state,
						 extended_state_area);
	/*
	 * The FP xstates and SSE xstates are legacy states. They are always
	 * in the fixed offsets in the xsave area in either compacted form
	 * or standard form.
	 */
	xstate_offsets[XFEATURE_FP]	= 0;
	xstate_sizes[XFEATURE_FP]	= offsetof(struct fxregs_state,
						   xmm_space);

	xstate_offsets[XFEATURE_SSE]	= xstate_sizes[XFEATURE_FP];
	xstate_sizes[XFEATURE_SSE]	= sizeof_field(struct fxregs_state,
						       xmm_space);

	for_each_extended_xfeature(i, fpu_kernel_cfg.max_features) {
		cpuid_count(XSTATE_CPUID, i, &eax, &ebx, &ecx, &edx);

		xstate_sizes[i] = eax;
		xstate_flags[i] = ecx;

		/*
		 * If an xfeature is supervisor state, the offset in EBX is
		 * invalid, leave it to -1.
		 */
		if (xfeature_is_supervisor(i))
			continue;

		xstate_offsets[i] = ebx;

		/*
		 * In our xstate size checks, we assume that the highest-numbered
		 * xstate feature has the highest offset in the buffer.  Ensure
		 * it does.
		 */
		WARN_ONCE(last_good_offset > xstate_offsets[i],
			  "x86/fpu: misordered xstate at %d\n", last_good_offset);

		last_good_offset = xstate_offsets[i];
	}
}
```