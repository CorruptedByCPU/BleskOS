;BleskOS

; not complete!
; work only with HTTP pages

internet_connection_status dd 0
last_arrived_packet dd 0

%define BIG_ENDAIN(x) ( ((x & 0xFF) << 8 ) | ((x >> 8) & 0xFF) )
%define ARP_REQUEST 1
%define ARP_REPLY 2
%define DHCP_OFFER 3
%define DHCP_ACK 4
%define DHCP_NAK 5
%define DNS_RESPONSE 6
%define TCP_HANDSHAKE 7
%define TCP_HTTP 8
%define TCP_END 9

type_of_received_packet dd 0
dns_report dd 0
http_file_pointer dd 0
%define NO_TCP 0
%define TCP_WAITING_FOR_HANDSHAKE 1
%define TCP_HANDSHAKE_RECEIVED 2
%define TCP_REQUEST_FOR_FILE 3
%define TCP_FILE_TRANSFERRING 4
%define TCP_FINALIZING 5
tcp_communication_type dd 0
tcp_file_length dd 0
tcp_file_transferred_length dd 0
tcp_last_ack_number dd 0
tcp_expected_ack_number dd 0

;Dynamic Host Control Protocol - DISCOVER message
dhcp_offer_packet:
;ethernet layer
db 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ;receiver MAC address
.ethernet_mac: db 0, 0, 0, 0, 0, 0 ;our MAC address
dw BIG_ENDAIN(0x0800) ;IP

;ip layer
.ip_layer:
db 0x45 ;header
db 0
dw BIG_ENDAIN(.dhcp_layer_end-.ip_layer) ;total length
dd 0
db 0x80 ;time to live
db 17 ;UDP protocol
.ip_checksum: dw 0 ;checksum
dd 0x00000000 ;source IP
dd 0xFFFFFFFF ;destination IP
.end_of_ip_layer:

;udp layer
dw BIG_ENDAIN(68) ;source port
dw BIG_ENDAIN(67) ;destination port
dw BIG_ENDAIN(.dhcp_layer_end-.dhcp_layer+8) ;length
.udp_crc: dw 0 ;checksum

;dhcp layer - 251 bytes
.dhcp_layer:
db 1 ;request
db 1 ;ethernet
db 6 ;hardware address length
db 0
dd 0x12121212 ;value for this transaction
dw 8 ;num of seconds
dw 0 ;flags
times 4 dd 0x00000000 ;we do not know our IP address
.dhcp_mac: times 16 db 0
times 64 db 0
times 128 db 0
db 99, 130, 83, 99 ;magic value for DHCP

db 0x35, 0x01, 0x01 ;command DHCP discover
db 0x32, 0x04, 0xC0, 0xA8, 0x01, 0x64 ;requested 192.168.1.100
db 0x37, 0x03, 0x01, 0x03, 0x06 ;parameter request list

db 0xFF
.dhcp_layer_end:

dhcp_offer_packet_end:

;Dynamic Host Control Protocol - REQUEST message
dhcp_request_packet:
;ethernet layer
db 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ;receiver MAC address
.ethernet_mac: db 0, 0, 0, 0, 0, 0 ;our MAC address
dw BIG_ENDAIN(0x0800) ;IP

;ip layer
.ip_layer:
db 0x45 ;header
db 0
dw BIG_ENDAIN(.dhcp_layer_end-.ip_layer) ;total length
dd 0
db 0x80 ;time to live
db 17 ;UDP protocol
.ip_checksum: dw 0 ;checksum
dd 0x00000000 ;source IP
dd 0xFFFFFFFF ;destination IP
.end_of_ip_layer:

;udp layer
dw BIG_ENDAIN(68) ;source port
dw BIG_ENDAIN(67) ;destination port
dw BIG_ENDAIN(.dhcp_layer_end-.dhcp_layer+8) ;length
.udp_crc: dw 0 ;checksum

;dhcp layer - 251 bytes
.dhcp_layer:
db 1 ;request
db 1 ;ethernet
db 6 ;hardware address length
db 0
dd 0x12121212 ;value for this transaction
dw 8 ;num of seconds
dw 0 ;flags
.client_ip: dd 0x00000000 ;our IP address
dd 0x00000000
.server_ip dd 0x00000000
dd 0x00000000
.dhcp_mac: times 16 db 0
times 64 db 0
times 128 db 0
db 99, 130, 83, 99 ;magic value for DHCP

db 0x35, 0x01, 0x03 ;command DHCP request
.command_our_ip: db 0x32, 0x04, 0x00, 0x00, 0x00, 0x00 ;requested IP address
.command_server_ip: db 0x36, 0x04, 0x00, 0x00, 0x00, 0x00 ;DHCP server IP

db 0xFF
.dhcp_layer_end:

dhcp_request_packet_end:

;Domain Name System Protocol - QUERY message
dns_packet:
;ethernet layer
db 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ;receiver MAC address
.ethernet_mac: db 0, 0, 0, 0, 0, 0 ;our MAC address
dw BIG_ENDAIN(0x0800) ;IP

;ip layer
.ip_layer:
db 0x45 ;header
db 0
.ip_length: dw 0 ;total length
dd 0
db 0x80 ;time to live
db 17 ;UDP protocol
.ip_checksum: dw 0 ;checksum
.ip_address: dd 0 ;our IP
.ip_destination: dd 0 ;destination IP
.end_of_ip_layer:

;udp layer
.udp_layer:
dw BIG_ENDAIN(53) ;source port
dw BIG_ENDAIN(53) ;destination port
.udp_length: dw 0 ;length
.udp_checksum: dw 0 ;checksum

;dns layer
.dns_layer:
dw BIG_ENDAIN(0xABCD) ;transaction id
dw BIG_ENDAIN(0x0100) ;flags
dw BIG_ENDAIN(1) ;one question
dw 0
dw 0
dw 0

;data
.dns_data:
times 263 db 0
.dns_layer_end:

dns_packet_end:

;Address Resoultion Protocol - REQUEST message
arp_packet:
;ethernet layer
db 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ;receiver MAC address
.ethernet_mac: db 0, 0, 0, 0, 0, 0 ;our MAC address
dw BIG_ENDAIN(0x0806) ;ARP

;arp layer
dw BIG_ENDAIN(0x1) ;ethernet
dw BIG_ENDAIN(0x0800) ;ip
dw BIG_ENDAIN(0x0604) ;hardware and protocol length
dw BIG_ENDAIN(0x0001) ;request
.arp_source_mac: times 6 db 0
.arp_source_ip: dd 0
.arp_destination_mac: times 6 db 0
.arp_destination_ip: dd 0

times 18 db 0 ;padding

arp_packet_end:

;Address Resoultion Protocol - RESPONSE message
arp_response_packet:
;ethernet layer
.destination_mac: db 0, 0, 0, 0, 0, 0 ;receiver MAC address
.ethernet_mac: db 0, 0, 0, 0, 0, 0 ;our MAC address
dw BIG_ENDAIN(0x0806) ;ARP

;arp layer
dw BIG_ENDAIN(0x1) ;ethernet
dw BIG_ENDAIN(0x0800) ;ip
dw BIG_ENDAIN(0x0604) ;hardware and protocol length
dw BIG_ENDAIN(0x0002) ;response
.arp_source_mac: times 6 db 0
.arp_source_ip: dd 0
.arp_destination_mac: times 6 db 0
.arp_destination_ip: dd 0

times 18 db 0 ;padding

arp_response_packet_end:

;Transmit Control Protocol - control messages without payload
tcp_control_packet:
;ethernet layer
.destination_mac: db 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ;receiver MAC address
.ethernet_mac: db 0, 0, 0, 0, 0, 0 ;our MAC address
dw BIG_ENDAIN(0x0800) ;IP

;ip layer
.ip_layer:
db 0x45 ;header
db 0
dw BIG_ENDAIN(tcp_control_packet_end-tcp_control_packet.ip_layer) ;total length
dw 0
dw 0x0040
db 0x80 ;time to live
db 6 ;TCP protocol
.ip_checksum: dw 0 ;checksum
.ip_address: dd 0 ;our IP
.ip_destination: dd 0 ;destination IP
.end_of_ip_layer:

;tcp layer
.tcp_layer:
dw BIG_ENDAIN(40000) ;source port
dw BIG_ENDAIN(80) ;destination port
.sequence_number: dd 0
.acknowledgment_number: dd 0
db 0x50 ;length of tcp layer
.control: db 0
.window: dw 0x00A0 ;window
.tcp_checksum: dw 0
dw 0 ;urgent pointer
tcp_control_packet_end:

;Transmit Control Protocol - messages with payload
tcp_get_packet:
;ethernet layer
.destination_mac: db 0, 0, 0, 0, 0, 0 ;receiver MAC address
.ethernet_mac: db 0, 0, 0, 0, 0, 0 ;our MAC address
dw BIG_ENDAIN(0x0800) ;IP

;ip layer
.ip_layer:
db 0x45 ;header
db 0
.ip_length: dw 0
dw 0
dw 0x0040
db 0x80 ;time to live
db 6 ;TCP protocol
.ip_checksum: dw 0 ;checksum
.ip_address: dd 0 ;our IP
.ip_destination: dd 0 ;destination IP
.end_of_ip_layer:

;tcp layer
.tcp_layer:
dw BIG_ENDAIN(40000) ;source port
dw BIG_ENDAIN(80) ;destination port
.sequence_number: dd 0
.acknowledgment_number: dd 0
db 0x50 ;length of tcp layer
.control: db 0x18
.window: dw 0xFF00 ;window
.tcp_checksum: dw 0
dw 0 ;urgent pointer

db 'GET '
.http_layer: times 512 db 0
tcp_get_packet_end:

;PSEUDO HEADERS
tcp_pseudo_header:
.ip_address: dd 0 ;our IP
.ip_destination: dd 0 ;destination IP

udp_pseudo_header:
.ip_address: dd 0 ;our IP
.ip_destination: dd 0 ;destination IP

url_for_dns times 256 db 0
tcp_path_to_file times 256 db 0
times 256 db 0
dns_ip_address dd 0

init_network_stack:
 mov al, byte [ethernet_card_mac+5]
 mov byte [dhcp_offer_packet.ethernet_mac+0], al
 mov byte [dhcp_offer_packet.dhcp_mac+0], al
 mov byte [dhcp_request_packet.ethernet_mac+0], al
 mov byte [dhcp_request_packet.dhcp_mac+0], al
 mov byte [dns_packet.ethernet_mac+0], al
 mov byte [arp_packet.ethernet_mac+0], al
 mov byte [arp_packet.arp_source_mac+0], al
 mov byte [arp_response_packet.ethernet_mac+0], al
 mov byte [arp_response_packet.arp_source_mac+0], al
 mov byte [tcp_control_packet.ethernet_mac+0], al
 mov byte [tcp_get_packet.ethernet_mac+0], al
 
 mov al, byte [ethernet_card_mac+4]
 mov byte [dhcp_offer_packet.ethernet_mac+1], al
 mov byte [dhcp_offer_packet.dhcp_mac+1], al
 mov byte [dhcp_request_packet.ethernet_mac+1], al
 mov byte [dhcp_request_packet.dhcp_mac+1], al
 mov byte [dns_packet.ethernet_mac+1], al
 mov byte [arp_packet.ethernet_mac+1], al
 mov byte [arp_packet.arp_source_mac+1], al
 mov byte [arp_response_packet.ethernet_mac+1], al
 mov byte [arp_response_packet.arp_source_mac+1], al
 mov byte [tcp_control_packet.ethernet_mac+1], al
 mov byte [tcp_get_packet.ethernet_mac+1], al
 
 mov al, byte [ethernet_card_mac+3]
 mov byte [dhcp_offer_packet.ethernet_mac+2], al
 mov byte [dhcp_offer_packet.dhcp_mac+2], al
 mov byte [dhcp_request_packet.ethernet_mac+2], al
 mov byte [dhcp_request_packet.dhcp_mac+2], al
 mov byte [dns_packet.ethernet_mac+2], al
 mov byte [arp_packet.ethernet_mac+2], al
 mov byte [arp_packet.arp_source_mac+2], al
 mov byte [arp_response_packet.ethernet_mac+2], al
 mov byte [arp_response_packet.arp_source_mac+2], al
 mov byte [tcp_control_packet.ethernet_mac+2], al
 mov byte [tcp_get_packet.ethernet_mac+2], al
 
 mov al, byte [ethernet_card_mac+2]
 mov byte [dhcp_offer_packet.ethernet_mac+3], al
 mov byte [dhcp_offer_packet.dhcp_mac+3], al
 mov byte [dhcp_request_packet.ethernet_mac+3], al
 mov byte [dhcp_request_packet.dhcp_mac+3], al
 mov byte [dns_packet.ethernet_mac+3], al
 mov byte [arp_packet.ethernet_mac+3], al
 mov byte [arp_packet.arp_source_mac+3], al
 mov byte [arp_response_packet.ethernet_mac+3], al
 mov byte [arp_response_packet.arp_source_mac+3], al
 mov byte [tcp_control_packet.ethernet_mac+3], al
 mov byte [tcp_get_packet.ethernet_mac+3], al
 
 mov al, byte [ethernet_card_mac+1]
 mov byte [dhcp_offer_packet.ethernet_mac+4], al
 mov byte [dhcp_offer_packet.dhcp_mac+4], al
 mov byte [dhcp_request_packet.ethernet_mac+4], al
 mov byte [dhcp_request_packet.dhcp_mac+4], al
 mov byte [dns_packet.ethernet_mac+4], al
 mov byte [arp_packet.ethernet_mac+4], al
 mov byte [arp_packet.arp_source_mac+4], al
 mov byte [arp_response_packet.ethernet_mac+4], al
 mov byte [arp_response_packet.arp_source_mac+4], al
 mov byte [tcp_control_packet.ethernet_mac+4], al
 mov byte [tcp_get_packet.ethernet_mac+4], al
 
 mov al, byte [ethernet_card_mac+0]
 mov byte [dhcp_offer_packet.ethernet_mac+5], al
 mov byte [dhcp_offer_packet.dhcp_mac+5], al
 mov byte [dhcp_request_packet.ethernet_mac+5], al
 mov byte [dhcp_request_packet.dhcp_mac+5], al
 mov byte [dns_packet.ethernet_mac+5], al
 mov byte [arp_packet.ethernet_mac+5], al
 mov byte [arp_packet.arp_source_mac+5], al
 mov byte [arp_response_packet.ethernet_mac+5], al
 mov byte [arp_response_packet.arp_source_mac+5], al
 mov byte [tcp_control_packet.ethernet_mac+5], al
 mov byte [tcp_get_packet.ethernet_mac+5], al
 
 mov esi, dhcp_offer_packet.ip_layer
 mov ecx, 20
 call read_checksum
 mov word [dhcp_offer_packet.ip_checksum], ax
 
 mov esi, dhcp_request_packet.ip_layer
 mov ecx, 20
 call read_checksum
 mov word [dhcp_request_packet.ip_checksum], ax

 ret

;esi = pointer ecx = length
convert_to_other_endain:
 shr ecx, 1 ;div 2
 .convert:
  mov ax, word [esi]
  mov bl, ah
  mov bh, al
  mov word [esi], bx
  add esi, 2
 loop .convert
 
 ret

;esi = pointer ecx = length
read_checksum:
 push ecx
 shr ecx, 1 ;div 2
 mov eax, 0
 mov edx, 0
 .add:
  mov dx, word [esi]
  add eax, edx
  add esi, 2
 loop .add
 
 pop ecx
 test ecx, 0x1
 jz .odd_number
  mov dx, 0
  mov dh, byte [esi]
  add eax, edx
 .odd_number:
 
 mov ebx, eax
 shr ebx, 16
 and eax, 0xFFFF
 add eax, ebx
 mov ebx, eax
 shr ebx, 16
 and eax, 0xFFFF
 add eax, ebx
 not eax
 
 ret
 
;esi = pointer ecx = length
read_checksum_tcp:
 push ecx
 mov eax, 0x0600
 shl ecx, 8
 add eax, ecx
 shr ecx, 9 ;div 2
 mov edx, 0
 mov dx, word [tcp_pseudo_header.ip_address]
 add eax, edx
 mov dx, word [tcp_pseudo_header.ip_address+2]
 add eax, edx
 mov dx, word [tcp_pseudo_header.ip_destination]
 add eax, edx
 mov dx, word [tcp_pseudo_header.ip_destination+2]
 add eax, edx
 .add:
  mov dx, word [esi]
  add eax, edx
  add esi, 2
 loop .add
 
 pop ecx
 test ecx, 0x1
 jz .odd_number
  mov dx, 0
  mov dl, byte [esi]
  add eax, edx
 .odd_number:
 
 mov ebx, eax
 shr ebx, 16
 and eax, 0xFFFF
 add eax, ebx
 mov ebx, eax
 shr ebx, 16
 and eax, 0xFFFF
 add eax, ebx
 not eax
 
 ret
 
;esi = pointer ecx = length
read_checksum_udp:
 push ecx
 mov eax, 0x1100
 shl ecx, 8
 add eax, ecx
 shr ecx, 9 ;div 2
 mov edx, 0
 mov dx, word [udp_pseudo_header.ip_address]
 add eax, edx
 mov dx, word [udp_pseudo_header.ip_address+2]
 add eax, edx
 mov dx, word [udp_pseudo_header.ip_destination]
 add eax, edx
 mov dx, word [udp_pseudo_header.ip_destination+2]
 add eax, edx
 .add:
  mov dx, word [esi]
  add eax, edx
  add esi, 2
 loop .add
 
 pop ecx
 test ecx, 0x1
 jz .odd_number
  mov dx, 0
  mov dl, byte [esi]
  add eax, edx
 .odd_number:
 
 mov ebx, eax
 shr ebx, 16
 and eax, 0xFFFF
 add eax, ebx
 mov ebx, eax
 shr ebx, 16
 and eax, 0xFFFF
 add eax, ebx
 not eax
 
 ret
 
ethernet_card_process_packet: 
 mov esi, dword [received_packet_pointer]

 cmp dword [esi], 0xFFFFFFFF
 je .compare_broadcast
 mov eax, 0
 mov al, byte [ethernet_card_mac+5]
 cmp byte [esi+0], al
 jne .done
 mov al, byte [ethernet_card_mac+4]
 cmp byte [esi+1], al
 jne .done
 mov al, byte [ethernet_card_mac+3]
 cmp byte [esi+2], al
 jne .done
 mov al, byte [ethernet_card_mac+2]
 cmp byte [esi+3], al
 jne .done
 mov al, byte [ethernet_card_mac+1]
 cmp byte [esi+4], al
 jne .done
 mov al, byte [ethernet_card_mac+0]
 cmp byte [esi+5], al
 jne .done
 jmp .packet
 .compare_broadcast:
 cmp word [esi+4], 0xFFFF
 jne .done
 
 .packet:
 mov dword [type_of_received_packet], 0
 cmp word [esi+12], 0x0608 ;ARP packet
 je .arp
 cmp word [esi+12], 0x0008 ;IP packet
 je .ip
 jmp .done
 
 .arp:
  cmp word [esi+20], 0x0100
  je .arp_request
  mov dword [type_of_received_packet], ARP_REPLY
  cmp word [esi+20], 0x0200
  je .print
  mov dword [type_of_received_packet], 0
 jmp .done
 
 .arp_request:
  mov dword [type_of_received_packet], ARP_REQUEST
  mov eax, dword [esi+28]
  mov dword [arp_response_packet.arp_destination_ip], eax
  mov al, byte [esi+6]
  mov byte [arp_response_packet.arp_destination_mac+0], al
  mov byte [arp_response_packet.destination_mac+0], al
  mov al, byte [esi+7]
  mov byte [arp_response_packet.arp_destination_mac+1], al
  mov byte [arp_response_packet.destination_mac+1], al
  mov al, byte [esi+8]
  mov byte [arp_response_packet.arp_destination_mac+2], al
  mov byte [arp_response_packet.destination_mac+2], al
  mov al, byte [esi+9]
  mov byte [arp_response_packet.arp_destination_mac+3], al
  mov byte [arp_response_packet.destination_mac+3], al
  mov al, byte [esi+10]
  mov byte [arp_response_packet.arp_destination_mac+4], al
  mov byte [arp_response_packet.destination_mac+4], al
  mov al, byte [esi+11]
  mov byte [arp_response_packet.arp_destination_mac+5], al
  mov byte [arp_response_packet.destination_mac+5], al
  
  mov dword [packet_pointer], arp_response_packet
  mov dword [packet_length], arp_response_packet_end-arp_response_packet
  call nic_send_packet ;send ARP response packet
 jmp .done
 
 .ip:
  cmp byte [esi+14+9], 6
  je .tcp
  cmp byte [esi+14+9], 17
  je .udp
  mov dword [type_of_received_packet], 0
 jmp .done
 
 .tcp:
  cmp byte [esi+14+20+13], 0x12
  je .tcp_handshake
  cmp byte [esi+14+20+13], 0x18
  je .http_response
  cmp byte [esi+14+20+13], 0x10
  je .tcp_ack
  cmp byte [esi+14+20+13], 0x11
  je .tcp_final
 jmp .done
 
 .tcp_handshake:
  mov dword [type_of_received_packet], TCP_HANDSHAKE
  mov al, byte [esi+6]
  mov byte [tcp_get_packet.destination_mac+0], al
  mov byte [tcp_control_packet.destination_mac+0], al
  mov al, byte [esi+7]
  mov byte [tcp_get_packet.destination_mac+1], al
  mov byte [tcp_control_packet.destination_mac+1], al
  mov al, byte [esi+8]
  mov byte [tcp_get_packet.destination_mac+2], al
  mov byte [tcp_control_packet.destination_mac+2], al
  mov al, byte [esi+9]
  mov byte [tcp_get_packet.destination_mac+3], al
  mov byte [tcp_control_packet.destination_mac+3], al
  mov al, byte [esi+10]
  mov byte [tcp_get_packet.destination_mac+4], al
  mov byte [tcp_control_packet.destination_mac+4], al
  mov al, byte [esi+11]
  mov byte [tcp_get_packet.destination_mac+5], al
  mov byte [tcp_control_packet.destination_mac+5], al
  
  mov eax, dword [esi+14+20+8] ;acknowledgment number
  mov bh, byte [esi+14+20+4] ;sequence number
  mov bl, byte [esi+14+20+5]
  shl ebx, 16
  mov bh, byte [esi+14+20+6]
  mov bl, byte [esi+14+20+7]

  inc ebx
  mov dh, bl
  mov dl, bh
  shl edx, 16
  shr ebx, 16
  mov dh, bl
  mov dl, bh
  mov dword [tcp_control_packet.sequence_number], eax
  mov dword [tcp_control_packet.acknowledgment_number], edx
  mov dword [tcp_get_packet.sequence_number], eax
  mov dword [tcp_get_packet.acknowledgment_number], edx
  mov dword [tcp_last_ack_number], edx
  
  mov dword [tcp_communication_type], TCP_HANDSHAKE_RECEIVED
  
  mov word [tcp_control_packet.tcp_checksum], 0
  mov byte [tcp_control_packet.control], (1 << 4) ;ACK flag
  mov esi, tcp_control_packet.tcp_layer
  mov ecx, 20
  call read_checksum_tcp
  mov word [tcp_control_packet.tcp_checksum], ax
  mov dword [packet_pointer], tcp_control_packet
  mov dword [packet_length], tcp_control_packet_end-tcp_control_packet
  call nic_send_packet
 jmp .done
 
 .http_response:
  mov dword [type_of_received_packet], TCP_HTTP

  cmp dword [tcp_communication_type], TCP_REQUEST_FOR_FILE
  jb .done
  cmp dword [tcp_communication_type], TCP_FINALIZING
  je .done
  
  mov edi, dword [http_file_pointer]
  mov ecx, 0
  mov ch, byte [esi+14+2]
  mov cl, byte [esi+14+3]
  sub ecx, 40 ;length of TCP data
  
  push esi
  add esi, 14+20+20
  mov edx, 0
  cmp dword [esi], 'HTTP'
  jne .copy_http_file
   .find_http_file_start:
    cmp dword [esi], 0
    je .done
    
    cmp dword [esi], 'Cont'
    jne .if_content_length
    cmp dword [esi+4], 'ent-'
    jne .if_content_length
    cmp dword [esi+8], 'Leng'
    jne .if_content_length
    cmp dword [esi+12], 'th: '
    jne .if_content_length
     pusha
     add esi, 16
     mov ecx, 0
     .content_length_convert_to_number:
      cmp word [esi], 0x0A0D
      je .save_content_length
      mov eax, ecx
      mov ebx, 10
      mul ebx
      mov ecx, eax
      mov eax, 0
      mov al, byte [esi]
      sub eax, '0'
      add ecx, eax
      inc esi
     jmp .content_length_convert_to_number
     .save_content_length:
     cmp ecx, 0
     jne .if_content_length_zero
      mov ecx, 0x100000
     .if_content_length_zero:
     mov dword [tcp_file_length], ecx
     popa
    .if_content_length:
    
    cmp dword [esi], 0x0A0D0A0D
    je .copy_http_file_header
    inc esi
    inc edx
   jmp .find_http_file_start
  .if_http_start:
  .copy_http_file_header:
  add esi, 4
  add edx, 4
  .copy_http_file:
  push ecx
  rep movsb 
  pop ecx
  sub ecx, edx
  add dword [http_file_pointer], ecx
  add dword [tcp_file_transferred_length], ecx
  pop esi

  mov eax, dword [esi+14+20+8] ;acknowledgment number
  mov bh, byte [esi+14+20+4] ;sequence number
  mov bl, byte [esi+14+20+5]
  shl ebx, 16
  mov bh, byte [esi+14+20+6]
  mov bl, byte [esi+14+20+7]
  
  mov ecx, 0
  mov ch, byte [esi+14+2]
  mov cl, byte [esi+14+3]
  sub ecx, 40 ;length of TCP data
  
  add ebx, ecx
  mov dh, bl
  mov dl, bh
  shl edx, 16
  shr ebx, 16
  mov dh, bl
  mov dl, bh
  mov dword [tcp_control_packet.sequence_number], eax
  mov dword [tcp_control_packet.acknowledgment_number], edx
  mov dword [tcp_get_packet.sequence_number], eax
  mov dword [tcp_get_packet.acknowledgment_number], edx
  
  mov al, byte [esi+6]
  mov byte [tcp_control_packet.destination_mac+0], al
  mov al, byte [esi+7]
  mov byte [tcp_control_packet.destination_mac+1], al
  mov al, byte [esi+8]
  mov byte [tcp_control_packet.destination_mac+2], al
  mov al, byte [esi+9]
  mov byte [tcp_control_packet.destination_mac+3], al
  mov al, byte [esi+10]
  mov byte [tcp_control_packet.destination_mac+4], al
  mov al, byte [esi+11]
  mov byte [tcp_control_packet.destination_mac+5], al

  mov word [tcp_control_packet.tcp_checksum], 0
  mov byte [tcp_control_packet.control], (1 << 4) ;ACK flag
  mov esi, tcp_control_packet.tcp_layer
  mov ecx, 20
  call read_checksum_tcp
  mov word [tcp_control_packet.tcp_checksum], ax
  mov dword [packet_pointer], tcp_control_packet
  mov dword [packet_length], tcp_control_packet_end-tcp_control_packet
  
  cmp dword [last_arrived_packet], 1
  jne .if_last_packet
  call nic_send_packet ;send ACK packet
  .if_last_packet:
  
  mov dword [tcp_communication_type], TCP_FILE_TRANSFERRING
  
  cmp dword [tcp_file_transferred_length], eax
  jb .done
  mov dword [tcp_communication_type], TCP_FINALIZING
 jmp .done
 
 .tcp_ack:
  cmp dword [tcp_communication_type], TCP_FILE_TRANSFERRING
  je .http_response
 jmp .done
 
 .tcp_final:
  mov dword [type_of_received_packet], TCP_END
  mov al, byte [esi+6]
  mov byte [tcp_get_packet.destination_mac+0], al
  mov al, byte [esi+7]
  mov byte [tcp_get_packet.destination_mac+1], al
  mov al, byte [esi+8]
  mov byte [tcp_get_packet.destination_mac+2], al
  mov al, byte [esi+9]
  mov byte [tcp_get_packet.destination_mac+3], al
  mov al, byte [esi+10]
  mov byte [tcp_get_packet.destination_mac+4], al
  mov al, byte [esi+11]
  mov byte [tcp_get_packet.destination_mac+5], al
  
  mov eax, dword [esi+14+20+8] ;acknowledgment number
  mov bh, byte [esi+14+20+4] ;sequence number
  mov bl, byte [esi+14+20+5]
  shl ebx, 16
  mov bh, byte [esi+14+20+6]
  mov bl, byte [esi+14+20+7]

  inc ebx
  mov dh, bl
  mov dl, bh
  shl edx, 16
  shr ebx, 16
  mov dh, bl
  mov dl, bh
  mov dword [tcp_control_packet.sequence_number], eax
  mov dword [tcp_control_packet.acknowledgment_number], edx
  mov dword [tcp_get_packet.sequence_number], eax
  mov dword [tcp_get_packet.acknowledgment_number], edx
  
  mov ax, word [esi+14+20+14]
  mov word [tcp_control_packet.window], ax
  mov word [tcp_control_packet.tcp_checksum], 0
  mov byte [tcp_control_packet.control], 0x11 ;FIN and ACK flag
  mov esi, tcp_control_packet.tcp_layer
  mov ecx, 20
  call read_checksum_tcp
  mov word [tcp_control_packet.tcp_checksum], ax
  mov dword [packet_pointer], tcp_control_packet
  mov dword [packet_length], tcp_control_packet_end-tcp_control_packet
  call nic_send_packet ;send ACK packet
  
  mov dword [tcp_communication_type], TCP_FINALIZING
 jmp .done
 
 .udp:
  cmp dword [esi+14+20+0], ((68 << 24) | (67 << 8))
  je .dhcp
  cmp dword [esi+14+20+0], ((53 << 24) | (53 << 8))
  je .dns
 jmp .done
 
 .dhcp:
  mov ecx, 0
  mov ebx, esi
  add ebx, 14+20+8+240
  .dhcp_find_packet_type:
   cmp byte [ebx], 0x35
   je .dhcp_packet_type_founded
   cmp byte [ebx], 0xFF
   je .done
   cmp byte [ebx], 0
   je .done
   mov cl, byte [ebx+1]
   add ebx, ecx
  jmp .dhcp_find_packet_type
   
  .dhcp_packet_type_founded:
  mov eax, dword [ebx]
  and eax, 0x00FFFFFF
  cmp eax, 0x020135
  je .dhcp_offer
  cmp eax, 0x050135
  je .dhcp_acknowledge
  cmp eax, 0x060135
  je .dhcp_nak
 jmp .done
 
 .dhcp_offer:
  mov dword [type_of_received_packet], DHCP_OFFER
  
  mov eax, 0
  mov edi, esi
  add edi, 14+20+8+240
  .dhcp_offer_find_server_identification:
   cmp byte [edi], 0
   je .wrong_dhcp_offer
   cmp byte [edi], 0xFF
   je .dhcp_set_values
   cmp byte [edi], 3
   jne .dhcp_server_ip
    mov ebx, dword [edi+2]
   .dhcp_server_ip:
   cmp byte [edi], 6
   jne .dhcp_dns_server_ip
    mov ecx, dword [edi+2]
   .dhcp_dns_server_ip:
   
   mov al, byte [edi+1]
   add eax, 2
   add edi, eax
  jmp .dhcp_offer_find_server_identification

  .dhcp_set_values:
  mov eax, dword [esi+14+20+8+16] ;our IP address
  
  mov dword [dhcp_request_packet.client_ip], eax
  mov dword [dhcp_request_packet.server_ip], ebx
  mov dword [dhcp_request_packet.command_our_ip+2], eax
  mov dword [dhcp_request_packet.command_server_ip+2], ebx
  
  mov dword [arp_packet.arp_source_ip], eax
  mov dword [arp_response_packet.arp_source_ip], eax
  
  mov dword [udp_pseudo_header.ip_address], eax
  mov dword [tcp_control_packet.ip_address], eax
  mov dword [tcp_pseudo_header.ip_address], eax
  mov dword [tcp_get_packet.ip_address], eax
  
  mov dword [dns_packet.ip_address], eax
  mov dword [dns_packet.ip_destination], ecx
 jmp .done
 
 .dhcp_acknowledge:
  mov dword [type_of_received_packet], DHCP_ACK
 jmp .done
 
 .dhcp_nak:
  mov dword [type_of_received_packet], DHCP_NAK
 jmp .done
 
 .wrong_dhcp_offer:
  ;TODO
 jmp .done
 
 .dns:
  cmp word [esi+14+20+8+0], 0xCDAB
  jne .done
  mov dword [type_of_received_packet], DNS_RESPONSE
  mov al, byte [esi+14+20+8+3]
  and al, 0x7
  cmp al, 0x3
  je .dns_no_ip
  add esi, 14+20+8+12
  .dns_skip_question:
   mov ebx, 0
   mov bl, byte [esi]
   cmp ebx, 0
   je .dns_skip_answers
   inc ebx
   add esi, ebx
  jmp .dns_skip_question
  
  .dns_skip_answers:
  add esi, 5
  .dns_found_answer:
   cmp word [esi], 0
   je .dns_no_ip
   cmp word [esi+2], 0x0100
   je .dns_read_ip
   mov eax, 0
   mov al, byte [esi+11]
   mov ah, byte [esi+10]
   add eax, 12
   add esi, eax
  jmp .dns_found_answer
  
  .dns_read_ip:
  add esi, 12
  mov eax, dword [esi]
  mov dword [dns_ip_address], eax
  mov dword [dns_report], 1
 jmp .done
 
 .dns_no_ip:
  mov dword [dns_report], 2
 jmp .done
 
 .print:
  cmp dword [type_of_received_packet], ARP_REQUEST
  jne .if_arp_request
   PSTR 'ARP request', arp_request_str
  .if_arp_request:
  
  cmp dword [type_of_received_packet], ARP_REPLY
  jne .if_arp_reply
   PSTR 'ARP reply', arp_reply_str
  .if_arp_reply:
  
  cmp dword [type_of_received_packet], DHCP_OFFER
  jne .if_dhcp_offer
   PSTR 'DHCP offer', dhcp_offer_str
  .if_dhcp_offer:
  
  cmp dword [type_of_received_packet], DHCP_ACK
  jne .if_dhcp_ack
   PSTR 'DHCP ack', dhcp_ack_str
  .if_dhcp_ack:
  
  cmp dword [type_of_received_packet], DHCP_NAK
  jne .if_dhcp_nak
   PSTR 'DHCP nak', dhcp_nak_str
  .if_dhcp_nak:
  
  cmp dword [type_of_received_packet], DNS_RESPONSE
  jne .if_dns_response
   PSTR 'DNS response', dns_response_str
  .if_dns_response:
  
  cmp dword [type_of_received_packet], TCP_HANDSHAKE
  jne .if_tcp_handshake
   PSTR 'TCP handshake', tcp_handshake_str
  .if_tcp_handshake:
  
  cmp dword [type_of_received_packet], TCP_HTTP
  jne .if_tcp_http
   PSTR 'TCP HTTP', tcp_http_str
  .if_tcp_http:
 
 .done:
 ret
 
connect_to_network:
 mov dword [type_of_received_packet], 0
 mov dword [internet_connection_status], 0
 
 mov dword [packet_pointer], dhcp_offer_packet
 mov dword [packet_length], dhcp_offer_packet_end-dhcp_offer_packet
 call nic_send_packet
 
 mov dword [ticks], 0
 .wait_for_dhcp_offer:
  cmp dword [type_of_received_packet], DHCP_OFFER
  je .dhcp_request
 cmp dword [ticks], 1000
 jb .wait_for_dhcp_offer
 mov dword [internet_connection_status], 2 ;error
 jmp .done
 
 .dhcp_request:
 mov dword [packet_pointer], dhcp_request_packet
 mov dword [packet_length], dhcp_request_packet_end-dhcp_request_packet
 call nic_send_packet
 
 mov dword [ticks], 0
 .wait_for_dhcp_ack:
  cmp dword [type_of_received_packet], DHCP_ACK
  je .connection_succesful
 cmp dword [ticks], 1000
 jb .wait_for_dhcp_ack
 mov dword [internet_connection_status], 2 ;error
 jmp .done
 
 .connection_succesful:
 mov dword [internet_connection_status], 1
  
 .done: 
 ret

get_ip_address_of_url:
 mov word [dns_packet.ip_checksum], 0
 mov word [dns_packet.udp_checksum], 0

 mov edx, 0
 mov eax, url_for_dns
 mov esi, url_for_dns
 mov edi, dns_packet.dns_data
 .next_entry:
 mov ebx, 0
 .find_length_of_entry:
  cmp byte [eax], 0
  je .create_entry_of_dns_packet
  cmp byte [eax], '.'
  je .create_entry_of_dns_packet
  inc eax
  inc ebx
  inc dh
 jmp .find_length_of_entry
 .create_entry_of_dns_packet:
 mov byte [edi], bl
 mov ecx, ebx
 .copy_string:
  inc edi
  mov dl, byte [esi]
  mov byte [edi], dl
  inc esi
 loop .copy_string
 inc edi
 inc esi
 cmp byte [eax], 0
 je .create_end_of_dns_packet
 inc eax
 jmp .next_entry
  
 .create_end_of_dns_packet:
 mov byte [edi], 0
 inc edi
 mov dword [edi], 0x01000100
 
 ;calculate length of packet
 add dh, 28
 mov dl, 0
 mov word [dns_packet.udp_length], dx
 add dh, 20
 mov word [dns_packet.ip_length], dx

 push edx
 mov esi, dns_packet.ip_layer
 mov ecx, 20
 call read_checksum
 mov word [dns_packet.ip_checksum], ax
 pop edx
 
 mov eax, dword [dns_packet.ip_destination]
 mov dword [udp_pseudo_header.ip_destination], eax
 mov esi, dns_packet.udp_layer
 mov dl, dh
 mov dh, 0
 add edx, 14
 mov ecx, edx
 sub ecx, 34
 push edx
 call read_checksum_udp
 mov word [dns_packet.udp_checksum], ax

 mov dword [packet_pointer], dns_packet
 pop edx
 mov dword [packet_length], edx
 call nic_send_packet ;send DNS packet
 
 .done: 
 ret

create_tcp_connection:
 cmp dword [tcp_communication_type], NO_TCP
 jne .done
 
 mov word [tcp_control_packet.ip_checksum], 0
 mov word [tcp_control_packet.tcp_checksum], 0
 
 mov dword [tcp_control_packet.destination_mac], 0xFFFFFFFF
 mov word [tcp_control_packet.destination_mac+4], 0xFFFF

 mov eax, dword [dns_ip_address]
 mov dword [tcp_control_packet.ip_destination], eax
 mov dword [tcp_pseudo_header.ip_destination], eax
 mov dword [tcp_get_packet.ip_destination], eax
 mov esi, tcp_control_packet.ip_layer
 mov ecx, 20
 call read_checksum
 mov word [tcp_control_packet.ip_checksum], ax
 
 mov dword [tcp_control_packet.sequence_number], 0
 mov dword [tcp_control_packet.acknowledgment_number], 0
 mov byte [tcp_control_packet.control], (1 << 1) ;SYN flag
 mov esi, tcp_control_packet.tcp_layer
 mov ecx, 20
 call read_checksum_tcp
 mov word [tcp_control_packet.tcp_checksum], ax
 mov dword [packet_pointer], tcp_control_packet
 mov dword [packet_length], tcp_control_packet_end-tcp_control_packet
 call nic_send_packet
 
 mov dword [tcp_communication_type], TCP_WAITING_FOR_HANDSHAKE
 
 mov dword [ticks], 0
 .wait_for_tcp_handshake:
  cmp dword [tcp_communication_type], TCP_HANDSHAKE_RECEIVED
  je .tcp_ack_handshake
 cmp dword [ticks], 1000
 jb .wait_for_tcp_handshake
 jmp .done
 
 .tcp_ack_handshake:
 
 .done:
 ret

tcp_transfer_file:
 mov dword [tcp_communication_type], TCP_REQUEST_FOR_FILE
 mov dword [tcp_file_length], 0
 mov dword [tcp_file_transferred_length], 0
 mov word [tcp_get_packet.ip_checksum], 0
 mov word [tcp_get_packet.tcp_checksum], 0
 
 ;add path to file to TCP packet
 mov eax, tcp_path_to_file
 mov ecx, 0
 mov edx, 0
 .find_length_of_path:
  cmp byte [eax], 0
  je .copy_path
  inc eax
  inc ecx
  inc edx
 jmp .find_length_of_path
 .copy_path:
 mov esi, tcp_path_to_file
 mov edi, tcp_get_packet.http_layer
 rep movsb
 mov byte [edi], ' '
 mov dword [edi+1], 'HTTP'
 mov dword [edi+5], '/1.1'
 mov word [edi+9], 0x0A0D
 mov dword [edi+11], 'Host'
 mov word [edi+15], ': '
 add edi, 17
 add edx, 17
 
 ;add address to TCP packet
 mov eax, url_for_dns
 mov ecx, 0
 .find_length_of_address:
  cmp byte [eax], 0
  je .copy_address
  inc eax
  inc ecx
  inc edx
 jmp .find_length_of_address
 .copy_address:
 add edx, 4 ;length of our created HTTP layer
 push edx
 push edx
 push edx
 mov esi, url_for_dns
 rep movsb
 mov dword [edi], 0x0A0D0A0D ;final sequence
 
 pop edx
 add edx, tcp_get_packet.http_layer-tcp_get_packet.ip_layer
 mov byte [tcp_get_packet.ip_length], dh
 mov byte [tcp_get_packet.ip_length+1], dl
 mov esi, tcp_get_packet.ip_layer
 mov ecx, tcp_get_packet.end_of_ip_layer-tcp_get_packet.ip_layer
 call read_checksum
 
 mov word [tcp_get_packet.ip_checksum], ax
 mov esi, tcp_get_packet.tcp_layer
 mov ecx, tcp_get_packet.http_layer-tcp_get_packet.tcp_layer
 pop edx
 add ecx, edx
 call read_checksum_tcp
 mov word [tcp_get_packet.tcp_checksum], ax
 
 mov dword [packet_pointer], tcp_get_packet
 mov dword [packet_length], tcp_get_packet.http_layer-tcp_get_packet
 pop edx
 add dword [packet_length], edx
 call nic_send_packet

 .done:
 ret
