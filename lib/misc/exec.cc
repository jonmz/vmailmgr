// Copyright (C) 1999,2000 Bruce Guenter <bruceg@em.ca>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <config.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "exec.h"
#include "utoa.h"
#include "config/configrc.h"

int setenv(const char* prefix, const mystring& value)
{
  mystring tmp = prefix + value;
  return putenv(tmp.c_str());
}

static int setenv(const char* prefix, unsigned value)
{
  return setenv(prefix, utoa(value));
}

void setenv(const vpwentry* vpw)
{
  setenv("VUSER=", vpw->name);
  setenv("MAILDIR=", vpw->mailbox);
  setenv("VUSER_CTIME=", vpw->ctime);
  setenv("VUSER_EXPIRY=", vpw->expiry);
  setenv("VUSER_MSGCOUNT=", vpw->msgcount);
  setenv("VUSER_MSGSIZE=", vpw->msgsize);
  setenv("VUSER_PERSONAL=", vpw->personal);
  setenv("VUSER_HARDQUOTA=", vpw->hardquota);
  setenv("VUSER_SOFTQUOTA=", vpw->softquota);
  //for(mystring_iter iter(vpw->data.str(), '\0'); iter; ++iter)
  //  setenv("VUSER_", *iter);
}

int execute_one(const char* args[])
{
  int pid = fork();
  switch(pid) {
  case -1:
    return -1;
  case 0:
    execvp(args[0], (char**)args);
    exit(127);
  default:
    for(;;) {
      int status;
      if(waitpid(pid, &status, 0) == -1) {
	if(errno != EINTR)
	  return -1;
      } else
	return status;
    }
  }
  return 0;
}
 
static int execute(const configuration* node, const mystring& name)
{
  if(node->parent) {
    int i = execute(node->parent, name);
    if(i || (node->parent->directory == node->directory))
      return i;
  }
  mystring path = node->directory + "/" + name;
  struct stat buf;
  if(stat(path.c_str(), &buf) != -1 && buf.st_mode & 0111) {
    const char* args[] = { path.c_str(), 0 };
    return execute_one(args);
  }
  else
    return 0;
}
  
int execute(const mystring& name)
{
  int i = execute(config, name);
  return (i == 99) ? 0 : i;
}
