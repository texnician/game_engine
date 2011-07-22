#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define TEST_REQ 0
#define TEST_ACK 1

#define MAX_MSG 512

#define MAX_NAME_LEN 128
#define MAX_REQ_DATA_LEN 512
#define MAX_TITILE_LEN 128
#define MAX_INFO_NUM 10

struct PKG_TEST_REQ
{
    char name[MAX_NAME_LEN];
    int type;
};

struct ROLE_INFO
{
    char title[MAX_TITILE_LEN];
    unsigned short level;
};

struct PKG_TEST_ACK
{
    int err_code;
    short num;
    ROLE_INFO info_list[MAX_INFO_NUM];
};

typedef size_t (*EncodeFunc)(int id, void *in_data, char *out_buf,
                              size_t out_buf_len);
typedef size_t (*DecodeFunc)(int id, char *in_buf, size_t in_buf_len,
                             void *out_data);

class Protocol
{
public:
    Protocol();

    bool Init();
    size_t Encode(int id, void *in_data, char *out_buf, size_t out_buf_len);
    size_t Decode(int id, char *in_buf, size_t in_buf_len, void *out_data);

private:
    EncodeFunc encode_func_table_[MAX_MSG];
    DecodeFunc decode_func_table_[MAX_MSG];
};

size_t ProtocolEncode(Protocol& ptcl, int id, void *in_data, char *out_buf, size_t out_buf_len);
size_t ProtocolDecode(Protocol& ptcl, int id, char *in_buf, size_t in_buf_len, void *out_data);

#endif //_PROTOCOL_H_
