#ifndef TYPES_H_
# define TYPES_H_

#define	NULL			0x00000000

typedef enum e_bool {
  TRUE = 1,
  FALSE = 0
} bool;

typedef enum e_retv {
  SUCCES = 0,
  FAILED = -1
} retv;

typedef int		int32_t;
typedef	short		int16_t;
typedef	char		int8_t;

typedef	unsigned int	uint32_t;
typedef	unsigned short	uint16_t;
typedef	unsigned char	uint8_t;

typedef	unsigned long	size_t;
typedef	long		ssize_t;

#endif /* !TYPES_H_ */
