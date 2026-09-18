#ifndef _MT19937_H_
#define _MT19937_H_

#ifdef __cplusplus
extern "C" {
#endif
void init_genrand(unsigned long s);
void init_by_array(unsigned long init_key[], int key_length);
unsigned long genrand_int32(void);
long genrand_int31(void);
#ifdef __cplusplus
};
#endif

#endif // _MT19937_H_
