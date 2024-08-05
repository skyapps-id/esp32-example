#ifndef HELPER_H
#define HELPER_H

#include "helper.h"

char* build_html(const char* template_html, const char* placeholder, const char* replacement);
void url_decode(char *dst, const char *src);

#endif // HELPER_H
