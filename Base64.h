#ifndef ___BASE64_H___  
#define ___BASE64_H___  

#include <string>
using std::string;
  
  
std::string base64_encode1(unsigned char const* bytes_to_encode, unsigned int in_len,bool fit_url=false);
bool base64_encode(const unsigned char *in_str,size_t in_len,unsigned char **out_str,size_t &out_len);
bool base64_decode(const unsigned char *in_str,size_t in_len,unsigned char **out_str,size_t &out_len);

namespace HttpUtility  
{  

  typedef unsigned char BYTE;  

  inline BYTE toHex(const BYTE &x)  
  {  
    return x > 9 ? x -10 + 'A': x + '0';  
  }  

  inline BYTE fromHex(const BYTE &x)  
  {  
    return isdigit(x) ? x-'0' : x-'A'+10;  
  }  

  inline string URLEncode(const string &sIn)  
  {  
    string sOut;  
    for( size_t ix = 0; ix < sIn.size(); ix++ )  
    {        
      BYTE buf[4];  
      memset( buf, 0, 4 );  
      if( isalnum( (BYTE)sIn[ix] ) )  
      {        
        buf[0] = sIn[ix];  
      }  
      //else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以  
      //{  
      //    buf[0] = '+';  
      //}  
      else  
      {  
        buf[0] = '%';  
        buf[1] = toHex( (BYTE)sIn[ix] >> 4 );  
        buf[2] = toHex( (BYTE)sIn[ix] % 16);  
      }  
      sOut += (char *)buf;  
    }  
    return sOut;  
  };  

  inline string URLDecode(const string &sIn)  
  {  
    string sOut;  
    for( size_t ix = 0; ix < sIn.size(); ix++ )  
    {  
      BYTE ch = 0;  
      if(sIn[ix]=='%')  
      {  
        ch = (fromHex(sIn[ix+1])<<4);  
        ch |= fromHex(sIn[ix+2]);  
        ix += 2;  
      }  
      else if(sIn[ix] == '+')  
      {  
        ch = ' ';  
      }  
      else  
      {  
        ch = sIn[ix];  
      }  
      sOut += (char)ch;  
    }  
    return sOut;  
  }  
} 
  
#endif // ___BASE64_H___  