/*!
    tool for parsing EIT (DVB Event Information Table) files
    Copyright (C) <2016>  <Andreas Weber>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   
*/
#ifndef PARSE_EIT_H
#define PARSE_EIT_H

typedef struct eit_event_struct {
	u_char	event_id_hi			: 8;
	u_char	event_id_lo			: 8;
	
	u_char	start_time_1			: 8;
	u_char	start_time_2			: 8;
	u_char	start_time_3			: 8;
	u_char	start_time_4			: 8;
	u_char	start_time_5			: 8;

	u_char	duration_1			: 8;
	u_char	duration_2			: 8;
	u_char	duration_3			: 8;

#if BYTE_ORDER == BIG_ENDIAN
	u_char	running_status			: 3;
	u_char	free_CA_mode			: 1;
	u_char	descriptors_loop_length_hi	: 4;
#else
	u_char	descriptors_loop_length_hi	: 4;
	u_char	free_CA_mode			: 1;
	u_char	running_status			: 3;
#endif

	u_char	descriptors_loop_length_lo	: 8;
	
} eit_event_t;
#define EIT_LOOP_SIZE 12

#define EIT_SHORT_EVENT_DESCRIPTOR 0x4d
#define EIT_SHORT_EVENT_DESCRIPTOR_SIZE 6

struct eit_short_event_descriptor_struct {
	u_char	descriptor_tag			: 8;
	u_char	descriptor_length		: 8;
	
	u_char	language_code_1			: 8;
	u_char	language_code_2			: 8;
	u_char	language_code_3			: 8;

	u_char	event_name_length		: 8;
};
	
#define EIT_EXTENDED_EVENT_DESCRIPTOR 0x4e

struct eit_extended_descriptor_struct {
	u_char descriptor_tag : 8;
	u_char descriptor_length : 8;
#if BYTE_ORDER == BIG_ENDIAN
	u_char descriptor_number : 4;
	u_char last_descriptor_number : 4;
#else
	u_char last_descriptor_number : 4;
	u_char descriptor_number : 4;
#endif
	u_char iso_639_2_language_code_1 : 8;
	u_char iso_639_2_language_code_2 : 8;
	u_char iso_639_2_language_code_3 : 8;
};


#endif