#include "parallel.h"

using namespace parlay;

// A serial implementation for checking correctness.
//
// Work = O(n)
// Depth = O(n)
template <class T, class F>
T scan_inplace_serial(T *A, size_t n, const F& f, T id) {
  T cur = id;
  for (size_t i=0; i<n; ++i) {
    T next = f(cur, A[i]);
    A[i] = cur;
    cur = next;
  }
  return cur;
}

// Computes partial sum of left subtree and store and return
template <class T, class F>
T scan_up(T *A, size_t n, T *L, const F& f){

  if(n == 0) return 0;
  if(n == 1) return A[0];

  T l, r;
  auto f1 = [&](){l = scan_up(A, n/2, L, f );};
  auto f2 = [&](){r = scan_up(A + n/2, n - n/2, L + n/2, f );};

  par_do(f1, f2);

  L[n/2-1] = l;

  return f(l,r);

}


// 
template <class T, class F>
void scan_down(T *R, size_t n, T *L, const F& f, T s){

  if(n == 1){
    R[0] = s;
  }
  else if(n <= 10000000)
  {
    scan_inplace_serial(R, n, f, s);
  }

  else{
    scan_down(R, n/2, L, f, s);
    scan_down(R+n/2, n-n/2, L+n/2, f, L[n/2-1]+s);
  }

}


// Parallel in-place prefix sums. Your implementation can allocate and
// use an extra n*sizeof(T) bytes of memory.
//
// The work/depth bounds of your implementation should be:
// Work = O(n)
// Depth = O(\log(n))
template <class T, class F>
T scan_inplace(T *A, size_t n, const F& f, T id) {
  
  T* L = (T*)malloc((n-1) * sizeof(T));
  
  T total = scan_up(A,n,L,f);
  scan_down(A,n,L,f,id);

  free(L);
  
  return total;  
}
