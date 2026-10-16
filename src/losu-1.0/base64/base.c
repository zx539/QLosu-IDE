#include "base.h"

int _decode(uint8_t x)
{
    /*
        该函数用于从编码表（LOSU_BASE64_CODE_TABLE）中查找对应字符的序号
        在Base64Decode函数中，这大概率是性能制约部分
        如需优化，可考虑使用二叉查找树进行优化
    */
    for (int i=0; i<strlen(LOSU_BASE64_CODE_TABLE); i++)
        if (LOSU_BASE64_CODE_TABLE[i]==(char)x)
            return i;
    return -1;
}

uint8_t *Base64Encode(uint8_t *content, int length)
{
    //创建缓冲区
    int buffersize = (length + length % 3) / 3 * 4;
    uint8_t *buffer = malloc(buffersize+sizeof(char));

    //编码
    int left = length % 3;
    int i, j;
    for (i=j=0; i<length-left; i+=3, j+=4)
    {
        buffer[j]   = _encode((content[i] & 0xfc) >> 2);                                       //第一个byte的前6bit
        buffer[j+1] = _encode(((content[i] & 0x3) << 4) + ((content[i+1] & 0xf0) >> 4));       //第一个字节的后2bit和第二个字节的前4bit
        buffer[j+2] = _encode(((content[i+1] & 0xf) << 2) + ((content[i+2] & 0xc0) >> 6));     //第二个字节的后4bit和第三个字节的前2bit
        buffer[j+3] = _encode(content[i+2] & 0x3f);                                            //第三个字节的后6bit
    }

    //填充
    switch (left)
    {
        case 0:
        {
            //长度正好时
            *((char*)&buffer[j]) = '\0';
            break;
        }
        case 1:
        {
            //剩余一个字节
            buffer[j]   = _encode((content[i] & 0xfc) >> 2);                                   //第一个byte的前6bit
            buffer[j+1] = _encode((content[i] & 0x3) << 4);                                    //第一个字节的后2bit和第二个字节的前4bit
            //填充
            buffer[j+2] = LOSU_BASE64_CODE_PADDING;
            buffer[j+3] = LOSU_BASE64_CODE_PADDING;
            *((char*)&buffer[j+4]) = '\0';
            break;
        }
        case 2:
        {
            //剩余两个字节
            buffer[j]   = _encode((content[i] & 0xfc) >> 2);                                   //第一个byte的前6bit
            buffer[j+1] = _encode(((content[i] & 0x3) << 4) + ((content[i+1] & 0xf0) >> 4));   //第一个字节的后2bit和第二个字节的前4bit
            buffer[j+2] = _encode((content[i+1] & 0xf) << 2);                                  //第二个字节的后4bit
            //填充
            buffer[j+3] = LOSU_BASE64_CODE_PADDING;
            *((char*)&buffer[j+4]) = '\0';
            break;
        }
    }

    return buffer; //记得free!!!!!!
}

uint8_t *Base64Decode(uint8_t *content, int length)
{
    //创建缓冲区
    int buffersize = (length / 4) * 3;
    uint8_t *buffer = malloc(buffersize+sizeof(char));
    memset(buffer, 0, buffersize+sizeof(char));

    //计算填充字符个数
    int left = 0;
    if (content[length-1]==LOSU_BASE64_CODE_PADDING)
        left++;
    if (content[length-2]==LOSU_BASE64_CODE_PADDING)
        left++;
    
    //解码
    int i, j, i_max;
    i_max = left == 0 ? length : length - 4;
    for (i=j=0; i<i_max; i+=4, j+=3)
    {
        buffer[j]   = ((_decode(content[i]) & 0x3f) << 2) + ((_decode(content[i+1]) & 0x30) >> 4);  //第一个byte的后6bit和第二个byte的第3到第4bit
        buffer[j+1] = ((_decode(content[i+1]) & 0xf) << 4) + ((_decode(content[i+2]) & 0x3c) >> 2); //第二个byte的第5到第8bit和第三个byte的第3到第6bit
        buffer[j+2] = ((_decode(content[i+2]) & 0x3) << 6) + ((_decode(content[i+3]) & 0x3f));       //第三个byte的第7到第8个bit和第四个byte的第3到第8bit
    }

    //处理填充的区块
    switch (left)
    {
        case 0:
        {
            *((char*)&buffer[j]) = '\0';
            break;
        }
        case 1:
        {
            buffer[j]   = ((_decode(content[i]) & 0x3f) << 2) + ((_decode(content[i+1]) & 0x30) >> 4);  //第一个byte的后6bit和第二个byte的第3到第4bit
            buffer[j+1] = ((_decode(content[i+1]) & 0xf) << 4) + ((_decode(content[i+2]) & 0x3c) >> 2); //第二个byte的第5到第8bit和第三个byte的第3到第6bit
            *((char*)&buffer[j+2]) = '\0';
        }
        case 2:
        {
            buffer[j]   = ((_decode(content[i]) & 0x3f) << 2) + ((_decode(content[i+1]) & 0x30) >> 4);  //第一个byte的后6bit和第二个byte的第3到第4bit
            buffer[j+1] = ((_decode(content[i+1]) & 0xf) << 4); //第二个byte的第5到第8bit
            break;
        }
    }

    return buffer;
}