#include <HDmap.hpp>
#include <errno.h>
#include <unistd.h>

bool HDmap::getMAP(struct connectInfo &conInfo, char *data)
{
  if(read(conInfo.sHandler, data, 65534))
  {
    //TODO, BASE64 decode map file: https://blog.csdn.net/qq0824/article/details/74910068
    return true;
  }
  else
  {
    printf("can not read msg body, errno: %d\n", errno);
    return false;
  }
}
