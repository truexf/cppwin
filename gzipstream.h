#ifndef GZIPSTREAM_H_
#define GZIPSTREAM_H_

bool gzip_compress(void *buf,size_t buf_size,void **buf_out,size_t &buf_out_size);
bool gzip_uncompress(void *buf,size_t buf_size,void **buf_out,size_t &buf_out_size);
void gzip_freebuf(void *buf);

#endif