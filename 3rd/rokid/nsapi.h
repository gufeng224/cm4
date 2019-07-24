#ifndef _NS_H_
#define _NS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * NS Frame size must 10 ms length
 */
typedef struct Ns Ns;
Ns  *ns_create(int sample_rate, int mode);
int  ns_init(Ns *ns);
void ns_destroy(Ns *ns);

// frame length must 10 ms
int ns_process(Ns *ns, short *frame_in, int in_len, short *frame_out, int buf_size);

#ifdef __cplusplus
}
#endif
#endif
