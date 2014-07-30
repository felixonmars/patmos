/*
   Copyright 2014 Technical University of Denmark, DTU Compute. 
   All rights reserved.
   
   This file is part of the time-predictable VLIW processor Patmos.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

      1. Redistributions of source code must retain the above copyright notice,
         this list of conditions and the following disclaimer.

      2. Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
   NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   The views and conclusions contained in the software and documentation are
   those of the authors and should not be interpreted as representing official
   policies, either expressed or implied, of the copyright holder.
 */

/*
 * Message passing API
 * 
 * Author: Rasmus Bo Soerensen (rasmus@rbscloud.dk)
 *
 */

#include "mp.h"

////////////////////////////////////////////////////////////////////////////
// Functions for initializing the message passing API
////////////////////////////////////////////////////////////////////////////

int mp_send_init(mpd_t* mpd_ptr, int recv_id, volatile void _SPM *remote_addr,
          volatile void _SPM *local_addr, size_t buf_size, size_t num_buf) {

  // Check if the remote and local addresses are double word aligned
  if ((volatile void _SPM *)DWALIGN(remote_addr) != remote_addr ||
    (volatile void _SPM *)DWALIGN(local_addr) != local_addr) {
    return 0;
  }
  mpd_ptr->remote_addr = remote_addr;
  mpd_ptr->local_addr = local_addr;
  // Align the buffer size to double words and add the flag size
  mpd_ptr->buf_size = DWALIGN(buf_size);
  mpd_ptr->num_buf = num_buf;
  int recv_count_offset = (mpd_ptr->buf_size + FLAG_SIZE) * NUM_WRITE_BUF;
  mpd_ptr->recv_count = (volatile size_t _SPM *)((char*)local_addr + recv_count_offset);
  mpd_ptr->recv_id = recv_id;

  // Initialize send count to 0 and recv count to 0.
  mpd_ptr->send_count = 0;
  mpd_ptr->send_ptr = 0;
  mpd_ptr->write_buf = local_addr;
  mpd_ptr->shadow_write_buf = (volatile void _SPM *)((char*)local_addr + (mpd_ptr->buf_size + FLAG_SIZE));
  *(mpd_ptr->recv_count) = 0;

  // Initialize last word in buffers to FLAG_VALID.
  *(volatile int _SPM *)((char*)mpd_ptr->local_addr + mpd_ptr->buf_size) = FLAG_VALID;
  *(volatile int _SPM *)((char*)mpd_ptr->local_addr + (mpd_ptr->buf_size + FLAG_SIZE) + mpd_ptr->buf_size) = FLAG_VALID;
  
  return 1;  
}

int mp_recv_init(mpd_t* mpd_ptr, int send_id, volatile void _SPM *remote_addr,
            volatile void _SPM *local_addr, size_t buf_size, size_t num_buf) {

  // Check if the remote and local addresses are double word aligned
  if ((volatile void _SPM *)DWALIGN(remote_addr) != remote_addr ||
    (volatile void _SPM *)DWALIGN(local_addr) != local_addr) {
    return 0;
  }

  mpd_ptr->remote_addr = remote_addr;
  mpd_ptr->local_addr = local_addr;
  // Align the buffer size to double words and add the flag size
  mpd_ptr->buf_size = DWALIGN(buf_size);
  mpd_ptr->num_buf = num_buf;
  int recv_count_offset = (mpd_ptr->buf_size + FLAG_SIZE) * num_buf;
  mpd_ptr->recv_count = (volatile size_t _SPM *)((char*)local_addr + recv_count_offset);
  mpd_ptr->send_id = send_id;
  int rmt_recv_count_offset = (mpd_ptr->buf_size + FLAG_SIZE) * NUM_WRITE_BUF;
  mpd_ptr->remote_recv_count = (volatile size_t _SPM *)((char*)remote_addr + rmt_recv_count_offset);
  mpd_ptr->read_buf = local_addr;

  // Initialize recv count to 0.
  *(volatile int _SPM *)(mpd_ptr->recv_count) = 0;
  mpd_ptr->recv_ptr = 0;

  // Initialize last word in each buffer to FLAG_INVALID
  for (int i = 1; i <= num_buf; i++) {
    *(volatile int _SPM *)((char*)mpd_ptr->local_addr + mpd_ptr->buf_size * i) = FLAG_INVALID;
  }
  return 1;
}

////////////////////////////////////////////////////////////////////////////
// Functions for transmitting data
////////////////////////////////////////////////////////////////////////////

void mp_send(mpd_t* mpd_ptr) {

  // Calculate the address of the remote receiving buffer
  int rmt_addr_offset = (mpd_ptr->buf_size + FLAG_SIZE) * mpd_ptr->send_ptr;
  volatile void _SPM * calc_rmt_addr = &mpd_ptr->remote_addr[rmt_addr_offset];

  while((mpd_ptr->send_count) - *(mpd_ptr->recv_count) == mpd_ptr->num_buf) {
    /* spin until there is room in receiving buffer*/
  }
  noc_send(mpd_ptr->recv_id,calc_rmt_addr,mpd_ptr->write_buf,mpd_ptr->buf_size + FLAG_SIZE); 

  // Increment the send counter
  mpd_ptr->send_count++;

  // Move the send pointer
  if (mpd_ptr->send_ptr == mpd_ptr->num_buf-1) {
    mpd_ptr->send_ptr = 0;
  } else {
    mpd_ptr->send_ptr++;  
  }

  // Swap write_buf and shadow_write_buf
  volatile void _SPM * tmp = mpd_ptr->write_buf;
  mpd_ptr->write_buf = mpd_ptr->shadow_write_buf;
  mpd_ptr->shadow_write_buf = tmp;

  return;
}

void mp_recv(mpd_t* mpd_ptr) {

  // Calculate the address of the local receiving buffer
  int locl_addr_offset = (mpd_ptr->buf_size + FLAG_SIZE) * mpd_ptr->recv_ptr;
  volatile void _SPM * calc_locl_addr = &mpd_ptr->local_addr[locl_addr_offset];

  volatile int _SPM * recv_flag = (volatile int _SPM *)((char*)calc_locl_addr + mpd_ptr->buf_size);

  while(*recv_flag == FLAG_INVALID) {
    /* Spin until message is received */
  }

  // Increment the receive counter
  (*mpd_ptr->recv_count)++;

  // Move the receive pointer 
  if (mpd_ptr->recv_ptr == mpd_ptr->num_buf - 1) {
    mpd_ptr->recv_ptr = 0;
  } else {
    mpd_ptr->recv_ptr++;  
  }

  // Set the reception flag of the received message to FLAG_INVALID
  *recv_flag = FLAG_INVALID; 

  // Set the new read buffer pointer
  mpd_ptr->read_buf = calc_locl_addr;

  return;
}


void mp_ack(mpd_t* mpd_ptr){
  // Update the remote receive count
  noc_send(mpd_ptr->send_id,mpd_ptr->remote_recv_count,mpd_ptr->recv_count,8);
  return;
}

////////////////////////////////////////////////////////////////////////////
// Help functions 
////////////////////////////////////////////////////////////////////////////

int mp_spm_alloc_size(mpd_t* mpd_ptr) {
  return 0;
}