#ifndef TCF_H
#define TCF_H

#include <stdint.h>

#define TCF_OK                0
#define TCF_ERR_IO           -1
#define TCF_ERR_FORMAT       -2
#define TCF_ERR_CRC          -3
#define TCF_ERR_MEMORY       -4

#ifdef __cplusplus
extern "C" {
#endif

int tcf_extract(const char *tcf_path, const char *output_dir);
int tcf_pack(const char *input_dir, const char *out_path);

#ifdef __cplusplus
}
#endif

#endif 
