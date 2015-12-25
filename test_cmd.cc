#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include <string.h>

#if defined(__MAC_OSX__)
 #include <CoreServices/CoreServices.h>
#endif

#include "vxi11.h"

#define WAITLOCK_FLAG 1
#define WRITE_END_CHAR 8
#define READ_END_CHAR 0x80
#define VXI_ENDW (WAITLOCK_FLAG | WRITE_END_CHAR)

CLIENT *rpcClient      = (CLIENT *)NULL;
Create_LinkResp *crlr  = (Create_LinkResp *)NULL;
Device_WriteResp *dwrr = (Device_WriteResp *)NULL;
Device_ReadResp *drdr  = (Device_ReadResp *)NULL;
Device_Error *derr     = (Device_Error *)NULL;

Create_LinkParms crlp;
Device_WriteParms dwrp;
Device_ReadParms drdp;

/* ICS 8003 default IP address */
//static char *defaultIP = "192.168.10.1";

int main (int argc, char** argv)
{
   if (argc!=3) {
      fprintf(stderr, "Usage %s <IP> <command>\n", argv[0]);
      fprintf(stderr," e.g. %s 192.168.10.1 \"OUTPUT1:STATE ON\"\n", argv[0]);
      return -1;
   }

   char* defaultIP = argv[1];
   char* sendMessage = argv[2];
   printf("defaultIP %s\n",  defaultIP);
   printf("sendMessage %s\n", sendMessage);

   /* declare some variables and initialize them */
   char host[16];
   strcpy(host,defaultIP);

   (void) printf("8003: INFO: Sending command=\"%s\"\n",sendMessage);

   /* create a client */
   (void) printf("8003: INFO: Creating client for host=%s\n",host);
   if ( (rpcClient=clnt_create(host,DEVICE_CORE,DEVICE_CORE_VERSION,"tcp")) == (CLIENT *)NULL ) {
      (void) printf("Error creating client\n");
      clnt_pcreateerror(host);
      return 1;
   }
   (void) printf("8003: INFO: Created client for host=%s\n",host);

   /* populate structure and create link */
   crlp.clientId = (long)rpcClient;
   crlp.lockDevice = 0;
   crlp.lock_timeout = 10000;
   crlp.device = "inst0";
   (void) printf("8003: INFO: Creating link for device=%s\n",crlp.device);
   if ( (crlr=create_link_1(&crlp,rpcClient)) == (Create_LinkResp *)NULL ) {
      clnt_perror(rpcClient, host);
      return 1;
   }
   (void) printf("8003: INFO: Created link for device=%s\n",crlp.device);

   /* populate structure and write command to device */
   dwrp.lid = crlr->lid;
   dwrp.io_timeout = 1000;
   dwrp.lock_timeout = 10000;
   dwrp.flags = VXI_ENDW;
   dwrp.data.data_len = strlen(sendMessage);
   dwrp.data.data_val = sendMessage;
   (void) printf("8003: INFO: Writing to device=%s\n",crlp.device);
   if ( (dwrr=device_write_1(&dwrp,rpcClient)) == (Device_WriteResp *)NULL ) {
      clnt_perror(rpcClient, host);
      (void) printf ("device_write returned dwrr == NULL \n");
      return 1;
   }
   (void) printf("8003: INFO: Wrote to device=%s\n",crlp.device);

   /* populate structure and read device */
   drdp.lid = crlr->lid;
   drdp.io_timeout = 1000;
   drdp.lock_timeout = 10000;
   drdp.flags = VXI_ENDW;
   drdp.termChar = '\n';
   drdp.requestSize = 1024;
   (void) printf("8003: INFO: Reading from device=%s\n",crlp.device);
   if ( (drdr=device_read_1(&drdp,rpcClient)) == (Device_ReadResp *)NULL) {
      clnt_perror(rpcClient, host);
      (void) printf("device read returned drdr == NULL\n");
      return 1;
   }
   (void) printf("8003: INFO: Read from device=%s, response=>%s<\n",crlp.device,drdr->data.data_val);

   /* destroy link */
   (void) printf("8003: INFO: Destroying link for device=%s\n",crlp.device);
   derr = destroy_link_1(&(crlr->lid), rpcClient);
   (void) printf("8003: INFO: Destroyed link for device=%s\n",crlp.device);
   (void) printf("8003: INFO: Destroying client for host=%s\n",host);
   clnt_destroy(rpcClient);
   (void) printf("8003: INFO: Destroyed client for host=%s\n",host);
   return 0;
}
