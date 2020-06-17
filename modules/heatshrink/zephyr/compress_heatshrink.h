#ifndef _COMPRESS_HEATSHRINK_H
#define _COMPRESS_HEATSHRINK_H

typedef struct {
  unsigned char *ptr;
  size_t len;
} HeatShrinkPtrInputCallbackInfo;

void heatshrink_ptr_output_cb(unsigned char ch, uint32_t *cbdata, uint32_t * count, uint32_t len); // takes **data
int heatshrink_ptr_input_cb(uint32_t *cbdata, uint32_t * count, uint32_t len); // takes *HeatShrinkPtrInputCallbackInfo
void heatshrink_var_output_cb(unsigned char ch, uint32_t *cbdata, uint32_t * count, uint32_t len); // takes *JsvStringIterator
int heatshrink_var_input_cb(uint32_t *cbdata, uint32_t * count, uint32_t len); // takes *JsvIterator

/** gets data from callback, writes to callback if nonzero. Returns total length. */
uint32_t heatshrink_encode_cb(int (*in_callback)(uint32_t *cbdata, uint32_t * count, uint32_t len), 
                              uint32_t *in_cbdata, 
                              uint32_t in_len, 
                              void (*out_callback)(unsigned char ch, uint32_t *cbdata, uint32_t * count, uint32_t len), 
                              uint32_t *out_cbdata,
                              uint32_t out_len);

/** gets data from callback, writes it into callback if nonzero. Returns total length */
uint32_t heatshrink_decode_cb(int (*in_callback)(uint32_t *cbdata, uint32_t * count, uint32_t len), 
                              uint32_t *in_cbdata, 
                              uint32_t in_len, 
                              void (*out_callback)(unsigned char ch, uint32_t *cbdata, uint32_t * count, uint32_t len), 
                              uint32_t *out_cbdata, 
                              uint32_t out_len);

/** gets data from array, writes to callback if nonzero. Returns total length. */
uint32_t heatshrink_encode(unsigned char *in_data, 
                           size_t in_len, 
                           void (*out_callback)(unsigned char ch, uint32_t *cbdata, uint32_t * count, uint32_t len), 
                           uint32_t *out_cbdata, 
                           uint32_t out_len);

/** gets data from callback, writes it into array if nonzero. Returns total length */
uint32_t heatshrink_decode(int (*in_callback)(uint32_t *cbdata, uint32_t * count, uint32_t len), 
                           uint32_t *in_cbdata, 
                           uint32_t in_len,
                           unsigned char *out_data,
                           uint32_t out_len);

#endif
