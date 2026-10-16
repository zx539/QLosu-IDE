#ifndef LOSU_BASE_H__
#define LOSU_BASE_H__
 
#define LOSU_BASE64_CODE_TABLE "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define LOSU_BASE64_CODE_PADDING (uint8_t)'='

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#define _encode(x) ((unsigned char*)LOSU_BASE64_CODE_TABLE)[(uint8_t)x]

int _decode(uint8_t x);

uint8_t *Base64Encode(uint8_t *content, int length);
uint8_t *Base64Decode(uint8_t *content, int length);

#endif