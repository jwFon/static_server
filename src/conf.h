#ifndef _CONF_H_
#define _CONF_H_

#include <string>

extern int PORT;
extern int MAX_EVENT;
extern int BUF_BLOCK_SIZE;
extern int MAX_ESTABLISHED_COUNT;

void conf_init();

#endif