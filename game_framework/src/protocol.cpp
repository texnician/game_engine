#include "define.h"
#if defined(OS_WINDOWS)
#include <Winsock2.h>
#define STRNCPY strncpy
#else
#include <sys/socket.h>
#define STRNCPY strncpy
#endif
#include <string.h>
#include "log.h"
#include "protocol.h"


static size_t EncodeTestReq(int id, void *in_data, char *out_buf,
                            size_t out_buf_len)
{
    PKG_TEST_REQ& req = *(PKG_TEST_REQ*)in_data;
    size_t encode_size = 0;
    char *wr_ptr = out_buf;
    
    STRNCPY(wr_ptr, req.name, out_buf_len);
    encode_size += strlen(wr_ptr) + 1;
    wr_ptr = out_buf + encode_size;
    out_buf_len -= encode_size;

    int ntype = htonl(req.type);
    memcpy(wr_ptr, &ntype, sizeof(ntype));
    encode_size += sizeof(ntype);
    wr_ptr += sizeof(ntype);
    out_buf_len -= encode_size;
    
    return encode_size;
}

static size_t DecodeTestAck(int id, char *in_buf, size_t in_buf_len,
                            void *out_data)
{
    PKG_TEST_ACK& ack = *(PKG_TEST_ACK*)(out_data);
    size_t decode_size = 0;
    char *rd_ptr = in_buf;

    int nerr_code = *(int*)rd_ptr;
    ack.err_code = ntohl(nerr_code);
    decode_size += sizeof(ack.err_code);
    rd_ptr += sizeof(ack.err_code);

    short nnum = *(short*)rd_ptr;
    ack.num = ntohs(nnum);
    decode_size += sizeof(ack.num);
    rd_ptr += sizeof(ack.num);

    for (short i = 0; i < ack.num; ++i) {
        ROLE_INFO& info = *(ROLE_INFO*)(rd_ptr);
        STRNCPY(ack.info_list[i].title, rd_ptr, MAX_TITILE_LEN);
        size_t str_len = strlen(ack.info_list[i].title) + 1;
        decode_size += str_len;
        rd_ptr += str_len;
        unsigned short nlevel = *(unsigned short*)rd_ptr;
        ack.info_list[i].level = ntohs(nlevel);
        decode_size += sizeof(nlevel);
        rd_ptr += sizeof(nlevel);
    }

    return decode_size;
}

Protocol::Protocol()
{
    memset(encode_func_table_, 0, MAX_MSG);
    memset(decode_func_table_, 0, MAX_MSG);
}

bool Protocol::Init()
{
    encode_func_table_[TEST_REQ] = EncodeTestReq;
    decode_func_table_[TEST_ACK] = DecodeTestAck;
return true;	
}

size_t Protocol::Encode(int id, void *in_data, char *out_buf, size_t out_buf_len)
{
    if (id >= 0 && id < MAX_MSG) {
        EncodeFunc func = encode_func_table_[id];
        if (func) {
            return (*func)(id, in_data, out_buf, out_buf_len);
        }
        else {
            LOG(L_ERROR, "Can't find Encode function for msg %d", id);
            return -1;
        }
    }
    else {
        LOG(L_ERROR, "Invalid message id %d", id);
        return -1;
    }
}

size_t Protocol::Decode(int id, char *in_buf, size_t in_buf_len, void *out_data)
{
    if (id >= 0 && id < MAX_MSG) {
        DecodeFunc func = decode_func_table_[id];
        if (func) {
            return (*func)(id, in_buf, in_buf_len, out_data);
        }
        else {
            LOG(L_ERROR, "Can't find Decode function for msg %d", id);
            return -1;
        }
    }
    else {
        LOG(L_ERROR, "Invalid message id %d", id);
        return -1;
    }
}

size_t ProtocolEncode(Protocol& ptcl, int id, void *in_data, char *out_buf, size_t out_buf_len)
{
    return ptcl.Encode(id, in_data, out_buf, out_buf_len);
}

size_t ProtocolDecode(Protocol& ptcl, int id, char *in_buf, size_t in_buf_len, void *out_data)
{
    return ptcl.Decode(id, in_buf, in_buf_len, out_data);
}
