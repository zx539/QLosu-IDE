#include "SHA384.h"
  
uint64_t Ch(uint64_t X, uint64_t Y, uint64_t Z)
{
    return (X & Y) ^ ((~X) & Z);
}
  
uint64_t Maj(uint64_t X, uint64_t Y, uint64_t Z)
{
    return (X & Y) ^ (X & Z) ^ (Y & Z);
}
  
/* 循环向右移动offset个比特位 */
uint64_t ROTR(uint64_t X, uint64_t offset)
{
    return (X >> offset) | (X << (64 - offset));
}
  
/* 向右移动offset个比特位 */
uint64_t SHR(uint64_t X, uint64_t offset)
{
    return X >> offset;
}
  
/* SIGMA0 */
uint64_t SIGMA0(uint64_t X)
{
    return ROTR(X, 28) ^ ROTR(X, 34) ^ ROTR(X, 39);
}
  
/* SIGMA1 */
uint64_t SIGMA1(uint64_t X)
{
    return ROTR(X, 14) ^ ROTR(X, 18) ^ ROTR(X, 41);
}
  
/* sigma0, different from SIGMA0 */
uint64_t sigma0(uint64_t X)
{
    return ROTR(X, 1) ^ ROTR(X, 8) ^ SHR(X, 7);
}
  
/* sigma1, different from SIGMA1 */
uint64_t sigma1(uint64_t X)
{
    return ROTR(X, 19) ^ ROTR(X, 61) ^ SHR(X, 6);
}
  

  
/* 此处的inlen用int不合理 但我们只是研究学习算法 不作为实际应用 */
int sha2_384(unsigned char *out, const unsigned char* in, const long long inlen)
{
    ASSERT_RETURN_INT(out && in && (inlen >= 0), 1);
    int i = 0, j = 0, t = 0;
  
    // step 1: 字节填充(Append Padding Bytes)
    // 数据先补上1个1比特，再补上k个0比特，使得补位后的数据比特数(n+1+k)满足(n+1+k) mod 1024 = 896，k取最小正整数
    int iX = inlen / HASH_BLOCK_SIZE;
    int iY = inlen % HASH_BLOCK_SIZE;
    iX = (iY < HASH_LEN_OFFSET) ? iX : (iX + 1);
  
    int iLen = (iX + 1) * HASH_BLOCK_SIZE;
    unsigned char* X = malloc(iLen);
    memcpy(X, in, inlen);
    // 先补上1个1比特+7个0比特
    X[inlen] = 0x80;
    // 再补上(k-7)个0比特
    for (i = inlen + 1; i < (iX * HASH_BLOCK_SIZE + HASH_LEN_OFFSET); i++)
    {
        X[i] = 0;
    }
  
    // step 2: 追加长度信息(Append Length) [此处inlen是按int处理的p[0~7]直接写死0 ]
    uint8_t *pLen = X + (iX * HASH_BLOCK_SIZE + HASH_LEN_OFFSET);
    uint64_t iTempLen = inlen << 3;
    uint8_t *pTempLen = (void*)&iTempLen;
    pLen[0] = 0;            pLen[1]  = 0;           pLen[2]  = 0;            pLen[3] = 0;
    pLen[4] = 0;            pLen[5] = 0;            pLen[6]  = 0;            pLen[7] = 0;
    pLen[8]  = pTempLen[7]; pLen[9]  = pTempLen[6]; pLen[10] = pTempLen[5];  pLen[11] = pTempLen[4];
    pLen[12] = pTempLen[3]; pLen[13] = pTempLen[2]; pLen[14] = pTempLen[1];  pLen[15] = pTempLen[0];
  
    // Step 3. 初始化MD Buffer(Initialize MD Buffer)
    uint64_t H0 = 0xCBBB9D5DC1059ED8;
    uint64_t H1 = 0x629A292A367CD507;
    uint64_t H2 = 0x9159015A3070DD17;
    uint64_t H3 = 0x152FECD8F70E5939;
    uint64_t H4 = 0x67332667FFC00B31;
    uint64_t H5 = 0x8EB44A8768581511;
    uint64_t H6 = 0xDB0C2E0D64F98FA7;
    uint64_t H7 = 0x47B5481DBEFA4FA4;
 
    uint64_t M[HASH_BLOCK_SIZE / 8] = { 0 };
    uint64_t W[HASH_ROUND_NUM] = { 0 };
  
    // step 4: 处理消息块(Process Message in 64-Byte Blocks)
    for (i = 0; i < iLen / HASH_BLOCK_SIZE; i++)
    {
        /* Copy block i into M. */
        for (j = 0; j < HASH_BLOCK_SIZE; j = j + 8)
        {
            uint64_t k = i * HASH_BLOCK_SIZE + j;
            M[j / 8] = ((uint64_t)X[k] << 56) | ((uint64_t)X[k + 1] << 48) | ((uint64_t)X[k + 2] << 40) | ((uint64_t)X[k + 3] << 32)
                | ((uint64_t)X[k + 4] << 24) | ((uint64_t)X[k + 5] << 16) | ((uint64_t)X[k + 6] << 8) | (uint64_t)X[k + 7];
        }
  
        /*  W[t]=M[t]; t:[0,15] */
        for (t = 0; t <= 15; t++)
        {
            W[t] = M[t];
        }
        /*  W[t] = sigma1(W[t - 2]) + W[t - 7] + sigma0(W[t - 15]) + W[t - 16]; t:[16,63] */
        for (t = 16; t < HASH_ROUND_NUM; t++)
        {
            W[t] = sigma1(W[t - 2]) + W[t - 7] + sigma0(W[t - 15]) + W[t - 16];
        }
  
        uint64_t A = H0;
        uint64_t B = H1;
        uint64_t C = H2;
        uint64_t D = H3;
        uint64_t E = H4;
        uint64_t F = H5;
        uint64_t G = H6;
        uint64_t H = H7;
  
        for (t = 0; t < HASH_ROUND_NUM; t++)
        {
            uint64_t T1 = H + SIGMA1(E) + Ch(E, F, G) + SHA384_K[t] + W[t];
            uint64_t T2 = SIGMA0(A) + Maj(A, B, C);
            H = G;
            G = F;
            F = E;
            E = D + T1;
            D = C;
            C = B;
            B = A;
            A = T1 + T2;
        }
  
        H0 = H0 + A;
        H1 = H1 + B;
        H2 = H2 + C;
        H3 = H3 + D;
        H4 = H4 + E;
        H5 = H5 + F;
        H6 = H6 + G;
        H7 = H7 + H;
    }
  
    // step 5: 输出
    uint64_t* pOut = (void*)(uint8_t*)out;
    pOut[0] = __bswap_64(H0);
    pOut[1] = __bswap_64(H1);
    pOut[2] = __bswap_64(H2);
    pOut[3] = __bswap_64(H3);
    pOut[4] = __bswap_64(H4);
    pOut[5] = __bswap_64(H5);
    free(X);

    return 0;
}

void SHA384(SHA384_t *task)
{
    const unsigned char *content = (void*)task->content;
    unsigned char result[48];
    sha2_384(result, content, strlen((void*)content));

    for (int i=0;i<48;i++)
        sprintf((char*)&task->result[i*2], "%02X", result[i]);
}