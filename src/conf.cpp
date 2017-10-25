#include "conf.h"
#include <iostream>

int PORT;
int MAX_EVENT;
int BUF_BLOCK_SIZE;
int MAX_ESTABLISHED_COUNT;

void conf_init()
{
	PORT = 3000;
	MAX_EVENT = 1000;
	BUF_BLOCK_SIZE = 512;
	MAX_ESTABLISHED_COUNT = 100;
}
