/*!
  \file eit_parse.c
  \author Andreas Weber

  tool for parsing EIT (DVB Event Information Table) files
  Copyright (C) 2016..2020 Andreas Weber

  Gibt die Informationen ine einer DreamBox 7025+ (vielleicht auch andere)
  .eit Datei als Text aus.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>
#include <inttypes.h>
#include <assert.h>

struct s_duration
{
  int hour;
  int minute;
  int second;
};

/*
  5.2.4 Event Information Table (EIT) : Seite 35

  duration: A 24-bit field containing the duration of the event in hours, minutes, seconds. format: 6 digits,
  4-bit BCD = 24 bit.

  EXAMPLE 3:
    01:45:30 is coded as "0x014530".
*/

uint8_t parse_duration (const uint8_t *p, size_t len, struct s_duration *s)
{
  if (len < 3)
    return 0;

  s->hour   = (p[0] >> 4) * 10 + (p[0] & 0x0F);
  s->minute = (p[1] >> 4) * 10 + (p[1] & 0x0F);
  s->second = (p[2] >> 4) * 10 + (p[2] & 0x0F);
  return 3;
}

struct s_start_time
{
  int Y;
  int D;
  int M;

  struct s_duration t;   // ist eigentlich die Startzeit, hat aber gleiches Format wie duration
};

/*
  5.2.4 Event Information Table (EIT) : Seite 35

  start_time: This 40-bit field contains the start time of the event in Universal Time, Co-ordinated (UTC) and Modified
    Julian Date (MJD) (see annex C). This field is coded as 16 bits giving the 16 LSBs of MJD followed by 24 bits coded as
    6 digits in 4-bit Binary Coded Decimal (BCD). If the start time is undefined (e.g. for an event in a NVOD reference
    service) all bits of the field are set to "1".

  EXAMPLE 1:
    93/10/13 12:45:00 is coded as "0xC079124500".
*/

uint8_t parse_start_time (const uint8_t *p, size_t len, struct s_start_time *s)
{
  if (len < 5)
    return 0;

  // EXAMPLE 1 from above
  //p[0] = 0xC0;
  //p[1] = 0x79;

  // Seite 145: Annex C
  int MJD = p[0] << 8 | p[1];

  int Ys = (MJD - 15078.2) / 365.25;
  int tmp = (Ys * 365.25);
  int Ms = (MJD - 14956.1 - tmp) / 30.6001;
  int tmp1 = (Ys * 365.25);
  int tmp2 = (Ms * 30.6001);
  s->D = MJD - 14956 - tmp1 - tmp2;
  int K = (Ms == 14 || Ms == 15)? 1 : 0;
  s->Y = Ys + K;
  s->M = Ms - 1 - K * 12;

  //printf ("MJD = %i\n");
  //printf ("Y/M/D = %i/%i/%i\n", s->Y, s->M, s->D);

  parse_duration (p + 2, len - 2, &s->t);
  //printf ("%02i:%02i:%02i\n", s->t.hour, s->t.minute, s->t.second);

  return 5;
}

void dump (const uint8_t *p, size_t len)
{
  for (unsigned int k = 0; k < len; ++k)
    printf ("%3i : 0x%02x  %3i '%c'\n", k, p[k], p[k], p[k]);
}

void dump_text (uint8_t *p, size_t len)
{
  //uint8_t * const p_orig = p;
  // Annex A, Seite 130

  // A.2 If the first byte of the text field has a value in the range "0x20" to "0xFF"
  // then this and all subsequent bytes in the text item are coded using
  // the default character coding table (table 00 - Latin alphabet)
  const char *code_table = "ISO-8859-1";
  uint8_t first_byte_value = *p;
  printf ("first_byte_value = 0x%02x\n", first_byte_value);
  if (first_byte_value < 0x20)
    {
      p++;
      len--;

      switch (first_byte_value)
        {
          case 0x01:
            code_table = "ISO-8859-5";
            break;
          case 0x02:
            code_table = "ISO-8859-6";
            break;
          case 0x03:
            code_table = "ISO-8859-7";
            break;
          case 0x04:
            code_table = "ISO-8859-8";
            break;
          case 0x05:
            code_table = "ISO-8859-9";
            break;
          case 0x06:
            code_table = "ISO-8859-10";
            break;
          case 0x07:
            code_table = "ISO-8859-11";
            break;
          case 0x09:
            code_table = "ISO-8859-13";
            break;
          case 0x0A:
            code_table = "ISO-8859-14";
            break;
          case 0x0B:
            code_table = "ISO-8859-15";
            break;
          case 0x11:
            code_table = "ISO-10646";
            break;
          case 0x13:
            code_table = "GB2312";
            break;
          case 0x15:
            code_table = "ISO-10646/UTF8";
            break;
          default:
            break;
        }

      if (first_byte_value == 0x10) // dynamically selected part of ISO/IEC 8859
        {
          uint8_t second_byte_value = *(p++);
          assert (second_byte_value == 0x00);  // Table A.4

          uint8_t third_byte_value = *(p++);
          len -= 2;

          switch (third_byte_value)
            {
              case 0x01:
                code_table = "ISO-8859-1";
                break;
              case 0x02:
                code_table = "ISO-8859-2";
                break;
              case 0x03:
                code_table = "ISO-8859-3";
                break;
              case 0x04:
                code_table = "ISO-8859-4";
                break;
              case 0x05:
                code_table = "ISO-8859-5";
                break;
              case 0x06:
                code_table = "ISO-8859-6";
                break;
              case 0x07:
                code_table = "ISO-8859-7";
                break;
              case 0x08:
                code_table = "ISO-8859-8";
                break;
              case 0x09:
                code_table = "ISO-8859-9";
                break;
              case 0x0A:
                code_table = "ISO-8859-10";
                break;
              case 0x0B:
                code_table = "ISO-8859-11";
                break;
              case 0x0D:
                code_table = "ISO-8859-13";
                break;
              case 0x0E:
                code_table = "ISO-8859-14";
                break;
              case 0x0F:
                code_table = "ISO-8859-15";
                break;
              default:
                break;
            }
        }
    }


  printf ("xcode_table = '%s'\n", code_table);

  iconv_t cd;

  cd = iconv_open ("UTF−8", code_table);
  if (cd == (iconv_t) -1)
    {
      fprintf (stderr, "iconv_open failed: %i = '%s'\n", errno, strerror (errno));
      exit (-1);
    }

  size_t outbytesleft = 1200;
  char *outbuf = (char *) malloc (outbytesleft);

  char *pout = outbuf;
  char *pin = p;
  iconv (cd, NULL, NULL, &pout, &outbytesleft);
  size_t nconv = iconv (cd, &pin, &len, &pout, &outbytesleft);

  *pout = 0;
  printf ("nconv = %zi\n", nconv);
  printf ("outbuf = '%s'\n", outbuf);

  assert (nconv == 0);

  printf ("---");

  // FIXME: Die cr/lf ersetzung sollte man besser vorher machen
  // andererseits steht da, das wäre die utf-8 sequence...

  // wtf? 0xC28A ist wohl CR/LF, Seite 130 Annex A

  //~ for (unsigned int k = 0; k < (len - (code_table > 0)); ++k)
    //~ {
      //~ // FIMXE: check for buffer end
      //~ if (   p[k] == 0xC2
          //~ && k +1 < (len - (code_table > 0))
          //~ && p[k+1] == 0x8A)
        //~ k++; //ignore CR/LF
      //~ else
        //~ printf ("%c", p[k]);
    //~ }

  printf ("---\n");

  if (iconv_close (cd) != 0)
    perror ("iconv_close");

  free (outbuf);
}

int main (int argc, char *argv[])
{
  if (argc < 2)
    {
      fprintf (stderr, "ERROR: No input file...\n\nUSAGE: %s EIT\n", argv[0]);
      exit (-1);
    }

  FILE *fp;
  for (int k = 1; k < argc; ++k)
    {
      const char *fn = argv[k];

      fp = fopen (fn, "rb");
      if (!fp)
      {
        fprintf (stderr, "error opening file %s\n", fn);
        exit(-1);
      }

      #define BUF_SIZE 2000
      uint8_t buf[BUF_SIZE];
      size_t num = fread (buf, 1, BUF_SIZE, fp);
      printf ("num (size of buffer) = %zu\n", num);

      if (num == BUF_SIZE)
        {
          fprintf (stderr, "ERROR: Buffer zu klein...\n");
          exit (-1);
        }

      fclose(fp);

      uint8_t *p = buf;

      // 5.2.4 Event Information Table (EIT), Seite 35:
      int event_id = p[0] << 8 | p[1];
      printf ("event_id = %i\n", event_id);
      p += 2;

      struct s_start_time st;
      uint8_t r = parse_start_time (p, num - (p - buf), &st);
      printf ("start_time = %i/%i/%i %02i:%02i:%02i\n", st.Y, st.M, st.D, st.t.hour, st.t.minute, st.t.second);
      p += r;

      struct s_duration dur;
      r = parse_duration (p, num - (p - buf), &dur);
      printf ("duration = %02i:%02i:%02i\n", dur.hour, dur.minute, dur.second);
      p += r;

      // running_status
      // undefined = 0, not_running, starts_in_a_few_seconds, pausing, running, serive_off_air, reserved1, reserved2

      uint8_t running_status = p[0] & 0x03;
      printf ("running_status = %i\n", running_status);

      uint8_t free_CA_mode   = (p[0] >> 3) & 0x01;
      printf ("free_CA_mode = %i\n", free_CA_mode);

      uint16_t descriptors_loop_length = (p[0] & 0xF0) << 8 | p[1];
      printf ("descriptors_loop_length = %i\n", descriptors_loop_length);

      p += 2;

      // Seite 39, Tabelle 12
      #define SHORT_EVENT_DESCRIPTOR 0x4d
      #define EXTENDED_EVENT_DESCRIPTOR 0x4e
      #define COMPONENT_DESCRIPTOR 0x50

      while (p < (buf + num))
      {
        printf ("Bytes left: %li\n", buf + num - p);

        uint8_t descriptor_tag = p[0];
        printf ("descriptor_tag = %#x\n", descriptor_tag);

        uint8_t descriptor_length = p[1];
        printf ("descriptor_length = %i\n", descriptor_length);  // Länge der folgenden Daten in Bytes

        p += 2;

        // Seite 87, Kapitel 6.2.37 : Short event descriptor
        if (descriptor_tag == SHORT_EVENT_DESCRIPTOR)
          {
            printf ("SHORT_EVENT_DESCRIPTOR\n");
            printf ("iso_639_2_language_code = \"%c%c%c\"\n", p[0], p[1], p[2]);
            p += 3;

            uint8_t event_name_length = p[0];
            printf ("event_name_length = %i\n", event_name_length);
            p += 1;

            dump_text (p, event_name_length);
            p += event_name_length;

            uint8_t text_length = p[0];
            printf ("text_length = %i\n", text_length);
            p += 1;

            dump_text (p, text_length);
            p += text_length;
          }
        // Seite 64, Kapitel 6.2.15 : Extended event descriptor
        else if (descriptor_tag == EXTENDED_EVENT_DESCRIPTOR)
          {
            printf ("EXTENDED_EVENT_DESCRIPTOR\n");
            uint8_t descriptor_number = p[0] >> 4;
            uint8_t last_descriptor_number = p[0] & 0x0F;
            p += 1;

            printf ("descriptor_number = %i\n", descriptor_number);
            printf ("last_descriptor_number = %i\n", last_descriptor_number);
            printf ("iso_639_2_language_code = \"%c%c%c\"\n", p[0], p[1], p[2]);
            p += 3;

            // Tabelle 53, Seite 64
            uint8_t length_of_items = *(p++);   // kann auch 0 sein
            printf ("length_of_items = %i\n", length_of_items);

            if (length_of_items > 0)
              {
                fprintf (stderr, "Noch nicht implementiert...\n");
                exit (-1);
              }

            uint8_t text_length = *(p++);
            printf ("text_length = %i\n", text_length);

            dump_text (p, text_length);
            p += text_length;
          }
        // Seite 46, Kapitel 6.2.8
        else if (descriptor_tag == COMPONENT_DESCRIPTOR)
          {
            printf ("COMPONENT_DESCRIPTOR\n");
            uint8_t stream_content_ext = p[0] >> 4;
            uint8_t stream_content = p[0] & 0x0F;
            uint8_t component_type = p[1];
            uint8_t component_tag = p[2];

            printf ("stream_content_ext = %i\n", stream_content_ext);
            printf ("stream_content = %i\n", stream_content);
            printf ("component_type = %i\n", component_type);
            printf ("component_tag = %i\n", component_tag);
            printf ("iso_639_2_language_code = \"%c%c%c\"\n", p[3], p[4], p[5]);
            p += 6;

            // hier keine Länge, muss man sich wohl aus descriptor_length berechnen
            size_t len = descriptor_length - 6;
            printf ("len = %zu\n", len);
            dump_text (p, len);
            p += len;

          }
        else
          {
            fprintf (stderr, "Unbekannter descriptor_tag %#x\n", descriptor_tag);
            exit (-1);
          }

      }
      printf ("End: Bytes left: %li\n", buf + num - p);
  }

  return 0;
}


