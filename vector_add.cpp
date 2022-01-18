#include <bang.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>


__mlu_global__ void VectorAdd(float *dA, float *dB, float *dC, int N) {
  struct timeval start;
  struct timeval end;
  gettimeofday(&start, NULL);

  for (int i = 0; i < N; i++) {
      dC[i] = dA[i] + dB[i];
  }
  gettimeofday(&end, NULL);
  uint32_t time_usec = (uint32_t)end.tv_usec - (uint32_t)start.tv_usec;
  printf("MLU Total Time: %u us\n", time_usec);
}

int main() {
  cnrtDev_t dev;
  cnrtQueue_t queue;
  cnrtDim3_t dim = {1, 1, 1};
  cnrtFunctionType_t ktype = CNRT_FUNC_TYPE_BLOCK;

  cnrtInit(0);
  cnrtGetDeviceHandle(&dev, 0);
  cnrtSetCurrentDevice(dev);
  cnrtCreateQueue(&queue);

  float *pA, *pB, *pC;
  int N = 1024;
  float A[N];
  float B[N];
  float C[N];

  for (int i = 0; i < N; i++) {
      A[i] = (float)(rand() % 100);
  }

  for (int i = 0; i < N; i++) {
      B[i] = (float)(rand() % 100);
  }


  CNRT_CHECK(cnrtMalloc((void **)&pA, N * sizeof(float)));
  CNRT_CHECK(cnrtMalloc((void **)&pB, N * sizeof(float)));
  CNRT_CHECK(cnrtMalloc((void **)&pC, N * sizeof(float)));
  CNRT_CHECK(cnrtMemcpy(pA, A, N * sizeof(float), CNRT_MEM_TRANS_DIR_HOST2DEV));
  CNRT_CHECK(cnrtMemcpy(pB, B, N * sizeof(float), CNRT_MEM_TRANS_DIR_HOST2DEV));

  VectorAdd<<<dim, ktype, queue>>>(pA, pB, pC, N);

  CNRT_CHECK(cnrtSyncQueue(queue));
  CNRT_CHECK(cnrtMemcpy(C, pC, N * sizeof(float), CNRT_MEM_TRANS_DIR_DEV2HOST));

  for (int i = 0; i < 10; i++) {
        printf("i = %d mlu: %f \n", i, C[i]);
  }


  CNRT_CHECK(cnrtFree(pA));
  CNRT_CHECK(cnrtFree(pB));
  CNRT_CHECK(cnrtFree(pC));
  CNRT_CHECK(cnrtDestroyQueue(queue));

  cnrtDestroy();
  return 0;
}

