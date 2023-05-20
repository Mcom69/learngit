#include "protocol.h"

/**
 * 申请弹性内存空间
 * @brief mkPDU
 * @param uiMsgLen
 * @return
 */
PDU *mkPDU(uint uiMsgLen)
{
    // 前面固定长度加上需要申请的弹性内存空间
    uint uiPDULen = sizeof(PDU)+uiMsgLen;
    PDU *pdu =(PDU*)malloc(uiPDULen);
    if(NULL == pdu)
    {
        exit(EXIT_FAILURE);
    }
    memset(pdu,0,uiPDULen);
    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}
