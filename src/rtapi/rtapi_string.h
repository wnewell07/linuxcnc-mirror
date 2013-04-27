//    Copyright 2006-2009, Jeff Epler
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#ifndef RTAPI_STRING_H
#define RTAPI_STRING_H
#ifdef __KERNEL__
#include <linux/string.h>
#define rtapi_strdup kstrdup
#define rtapi_strndup kstrndup
#define rtapi_argv_split argv_split
#define rtapi_argv_free argv_free
#else
#include <string.h>
#define rtapi_strdup(s,g) strdup(s)
#define rtapi_strndup(s,n,g) strndup(s,n)
void rtapi_argv_free(char **argv);
char **rtapi_argv_split(long gfp, const char *str, int *argcp);
#endif
#endif
