// Copyright (C) 1999,2000 Bruce Guenter <bruce@untroubled.org>
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
#include "vdomain.h"
#include "misc/maildir.h"

response vdomain::set(const vpwentry* vpw, bool onlyadd)
{
  if(!vpw)
    RETURN(err, "Internal error: no vpwentry");
  if(!validate_username(vpw->name))
    RETURN(bad, "Virtual user or alias name contains invalid characters");
  if(!validate_password(vpw->pass))
    RETURN(bad, "Password field contains invalid characters");
  if(mkdirp(vpw->directory, 0700) == -1)
    RETURN(err, "Can't create the user directory '" + vpw->directory + "'");
  if(vpw->has_mailbox && !make_maildir(vpw->directory))
    RETURN(err, "Can't create the mail directory '" + vpw->directory + "'");
  if(!table()->put(vpw, onlyadd)) {
    delete_directory(vpw->directory);
    RETURN(err, "Can't add the user to the password file");
  }
  RETURN(ok, "User added successfully");
}
