/*!
  \file eit_parse.co
  \author Andreas Weber
  \changelog 6.1.2012 aw: first version

  Gibt die Informationen in einer DreamBox 7025+ (vielleicht auch andere)
  .eit Datei als Text aus.
  Informationen aus en_300468v011201o.pdf
  (Digital Video Broadcasting (DVB); Specification for Service Information (SI) in DVB systems)
  von http://de.wikipedia.org/wiki/Event_Information_Table verlinkt.

  In die Datei wird der Block
  
  event_id
  start_time
  duration
  running_status
  free_CA_mode
  descriptors_loop_length
  for(i=0;i<N;i++)
    {
     descriptor()
    }

  geschrieben.

  event_id        :2 Byte identification number
  start_date      :2 Byte Datum in MJD, siehe Annex C
  start_time      :3 Byte BCD 0x173700 -> 17:37:00
  duration        :3 Byte BCD 0x014530 -> 01:45:30
  running_status  :3 Bit  1=not running, 2=starts in a few seconds, 3=pausing,
                          4=running, 5=service off-air, alles andere undefined oder reserved
  free_CA_mode    :1 Bit 0=free, 1=scrambled
  descriptors_len :12 Bit total length in bytes
  
  descriptor, Kapitel 6.1
  descriptor_tag  :1 Byte, z.B. 0x4D short_event_descriptor
  descriptor_lengt:1 Byte Länge folgende Daten

  http://www.dream-multimedia-tv.de/board/index.php?page=Thread&threadID=11100
*/


#include <stdio.h>
#include <stdlib.h>
#include "parse_eit.h"

int main (int argc, char *argv[])
{
  FILE *fp;
  fp=fopen("samples/simpsons.eit","rb");
  if(!fp)
  {
    fprintf(stderr,"error opening file\n"); 
    exit(-1);
    
  }
  eit_event_t eit;
  size_t num=fread((void*)&eit,sizeof(eit_event_t),1,fp);
  
  //hm, Bytefolge wohl big-endian in der Datei
  printf("event_id %i\n",eit.event_id_hi*256+eit.event_id_lo);

  //Duration als BCD, noch in int konvertieren
  printf("duration %i:%i:%i\n",eit.duration_1,eit.duration_2,eit.duration_3);

  printf ("running_status %i\n", eit.running_status);
  printf ("free_CA_mode %i\n", eit.free_CA_mode);
  printf ("descriptors_loop_length_hi %i\n", eit.descriptors_loop_length_hi);
  printf ("descriptors_loop_length_lo %i\n", eit.descriptors_loop_length_lo);

  fclose(fp);
  
  return 0;
}
  

