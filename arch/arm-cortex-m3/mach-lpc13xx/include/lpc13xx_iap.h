#ifndef LPC13XX_IAP_H_
#define LPC13XX_IAP_H_

#include <types.h>

int iap_readuuid(u8 *uuid);
int iap_invokeisp(void);

#endif
