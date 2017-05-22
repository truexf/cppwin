#ifndef ADSCRYPT_H
#define ADSCRYPT_H

#include <string>
using std::string;
string adsEncryptStr(const string &data,const char* key);
string adsDecryptStr(const string &data,const char* key);


#endif