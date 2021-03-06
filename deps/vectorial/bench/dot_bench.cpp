
#include "bench.h"
#include <stdlib.h>

#include <iostream>
#include "vectorial/vec4f.h"

#define NUM (81920)
#define ITER 100
using namespace vectorial;

namespace {
    vec4f* alloc_vec4f(size_t n) {
        void *ptr = memalign(n*sizeof(vec4f), 16);
        return static_cast<vec4f*>(ptr);
    }    
}



static vec4f * a;
static vec4f * b;
static float * c;




void dot_func() {
    
    vec4f* vectorial_restrict aa = a;
    vec4f* vectorial_restrict bb = b;
    float* vectorial_restrict cc = c;
    
    for(size_t i = 0; i < NUM; ++i)
    {
        cc[i] = dot(aa[i], bb[i]);
    }    
}

void dot_bench() {

    a = alloc_vec4f(NUM);
    b = alloc_vec4f(NUM);
    c = static_cast<float*>(malloc(NUM * sizeof(float)));


    for(size_t i = 0; i < NUM; ++i)
    {
        a[i]=vec4f(i,i,i,i);
        b[i]=vec4f(NUM-i, NUM-i, NUM-i, NUM-i);
    }
        
    profile("dot", dot_func, ITER, NUM);

    memfree(a);
    memfree(b);
    memfree(c);


}
