//todo: 容錯能力
#include "Base64.h"  
#include <stdlib.h>
#include <string>
using std::string;


static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
static const std::string base64_chars_fiturl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-=";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}



std::string base64_encode1(unsigned char const* bytes_to_encode, unsigned int in_len,bool fit_url) 
{
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
		  if(fit_url)
			ret += base64_chars_fiturl[char_array_4[i]];
		  else
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
		if(fit_url)
			ret += base64_chars_fiturl[char_array_4[j]];
		else
			ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

bool base64_encode(const unsigned char *in_str,size_t in_len,unsigned char **out_str,size_t &out_len)
{
  string ret = base64_encode1(in_str,in_len);
  *out_str = (unsigned char *)malloc(ret.length());
  memcpy(*out_str,ret.c_str(),ret.length());
  out_len = ret.length();
  return true;
}
  
const char tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefehijklmnopqrstuvwxyz0123456789+/=";

static const unsigned char g_pMap[256] =  
{  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  
  52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,  
  255, 0, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,  
  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  
  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  
  255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  
  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
  49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
  255, 255, 255, 255  
};  

//bool base64_encode(const unsigned char *in_str,size_t in_len,unsigned char **out_str,size_t &out_len)
//{
//  if (!in_str || !in_len || !out_str)
//    return false;
//  out_len = in_len / 3 * 4;
//  if (in_len % 3)
//    out_len+=4;    
//  unsigned char *outarr = (unsigned char*)malloc(out_len);;
//  size_t i1=0,i2=1,i3=2,j1=0,j2=1,j3=2,j4=3;  
//  while (1)
//  {
//    if (i3 < in_len)
//    {    
//      outarr[j1] = in_str[i1] >>2;      
//      outarr[j2] = in_str[i1] << 6;
//      outarr[j2] >>= 2;      
//      outarr[j2] |= (in_str[i2] >> 4);            
//      outarr[j3] = in_str[i2] << 4;
//      outarr[j3] >>= 2;      
//      outarr[j3] |= (in_str[i3] >> 6);
//      outarr[j4] = in_str[i3] << 2;
//      outarr[j4] >>= 2;
//    }
//    else if (i2 < in_len)
//    {
//      outarr[j1] = in_str[i1] >>2;      
//      outarr[j2] = in_str[i1] << 6;
//      outarr[j2] >>= 2;      
//      outarr[j2] |= (in_str[i2] >> 4);            
//      outarr[j3] = in_str[i2] << 4;
//      outarr[j3] >>= 2;      
//      outarr[j4] = 64;
//    }
//    else if (i1 < in_len)
//    {
//      outarr[j1] = in_str[i1] >>2;      
//      outarr[j2] = in_str[i1] << 6;
//      outarr[j2] >>= 2;      
//      outarr[j3] = 64;
//      outarr[j4] = 64;
//    }
//    else
//      break;
//    outarr[j1] = tbl[outarr[j1]];
//    outarr[j2] = tbl[outarr[j2]];
//    outarr[j3] = tbl[outarr[j3]];
//    outarr[j4] = tbl[outarr[j4]];
//    i1+=3;
//    i2+=3;
//    i3+=3;
//    j1+=4;
//    j2+=4;
//    j3+=4;
//    j4+=4;
//  }
//
//  *out_str = outarr;
//  return true;
//}

inline unsigned char get_sn(unsigned char c)
{
  return g_pMap[c];
}
bool base64_decode(const unsigned char *in_str,size_t in_len,unsigned char **out_str,size_t &out_len)
{
  if (!in_str || !in_len || !out_str)
    return false;
  for(size_t i = 0; i!=in_len;++i)
    if (255 == g_pMap[in_str[i]])
      return false; //出现非法字符
  out_len = in_len / 4 * 3;
  unsigned char *outarr = (unsigned char*)malloc(out_len);
  size_t i1=0,i2=1,i3=2,j1=0,j2=1,j3=2,j4=3;  
  unsigned char c1,c2,c3,c4,d1,d2,d3,t;
  while(1)
  {    
    if (j4 < in_len)
    {    
      c1 = get_sn(in_str[j1]);      
      c2 = get_sn(in_str[j2]);
      c3 = get_sn(in_str[j3]);
      c4 = get_sn(in_str[j4]);    
    }
    else if (j3 < in_len)
    {
      c1 = get_sn(in_str[j1]);
      c2 = get_sn(in_str[j2]);
      c3 = get_sn(in_str[j3]);
      c4 = 0;//get_sn(in_str[j4]);    
    }
    else if (j2 < in_len)
    {
      c1 = get_sn(in_str[j1]);
      c2 = get_sn(in_str[j2]);
      c3 = 0;//get_sn(in_str[j3]);
      c4 = 0;//get_sn(in_str[j4]);    
    }
    else if (j1 < in_len)
    {
      c1 = get_sn(in_str[j1]);
      c2 = 0;//get_sn(in_str[j2]);
      c3 = 0;//get_sn(in_str[j3]);
      c4 = 0;//get_sn(in_str[j4]);    
    }
    else
      break;

    d1 = c1 << 2; 
    t = c2 << 2;
    t >>= 6;
    d1 |= t;
    d2 = c2 << 4;
    t = c3 >> 2;    
    d2 |= t;
    d3 = c3 << 6;    
    d3 |= c4;

    outarr[i1] = d1;
    outarr[i2] = d2;
    outarr[i3] = d3;

    i1 += 3;
    i2 += 3;
    i3 += 3;
    j1 += 4;
    j2 += 4;
    j3 += 4;
    j4 += 4;    
  }
  *out_str = outarr;
  if ('='==in_str[in_len-1])
  {
    --out_len;
    if ('='==in_str[in_len-2])
    {
      --out_len;
      if ('='==in_str[in_len-3])      
        --out_len;      
    }
  }

  return true;
}