#ifndef DSPI_H_
#define DSPI_H_

#include <stdint.h>

void dspi_init(void);
inline uint16_t dspi_no_fifo_3(uint32_t master);

#endif /* DSPI_H_ */
