#include "adsCrypt.h"
#include <string>
#include "sha256.h"
#include "CSysUtils.h"
#include "Base64.h"

using namespace std;

void swap(unsigned char *s, unsigned int i, unsigned int j)  
{   
    unsigned char temp = s[i];  
    s[i] = s[j];  
    s[j] = temp; 
}   

void rc4_init(unsigned char *s, unsigned char *key,unsigned long Len) 
{   
    int i =0, j = 0, k[256] = {0};  
    for(i=0;i<256;i++)  
    {    
        s[i]=i;    
        k[i]=key[i%Len];  
    }   

    for (i=0; i<256; i++)  
    {    
        j=(j+s[i]+k[i])%256;   
        swap(s,i,j);  
    } 
}  

void rc4_crypt(unsigned char *s,unsigned char *Data, unsigned long Len) 
{   
    int x=0,y=0,t=0;  
    unsigned long i=0;  
    for(i=0;i<Len;i++)  
    {
        x=(x+1)%256;  
        y=(y+s[x])%256;  
        t=(s[x]+s[y])%256;  
        swap(s,x,y);  
        Data[i] ^= s[t]; 
    } 
}



static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';

    }

    return ret;

}

std::string base64_decode_(std::string const& encoded_string) 
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
    ret.reserve(encoded_string.length());

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;

        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

string adsEncryptStr( const string &data,const char* key )
{
    string shashkey = sha256(string(key));
    unsigned char *en = (unsigned char*)malloc(data.length());
    memcpy(en,data.c_str(),data.length());
    unsigned char *k = (unsigned char*)malloc(shashkey.length());
    memcpy(k,shashkey.c_str(),shashkey.length());
    unsigned char sbox[256];
    memset(sbox,0,256);
    rc4_init(sbox,k,shashkey.length());
    rc4_crypt(sbox,en,data.length());
    return base64_encode(en,data.length());
}

string adsDecryptStr( const string &data,const char* key )
{    
    string strde64 = base64_decode_(data);
    string shashkey = sha256(string(key));
    unsigned char *en = (unsigned char*)malloc(strde64.length());
    if(!en)
        return "";
    ::memcpy(en,strde64.c_str(),strde64.length());
    unsigned char *k = (unsigned char*)malloc(shashkey.length());
    memcpy(k,shashkey.c_str(),shashkey.length());
    unsigned char sbox[256];
    ::memset(sbox,0,256);
    rc4_init(sbox,k,shashkey.length());
    rc4_crypt(sbox,en,strde64.length());
    string ret((char*)en,strde64.length());    
    free(en);
    return ret;
}

