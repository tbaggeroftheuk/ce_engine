#ifndef TCF_H
#define TCF_H

#include <stdint.h>

#define TCF_OK                0
#define TCF_ERR_IO           -1
#define TCF_ERR_FORMAT       -2
#define TCF_ERR_CRC          -3
#define TCF_ERR_MEMORY       -4
#define TCF_ERR_NOT_FOUND    -5
#define TCF_ERR_ARG          -6

#ifdef __cplusplus
extern "C" {
#endif

int tcf_extract(const char *tcf_path, const char *output_dir);
int tcf_pack(const char *input_dir, const char *out_path);

/* Loads a single file from a .tcf into memory.
 * - inner_path is the archive path (forward slashes).
 * - out_data is heap-allocated with malloc; free with tcf_free().
 */
int tcf_load_file(const char *tcf_path,
                  const char *inner_path,
                  uint8_t **out_data,
                  uint32_t *out_size);

void tcf_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif 
