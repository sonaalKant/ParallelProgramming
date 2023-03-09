#include "parallel.h"
#include "random.h"

// For timing parts of your code.
#include "get_time.h"

// For computing sqrt(n)
#include <math.h>
#include <map>

using namespace parlay;

// Some helpful utilities
namespace {

// returns the log base 2 rounded up (works on ints or longs or unsigned
// versions)
template <class T>
size_t log2_up(T i) {
  assert(i > 0);
  size_t a = 0;
  T b = i - 1;
  while (b > 0) {
    b = b >> 1;
    a++;
  }
  return a;
}

}  // namespace


struct ListNode {
  ListNode* next;
  size_t rank;
  ListNode(ListNode* next) : next(next), rank(std::numeric_limits<size_t>::max()) {}
};

// Serial List Ranking. The rank of a node is its distance from the
// tail of the list. The tail is the node with `next` field nullptr.
//
// The work/depth bounds are:
// Work = O(n)
// Depth = O(n)
void SerialListRanking(ListNode* head) {
  size_t ctr = 0;
  ListNode* save = head;
  while (head != nullptr) {
    head = head->next;
    ++ctr;
  }
  head = save;
  --ctr;  // last node is distance 0
  while (head != nullptr) {
    head->rank = ctr;
    head = head->next;
    --ctr;
  }
}

size_t WeightedSerialListRanking(ListNode* head, long num_samples, int *weights) {
  
  if(head->next == nullptr)
  {
    head->rank = 0;
    return 0;
  }
  // std :: cout << num_samples << " " << weights[0] << std ::endl;
  head->rank = 1 + WeightedSerialListRanking(head->next, num_samples-1, (weights+1)) + weights[0];

  return head->rank;
  
}

// Wyllie's List Ranking. Based on pointer-jumping.
//
// The work/depth bounds of your implementation should be:
// Work = O(n*\log(n))
// Depth = O(\log^2(n))
void WyllieListRanking(ListNode* L, size_t n) {

    // size_t* D = (size_t*)malloc((n-1) * sizeof(size_t));

    parallel_for(0, n, [&](size_t i)
    {
      if(L[i].next != nullptr) L[i].rank = 1;
      else{
        L[i].rank = 0;
      } 
    });

    for(size_t j=0;j<log2_up(n);j++)
    {
      parallel_for(0, n, [&](size_t i)
      {
        if(L[i].next != nullptr){
          L[i].rank = L[i].rank + L[i].next->rank;
          L[i].next = L[i].next->next;
        }
      });
    }

    // for(int i=0;i<n;i++)
    // {
    //   if(L[i].next != nullptr) L[i].rank = 1;
    //   else
    //     L[i].rank = 0;
    // }

    // for(int j=0;j<log2_up(n);j++)
    // {
    //     for(int i=0;i<n;i++)
    //     {
    //       // std::cout << j << i << L[i].rank << std::endl;
    //       if (L[i].next != nullptr){
    //           L[i].rank = L[i].rank + L[i].next->rank;
    //           L[i].next = L[i].next->next;
    //       }
          
    //     }
    // }
}


// Sampling-Based List Ranking
//
// The work/depth bounds of your implementation should be:
// Work = O(n) whp
// Depth = O(\sqrt(n)* \log(n)) whp
void SamplingBasedListRanking(ListNode* L, size_t n, long num_samples=-1, parlay::random r=parlay::random(0)) {
  // Perhaps use a serial base case for small enough inputs?

  if (num_samples == -1) {
    num_samples = sqrt(n);
  }

  ListNode* L1 = (ListNode*)malloc(num_samples * sizeof(ListNode));
  int *weights = (int*)malloc(num_samples * sizeof(int));
  // int *pos = (int*)malloc(num_samples * sizeof(int))

  L1[0] = L[0];
  int count = 0;

  std::map<int,int> hm;
  hm[0] = 1;

  // std :: cout << "Num Samples " << num_samples << std :: endl;
  ListNode *head = L; 
  // for(int i=0;i<num_samples-2;i++)
  // {
  //   // std::cout << r[i]%100 << " " <<  100/num_samples << " " << num_samples-2 << " " << count << std::endl;  
  //     int idx = r[i] % n; 
  //     while(hm.find(idx) != hm.end())
  //       idx = r[i] % n;
      
  //     hm[idx] = 1;

  //     std :: cout << idx << std :: endl;
  //     L1[count+1] = L[idx];
  //     // std::cout << "here" << std::endl;
  //     L1[count].next = &L1[count+1];
  //     int ctr = 0;
  //     // std::cout << "here" << std::endl;
  //     while (head->next != &L[idx])
  //     {
  //       // std::cout << "here" << std::endl;
  //       head = head->next;
  //       ctr++;
  //     }
  //     std :: cout << idx  << " " << ctr << std ::endl;
  //     head = head->next;
  //     weights[count++] = ctr;
    
  // }

  // 9 --> 3 --> [0,4,8]
  // 16 --> 4 --> [0,5, 10, 15]
  // 25 --> 5 --> [0, 6, 12, 18, 24]
  // 36 --> 6 --> [0, 7, 14, 21, 28, 35]
  // 50 --> 7 --> [0,8,16,24,32,40, 49]

  int ctr = 1;
  int pt = (n-1) / (num_samples-1);
  while(head->next != null && count < num_samples)
  {
    if(ctr % pt == 0)
    {
      
    }

  }

  int ctr = -1;
  while(head->next != nullptr)
  {
    head = head->next;
    ctr++;
  }

  L1[count + 1] = *head;
  // std :: cout << "Istail : " << (L1[count+1].next == nullptr)  << "  " << (head->next == nullptr) << std::endl;
  L1[count].next = &L1[count+1];
  weights[count++] = ctr;
  weights[count] = 0;

  // std :: cout << "Istail : " << (L1[num_samples-1].next == nullptr)  << std::endl;

  // L1[count+1] = L[n-1];
  // L1[count].next = &L1[count+1];

  // int ctr = 0;
  // while (head->next != &L[n-1])
  // {
  //   head = head->next;
  //   ctr++;
  //   std :: cout << " HereR : " << ctr << std::endl;
  // }
  // std :: cout << "Here" << ctr << std :: endl;
  // weights[count] = ctr;
  // count++;

  // weights[count] = 0;

  // std :: cout << "here " << std::endl;

  for(int i =0 ;i < num_samples; i++)
  {
    std::cout<< weights[i] << std::endl;
  }

  WeightedSerialListRanking(L1, num_samples, weights);

  // for(int i =0 ;i < num_samples; i++)
  // {
  //   std::cout<< "Rank "<< L1[i].rank << std::endl;
  // }

  head = L;
  
  for(int i=0;i<num_samples;i++)
  {
    
    // std::cout << "start " << i << " " << (head == nullptr) << std::endl;
    head->rank = L1[i].rank;
    // std::cout << L1[i].rank << std::endl;
    int prev_rank = L1[i].rank;
    // std::cout << "P " << i << std:: endl;
    while(weights[i] > 0)
    {
      head = head->next;
      head->rank = prev_rank - 1;
      // std::cout << head->rank << std::endl;
      weights[i]--;
      prev_rank = head->rank;
    }
    // std::cout << "Final " << std :: endl;
    if(head != nullptr)
      head = head->next;
    // std::cout << "Complete " << i << std::endl;
  }

  // std::cout << "Final 2" << std :: endl;

}

