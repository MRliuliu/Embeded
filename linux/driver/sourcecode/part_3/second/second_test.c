/*======================================================================
    A test program to access /dev/second
    This example is to help understand kernel timer 
    
    The initial developer of the original code is Baohua Song
    <author@linuxdriver.cn>. All Rights Reserved.
======================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

int main()
{
  int fd;
  int counter = 0;
  int old_counter = 0;
  
  /*��/dev/second�豸�ļ�*/
  fd = open("/dev/second", O_RDONLY);
  if (fd < 0)
  {
	  perror("fail to open");
	  exit(-1);
  }
  while (1)
  {
	  read(fd,&counter, sizeof(unsigned int));//��Ŀǰ����������
	  if(counter != old_counter)
	  {	
		  printf("seconds after open /dev/second :%d\n",counter);
		  old_counter = counter;
	  }	
  }    
  
  return 0;
}
