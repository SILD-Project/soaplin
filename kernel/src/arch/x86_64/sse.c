#include <arch/x86_64/sse.h>
#include <sys/log.h>
#include <sys/printf.h>

int cpuid_check_bit(int reg, int bit) {
  int eax, ebx, ecx, edx;

  // Minimal inline assembly to execute CPUID
  __asm__ volatile("cpuid" // Execute CPUID instruction
                   : "=a"(eax), "=b"(ebx), "=c"(ecx),
                     "=d"(edx) // Output registers
                   : "a"(0x1)  // Input: EAX = 0x1 (query feature flags)
                   :           // No clobbered registers
  );

  // Check bit 25 of EDX (SSE support) in plain C
  if (reg == 0) {
    if (edx & (1 << bit)) {
      return 1; // SSE is supported
    } else {
      return 0; // SSE is not supported
    }
  } else if (reg == 1) {
    if (ecx & (1 << bit)) {
      return 1; // SSE is supported
    } else {
      return 0; // SSE is not supported
    }
  }

  return 0;
}

void sse_init() {
  int sse = cpuid_check_bit(0, 25);
  int sse2 = cpuid_check_bit(0, 26);
  int sse3 = cpuid_check_bit(1, 0);
  int ssse3 = cpuid_check_bit(1, 9);

  if (sse)
    log("sse - sse is supported!\n");
  else
    log("sse - sse isn't supported!\n");

  if (sse2)
    log("sse - sse2 is supported!\n");
  else
    log("sse - sse2 isn't supported!\n");

  if (sse3)
    log("sse - sse3 is supported!\n");
  else
    log("sse - sse3 isn't supported!\n");

  if (ssse3)
    log("sse - ssse3 is supported!\n");
  else
    log("sse - ssse3 isn't supported!\n");
}